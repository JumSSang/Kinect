#pragma once
#ifndef _SKINNEDMESH_H_
#define _SKINNEDMESH_H_


#include <windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <NuiApi.h>
#include <string>
#include "stdafx.h"

#define TIME_DELTA 0.001f

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }


enum METHOD
{
	D3DNONINDEXED,
	D3DINDEXED,
	SOFTWARE,
	D3DINDEXEDVS,
	D3DINDEXEDHLSLVS,
	NONE
};

//Ŭ���� ����
class CSkinnedMesh;

//--------------------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//--------------------------------------------------------------------------------------
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
	D3DXMATRIXA16        CombinedTransformationMatrix;
	D3DXMATRIXA16		 KinectMatrix;
	bool bKinected;
	
	inline D3DXFRAME_DERIVED()
	{
		D3DXMatrixIdentity(&KinectMatrix);
		bKinected = false;
	}
};


//--------------------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//--------------------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    

	// SkinMesh info             
	LPD3DXMESH           pOrigMesh;
	LPD3DXATTRIBUTERANGE pAttributeTable;
	DWORD                NumAttributeGroups; 
	DWORD                NumInfl;
	LPD3DXBUFFER         pBoneCombinationBuf;
	D3DXMATRIX**         ppBoneMatrixPtrs;
	D3DXMATRIX*          pBoneOffsetMatrices;
	DWORD                NumPaletteEntries;
	bool                 UseSoftwareVP;
	DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
};


//--------------------------------------------------------------------------------------
// Name: class CAllocateHierarchy
// Desc: D3DXLoadMeshHierarchyFromX�� �Ž��� ������ �ڵ����� �����ϴ� Ŭ���� 
//       �⺻���� DirextX���� ���� ������ �������ؼ� ��ߵȴ�. 
//--------------------------------------------------------------------------------------
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
	//������ ����.
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	//�Ž� �����̳� ����
	STDMETHOD(CreateMeshContainer)(THIS_ 
		LPCSTR Name, 
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials, 
		CONST D3DXEFFECTINSTANCE *pEffectInstances, 
		DWORD NumMaterials, 
		CONST DWORD *pAdjacency, 
		LPD3DXSKININFO pSkinInfo, 
		LPD3DXMESHCONTAINER *ppNewMeshContainer);
	//������ ����
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);	
	//�Ž� �����̳� ����
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
	//�Ž������ִ� ���� �̸��� �о pNewName�� ����.
	HRESULT AllocateName(LPCSTR Name, LPSTR* pNewName);
	//CAllocateHierarchy���� CSKinnedMesh Ŭ������ ����ϱ� ���ؼ� 
	CAllocateHierarchy(CSkinnedMesh *pApp) : m_pApp(pApp) {}
	CAllocateHierarchy() {}

public:
	CSkinnedMesh* m_pApp;
};

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh
// Desc: ��Ų�� �Ž� �ε�� ��ο� ���� ������ �������� �κ��� ����
//       
//--------------------------------------------------------------------------------------

class CSkinnedMesh
{
public:
	LPDIRECT3DDEVICE9			m_pD3DDevice;
	LPD3DXFONT			        m_pFont;         // Font for drawing text
	LPD3DXEFFECT				m_pEffect;       // D3DX effect interface
	bool						m_bShowHelp;     // If true, it renders the UI control text
	LPD3DXFRAME					m_pFrameRoot;
	ID3DXAnimationController*   g_pAnimController;
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;

	D3DXVECTOR3					m_vObjectCenter;        // �ٿ�����Ǿ��� �߽ɰ� ���
	FLOAT						m_fObjectRadius;        // �ٿ�����Ǿ��� �ݰ� ���
	METHOD						m_SkinningMethod;		 // Current skinning method
	D3DXMATRIXA16*				m_pBoneMatrices;
	UINT						m_NumBoneMatricesMax;
	LPDIRECT3DVERTEXSHADER9		m_pIndexedVertexShader[4];
	D3DXMATRIXA16				m_matView;              // View matrix
	D3DXMATRIXA16				m_matWorld;
	DWORD						m_dwBehaviorFlags;      // Behavior flags of the 3D device
	bool						m_bUseSoftwareVP;       // Flag to indicate whether software vp is
	//D3DXMESHDATA				MeshData;
	ID3DXMesh					*pObjMesh;
	D3DXFRAME_DERIVED			*SkinFrame;
	D3DXMATRIX					**ppFrameMatrix;
	D3DXMATRIX					*pBoneMatrix;
	float Ang;

	unsigned int m_currentAnimationSet;	
	unsigned int m_numAnimationSets;
	unsigned int m_currentTrack;
	float m_currentTime;
	float m_speedAdjust;
	std::string m_filename;

public:

	CSkinnedMesh();
	~CSkinnedMesh();
	//�Ž��� �ε� �Ѵ�.
	HRESULT LoadMesh(LPDIRECT3DDEVICE9 pd3dDevice, const std::string &filename);
	//��Ų �Ž��� �����Ѵ�
	HRESULT GenerateSkinnedMesh( D3DXMESHCONTAINER_DERIVED* pMeshContainer);
	//�Ž� �����̳ʸ� ������Ѵ�.
	void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	//�Ž��� ȭ�鿡 �Ѹ�.
	void DrawFrame(LPD3DXFRAME pFrame );
	//�������� �̵� �ִϸ��̼�
	void FrameMove(float elapsedTime);
	//�ð� ���.
	float GetElapsedTime();
	//�޽������̳ʿ� ��Ʈ������ �޾��ִ� �Լ�
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );
	//Ʈ�� ������ ���� ���鼭 �޽������̳ʿ� ���� ��ȯ ��Ʈ������ �Ҵ�
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	//�ִϸ��̼� ƽ�� �°� ��Ʈ������ ������Ʈ
	void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParent, LPD3DXMATRIX pKinected);
	//������
	void ReleaseAttributeTable( LPD3DXFRAME pFrameBase );
	//����
	void MeshRender();
	D3DXFRAME_DERIVED * FindDXFrame(const char *FrameName, D3DXFRAME_DERIVED* Frame);
	void SetMatrix(D3DXMATRIX *matTransformation, D3DXFRAME_DERIVED* Frame);

	void SetWorldMatrix(const D3DXMATRIX &Matrix);
	void SetKinectMatrix(unsigned int index, const D3DXMATRIX &Matrix);
	void BoneTranslation(TCHAR* boneName, float x, float y, float z);

	unsigned int GetCurrentAnimationSet() const {return m_currentAnimationSet;}
	std::string GetAnimationSetName(unsigned int index);
	std::string GetFilename() const {return m_filename;}
};


#endif _SKINNEDMESH_H_
