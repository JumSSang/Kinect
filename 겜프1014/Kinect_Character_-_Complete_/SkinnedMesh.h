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

//클레스 선언
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
// Desc: D3DXLoadMeshHierarchyFromX로 매쉬를 읽을때 자동으로 실행하는 클래스 
//       기본형은 DirextX에서 지원 나머진 재정의해서 써야된다. 
//--------------------------------------------------------------------------------------
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
	//프레임 생성.
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	//매쉬 콘테이너 생성
	STDMETHOD(CreateMeshContainer)(THIS_ 
		LPCSTR Name, 
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials, 
		CONST D3DXEFFECTINSTANCE *pEffectInstances, 
		DWORD NumMaterials, 
		CONST DWORD *pAdjacency, 
		LPD3DXSKININFO pSkinInfo, 
		LPD3DXMESHCONTAINER *ppNewMeshContainer);
	//프레임 종료
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);	
	//매쉬 콘테이너 종료
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
	//매쉬에서있는 본의 이름을 읽어서 pNewName에 저장.
	HRESULT AllocateName(LPCSTR Name, LPSTR* pNewName);
	//CAllocateHierarchy에서 CSKinnedMesh 클레스를 사용하기 위해서 
	CAllocateHierarchy(CSkinnedMesh *pApp) : m_pApp(pApp) {}
	CAllocateHierarchy() {}

public:
	CSkinnedMesh* m_pApp;
};

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh
// Desc: 스킨드 매쉬 로드및 드로우 뼈대 생성등 전반적인 부분을 관리
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

	D3DXVECTOR3					m_vObjectCenter;        // 바운딩스피어의 중심값 계산
	FLOAT						m_fObjectRadius;        // 바운딩스피어의 반경 계산
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
	//매쉬를 로드 한다.
	HRESULT LoadMesh(LPDIRECT3DDEVICE9 pd3dDevice, const std::string &filename);
	//스킨 매쉬를 생성한다
	HRESULT GenerateSkinnedMesh( D3DXMESHCONTAINER_DERIVED* pMeshContainer);
	//매쉬 컨테이너를 드로윙한다.
	void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	//매쉬를 화면에 뿌림.
	void DrawFrame(LPD3DXFRAME pFrame );
	//프레임을 이동 애니메이션
	void FrameMove(float elapsedTime);
	//시간 얻기.
	float GetElapsedTime();
	//메시컨테이너에 매트릭스를 달아주는 함수
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );
	//트리 구조의 뼈를 돌면서 메시컨테이너에 최종 변환 매트릭스를 할당
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	//애니메이션 틱에 맞게 매트릭스를 업데이트
	void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParent, LPD3DXMATRIX pKinected);
	//릴리즈
	void ReleaseAttributeTable( LPD3DXFRAME pFrameBase );
	//랜더
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
