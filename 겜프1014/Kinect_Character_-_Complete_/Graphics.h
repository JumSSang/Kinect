#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "SkinnedMesh.h"
/*
	The CGraphics class provides all the graphics for the demo
	It has code to set up Direct3D and load x files via the CXFileEntity class
*/

struct POLAR
{
	float x, y, z;
	float angle;
	float radius;
	float RotationY;
};

class CGraphics
{
public:
	// Direct3D objects
	ID3DXFont * m_font;
	LPDIRECT3D9				g_pD3D; // Direct3D 객체
	LPDIRECT3DDEVICE9		g_pd3dDevice; // 랜더링 장치 (비디오카드)

	// Internal objects	
	CSkinnedMesh* m_entity;
	bool g_bFound;

	bool m_displayControls;	
	void DisplayText() const;

	CGraphics(void);
	~CGraphics(void);
	bool Initialise(HWND hWnd);
	void SetupCamera();
	void Update();
	bool LoadSXFile();
	void InputKey();
	
	void SetWorldMatrix(int entityIndex,const D3DXMATRIX& mat);
	void SetKinectMatrix(int entityIndex,const D3DXMATRIX* mat);

	void BoneTranslation(int index, float x, float y, float z);
	void CharacterMove(float RotateY, float x, float y, float z);
	void TranslationValue(int index, int start, int end, float x, float y, float z);
	void ToggleControlsDisplay();
};
