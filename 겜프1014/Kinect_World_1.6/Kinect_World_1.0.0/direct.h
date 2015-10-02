#include <d3d9.h>
#include <d3dx9.h>
#include <math.h>
#include "openCV.h"




class DirectX :public OpenCV
{
private:
	IDirect3DTexture9 *m_pVideoTexture;
	LPDIRECT3DTEXTURE9      m_pTexture = NULL; /// 텍스쳐 정보
	LPDIRECT3D9             m_pD3D = NULL; /// D3D 디바이스를 생성할 D3D객체변수
	LPDIRECT3DDEVICE9       m_pd3dDevice = NULL; /// 렌더링에 사용될 D3D디바이스
	LPDIRECT3DVERTEXBUFFER9 m_pVB = NULL; /// 정점을 보관할 정점버퍼
	LPD3DXMESH pSphere;

	struct CUSTOMVERTEX
	{
		FLOAT x, y, z; /// 정점의 변환된 좌표(rhw값이 있으면 변환이 완료된 정점이다.
		FLOAT u, v; //텍스처 좌표 
	};


public:

	HRESULT D3DStartup(HWND hWnd);
	void RenderTexture(LPDIRECT3DTEXTURE9 pTex, float x, float y, float z);
	void SetupMatrics();
	void D3DCleanup();
	HRESULT InitVB();
	void SetupLight();
};