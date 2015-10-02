#include "d3d.h"
#include "CvKinect.h"
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)


LPDIRECT3DDEVICE9 g_pDevice;

LPDIRECT3D9       g_pD3D;
HRESULT Kinect::D3DStartup(HWND hWnd)
{
	HRESULT hr;

	/// ����̽��� �����ϱ����� D3D��ü ����
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	/// ����̽��� ������ ����ü
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;// �X�����۰� 32��Ʈ�� ���������ʾ� 16��Ʈ�� ����

	/// ����̽� ����
	if (FAILED(hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pDevice)))
	{
		MessageBox(NULL, "����̽� ���� ����.", NULL, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	/// ����̽� ���������� ó���Ұ�� ���⿡�� �Ѵ�.
	// �ӽ� �ؽ�ó�� �����Ѵ�.,
	hr = g_pDevice->CreateTexture(1024, 1024, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pVideoTexture, NULL);	//D3DPOOL_DEFAULT �ϰ�� Dynamic �� ���ϸ� ���� ���Ѵ�. ����
	if (FAILED(hr))
	{
		MessageBox(NULL, "�ؽ�ó ���� ����.", NULL, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	D3DXCreateFont(

		g_pDevice,               //�׷��� ī�� �������̽� 
		50,                              // ���� ���� ũ��
		50,                               // ���� ���� ũ��
		1000,                            // ���� 0~1000����
		1,                                // �Ӹʰ� ����ϰ� ���Ǵ� �� . 1�� �ش�
		false,                           // ���ڸ� ( ���� ) 
		DEFAULT_CHARSET,       // �ϴ� �̰�����...
		OUT_DEFAULT_PRECIS,   // ���е�
		DEFAULT_QUALITY,        // �ϴ� �̰�����...
		0,                                // �ϴ� �̰�����...
		"�Ÿ���",                      // ��� �۲�
		&mFont);                    // �������̽� ������ (2��)
	// ���Ǿ �����Ѵ�.
	//	D3DXCreateSphere(g_pDevice, 10, 10, 50, &pSphere, NULL);
	
	SetupMatrics();

	D3DXVECTOR3 temp[13] =
	{
		D3DXVECTOR3(-1.0f, -0.6f, 5),

		D3DXVECTOR3(0.1f, -0.6f, 5),

		D3DXVECTOR3(1.2f, -0.6f, 5),

		D3DXVECTOR3(2.3f, -0.6f, 5),

		D3DXVECTOR3(3.3f, -0.6f, 5),

		D3DXVECTOR3(4.3f, -0.6f, 5),

		D3DXVECTOR3(-1.0f, 0.0f, 5),

		D3DXVECTOR3(0.1f, 0.0f, 5),

		D3DXVECTOR3(1.2f, 0.0f, 5),

		D3DXVECTOR3(2.3f, 0.0f, 5),

		D3DXVECTOR3(3.3f, 0.0f, 5),

		D3DXVECTOR3(4.3f, 0.0f, 5),

	};
	for (int i = 0; i < 13; i++)
	{
		BoundingArea[i] = temp[i];
	}
	viewLeft.X = 0; //����Ʈ�� ����x
	viewLeft.Y = 0; //����Ʈ�� ���y��
	viewLeft.Width = 700; //����ũ��
	viewLeft.Height = 700; //���α���
	viewLeft.MaxZ = 1.0f; //�������� ���� �ִ밪
	viewLeft.MinZ = 0.0f; //������ ������ �ּҰ�
	g_pDevice->SetViewport(&viewLeft);



	viewCenter.X = 600; //����Ʈ�� ����x
	viewCenter.Y = 0; //����Ʈ�� ���y��
	viewCenter.Width = 300; //����ũ��
	viewCenter.Height = 100; //���α���
	viewCenter.MaxZ = 1.0f; //�������� ���� �ִ밪
	viewCenter.MinZ = 0.0f; //������ ������ �ּҰ�
	g_pDevice->SetViewport(&viewCenter);


	viewRight.X = 800; //����Ʈ�� ����x
	viewRight.Y = 0; //����Ʈ�� ���y��
	viewRight.Width = 700; //����ũ��
	viewRight.Height = 700; //���α���
	viewRight.MaxZ = 1.0f; //�������� ���� �ִ밪
	viewRight.MinZ = 0.0f; //������ ������ �ּҰ�
	g_pDevice->SetViewport(&viewRight);
	
	viewMenu.X = 0;
	viewMenu.Y = 0;
	viewMenu.Width = 1500;
	viewMenu.Height = 700;
	viewMenu.MaxZ = 1.0f;
	viewMenu.MinZ = 0.0f;
	g_pDevice->SetViewport(&viewMenu);



	InitTexture();
	TInitVB();
	TInitIB();
	InitMesh(1);


	return S_OK;
}


HRESULT Kinect::TInitIB()
{

	if (FAILED(g_pDevice->CreateIndexBuffer((g_cxHeight - 1)*(g_czHeight - 1) * 2 * sizeof(MYINDEX),

		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &tg_pIB, NULL)))

	{

		return E_FAIL;

	}




	MYINDEX i;

	MYINDEX* pI;

	if (FAILED(tg_pIB->Lock(0, (g_cxHeight - 1)*(g_czHeight - 1) * 2 * sizeof(MYINDEX), (void**)&pI, 0)))

		return E_FAIL;




	for (DWORD z = 0; z < g_czHeight - 1; z++)

	{

		for (DWORD x = 0; x < g_cxHeight - 1; x++)

		{
			i._0 = (z*g_cxHeight + x);

			i._1 = (z*g_cxHeight + x + 1);

			i._2 = ((z + 1)*g_cxHeight + x);

			*pI++ = i;

			i._0 = ((z + 1)*g_cxHeight + x);

			i._1 = (z*g_cxHeight + x + 1);

			i._2 = ((z + 1)*g_cxHeight + x + 1);

			*pI++ = i;

		}

	}

	tg_pIB->Unlock();




	return S_OK;

}

void Kinect::DrawMesh(D3DXMATRIXA16* pMat)
{
	g_pDevice->SetTransform(D3DTS_WORLD, pMat);

	g_pDevice->SetStreamSource(0, tg_pVB, 0, sizeof(CUSTOMVERTEX));

	g_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	g_pDevice->SetIndices(tg_pIB);

	g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, g_cxHeight*g_czHeight, 0, (g_cxHeight - 1)*(g_czHeight - 1) * 2);
}


HRESULT Kinect::TInitVB()
{
	D3DSURFACE_DESC ddsd;

	D3DLOCKED_RECT d3drc;

	t_g_pTexHeight->GetLevelDesc(0, &ddsd);	// �ؽ����� ����

	g_cxHeight = ddsd.Width;	// �ؽ����� ����ũ��

	g_czHeight = ddsd.Height;	// �ؽ����� ����ũ��

	/// ���� ���� ����

	if (FAILED(g_pDevice->CreateVertexBuffer(ddsd.Width*ddsd.Height*sizeof(TerCUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX,

		D3DPOOL_DEFAULT, &tg_pVB, NULL)))

	{

		return E_FAIL;

	}




	// �ؽ��� �޸� ��

	t_g_pTexHeight->LockRect(0, &d3drc, NULL, D3DLOCK_READONLY);

	VOID* pVertices;

	// �������� ��

	if (FAILED(tg_pVB->Lock(0, g_cxHeight*g_czHeight*sizeof(TerCUSTOMVERTEX), (void**)&pVertices, 0)))

		return E_FAIL;
	TerCUSTOMVERTEX v;

	TerCUSTOMVERTEX* pV = (TerCUSTOMVERTEX*)pVertices;
	for (DWORD z = 0; z < g_czHeight; z++)

	{

		for (DWORD x = 0; x < g_cxHeight; x++)

		{

			v.p.x = (float)x - g_cxHeight / 2.0f;	// ������ x��ǥ (�޽ø� ������ ����)

			v.p.z = -((float)z - g_czHeight / 2.0f);	// ������ z��ǥ (�޽ø� ������ ����), z���� ����� �����̹Ƿ� -�� ���Ѵ�.

			v.p.y = ((float)(*((LPDWORD)d3drc.pBits + x + z*

				(d3drc.Pitch / 4)) & 0x000000ff)) / 10.0f;

			// DWORD�̹Ƿ� pitch/4




			v.n.x = v.p.x;

			v.n.y = v.p.y;

			v.n.z = v.p.z;

			D3DXVec3Normalize(&v.n, &v.n);

			v.t.x = (float)x / (g_cxHeight - 1);

			v.t.y = (float)z / (g_czHeight - 1);

			*pV++ = v;

			//g_pLog->Log("[%f, %f, %f]", v.x, v.y, v.z);

		}

	}



	tg_pVB->Unlock();

	t_g_pTexHeight->UnlockRect(0);




	return S_OK;

}
VOID Kinect::SetupMatrics() //ī�޶� ��ġ
{


	//�������
	D3DXMATRIXA16 matWorld;
	//UINT iTime = timeGetTime() % 1000; 
	//FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	//D3DXMatrixRotationY(&matWorld, fAngle); // Y���� ȸ�������� ȸ������� �����Ѵ�.
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld); //������ ȸ�� ����� ���� ��ķ� ����̽��� �����Ѵ�.

	//�� ����� �����ϱ� ���ؼ��� 3������ ���� �ʿ���
	D3DXVECTOR3 vEyePt(CameraSpot.x, CameraSpot.y, CameraSpot.z);  //���� ��ġ
	D3DXVECTOR3 vLookatPt(IronLook.x, IronLook.y, IronLook.z); //���� �ٶ󺸴� ��ġ
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f); //õ�������� ��Ÿ���� ��溤��(0,1,0)

	//D3DXVECTOR3 vEyePt(0.0f, 0, 0);  //���� ��ġ
	//D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 1.0f); //���� �ٶ󺸴� ��ġ
	//D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f); //õ�������� ��Ÿ���� ��溤��(0,1,0)

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec); //1,2,3�� ������ �� ��� ����
	g_pDevice->SetTransform(D3DTS_VIEW, &matView); //������ �� ����� ����̽��� ����


	//�������� ����� �����ϱ� ���ؼ��� �þ߰�(FOV=Field of View)�� ��Ⱦ�� (aspect ratio),Ŭ���� ����� ���� �ʿ��ϴ�.
	D3DXMATRIXA16 matProj;
	//D3DXMatrixOrthoOffCenterLH(&matProj, 0, (float)CLIENT_WIDTH, (float)CLIENT_HEIGHT, 0, -100.f, 100.f);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(43), (float)CLIENT_WIDTH / CLIENT_HEIGHT, 0.01f, 100.f); //�þ߰�
	g_pDevice->SetTransform(D3DTS_PROJECTION, &matProj); //������ �������� ����� ����̽��� ����

}
void Kinect::SetupLight()
{
	//����(material ����
	//������ ����̽��� �� �ϳ��� ������ �� �ִ�.
	D3DMATERIAL9 mtrl;
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.2f;
	g_pDevice->SetMaterial(&mtrl);

	D3DXVECTOR3 verDir; //���⼺ ���� (directional light)�� ���� ���� ����
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;

//	D3DLIGHT_SPOT
	light.Diffuse.r = 2.0f;
	light.Diffuse.g = 2.0f;
	light.Diffuse.b = 2.0f;
	verDir = D3DXVECTOR3(350.0f, //������ ����
		1.0f,
		sinf(lightdir));
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &verDir);
	//������ ������ ���� ���ͷ� �����.
	light.Range = 1000.0f; //������ �ٴٸ��� �ִ� �ִ�Ÿ�
	g_pDevice->SetLight(0, &light); //����̽��� 0�� ���� ��ġ
	g_pDevice->LightEnable(0, TRUE);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);//���� ������ �Ҵ�.
	g_pDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);//ȯ�� ���� (ambient light)�� �� ����
	//	g_pDevice->SetLight(0, &light);
	//g_pDevice->LightEnable(0, TRUE);


}

