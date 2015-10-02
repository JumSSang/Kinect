#include <d3d9.h>
#include <d3dx9.h>
#include <math.h>
#include "openCV.h"




class DirectX :public OpenCV
{
private:
	IDirect3DTexture9 *m_pVideoTexture;
	LPDIRECT3DTEXTURE9      m_pTexture = NULL; /// �ؽ��� ����
	LPDIRECT3D9             m_pD3D = NULL; /// D3D ����̽��� ������ D3D��ü����
	LPDIRECT3DDEVICE9       m_pd3dDevice = NULL; /// �������� ���� D3D����̽�
	LPDIRECT3DVERTEXBUFFER9 m_pVB = NULL; /// ������ ������ ��������
	LPD3DXMESH pSphere;

	struct CUSTOMVERTEX
	{
		FLOAT x, y, z; /// ������ ��ȯ�� ��ǥ(rhw���� ������ ��ȯ�� �Ϸ�� �����̴�.
		FLOAT u, v; //�ؽ�ó ��ǥ 
	};


public:

	HRESULT D3DStartup(HWND hWnd);
	void RenderTexture(LPDIRECT3DTEXTURE9 pTex, float x, float y, float z);
	void SetupMatrics();
	void D3DCleanup();
	HRESULT InitVB();
	void SetupLight();
};