void Kinect::D3DCleanup()
{
	if (m_pVideoTexture != NULL)
		m_pVideoTexture->Release();

	if (m_pVB != NULL)
		m_pVB->Release();

	if (g_pDevice != NULL)
		g_pDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

HRESULT Kinect::InitTexture()
{
	if (FAILED(D3DXCreateTextureFromFileEx(g_pDevice, BMP_HEIGHTMAP, D3DX_DEFAULT,

		D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,

		D3DX_DEFAULT, 0, NULL, NULL, &t_g_pTexHeight)))

		return E_FAIL;




	// ���� ��

	if (FAILED(D3DXCreateTextureFromFile(g_pDevice, "tile2.tga", &t_g_pTexDiffuse)))

		return E_FAIL;
	return S_OK;
}
HRESULT Kinect::InitVB()
{
	//InitMesh();

	float u, v;
	u = 640.0f / 1024.0f;
	v = 480.0f / 1024.0f;
	/// �ﰢ���� �������ϱ����� ������ ������ ����
	CUSTOMVERTEX vertices[] =
	{
		{ -6.4f, -4.8f, 0.0f, 0, v },
		{ -6.4f, 4.8f, 0.0f, 0, 0 },
		{ 6.4f, -4.8f, 0.0f, u, v }, // x, y, z, color
		{ 6.4f, 4.8f, 0.0f, u, 0 },

	};
	/// �������� ����
	/// 3���� ����������� ������ �޸𸮸� �Ҵ��Ѵ�.
	/// FVF�� �����Ͽ� ������ �������� ������ �����Ѵ�.
	if (FAILED(g_pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &m_pVB, NULL)))
	{
		return E_FAIL;
	}



	/// �������۸� ������ ä���. 
	/// ���������� Lock()�Լ��� ȣ���Ͽ� �����͸� ���´�.
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, vertices, sizeof(vertices));
	m_pVB->Unlock();

	CUSTOMVERTEX Groundvertices[] =
	{
		{ 150.0f, 50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
	};
	if (FAILED(g_pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &m_RectVB, NULL)))
	{
		return E_FAIL;
	}
	VOID* psVertices;
	if (FAILED(m_RectVB->Lock(0, sizeof(Groundvertices), (void**)&psVertices, 0)))
		return E_FAIL;
	memcpy(psVertices, Groundvertices, sizeof(Groundvertices));
	m_RectVB->Unlock();

	m_ballcreate = 0.0f;



	return S_OK;
}
