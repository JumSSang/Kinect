#include "d3d.h"
#include "CvKinect.h"
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)


LPDIRECT3DDEVICE9 g_pDevice;

LPDIRECT3D9       g_pD3D;
HRESULT Kinect::D3DStartup(HWND hWnd)
{
	HRESULT hr;

	/// 디바이스를 생성하기위한 D3D객체 생성
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	/// 디바이스를 생성할 구조체
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;// 뎊스버퍼가 32비트를 지원하지않아 16비트로 변경

	/// 디바이스 생성
	if (FAILED(hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pDevice)))
	{
		MessageBox(NULL, "디바이스 생성 실패.", NULL, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	/// 디바이스 상태정보를 처리할경우 여기에서 한다.
	// 임시 텍스처를 생성한다.,
	hr = g_pDevice->CreateTexture(1024, 1024, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pVideoTexture, NULL);	//D3DPOOL_DEFAULT 일경우 Dynamic 을 안하면 락을 못한다. 주의
	if (FAILED(hr))
	{
		MessageBox(NULL, "텍스처 생성 실패.", NULL, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	D3DXCreateFont(

		g_pDevice,               //그래픽 카드 인터페이스 
		50,                              // 글자 세로 크기
		50,                               // 글자 가로 크기
		1000,                            // 굵기 0~1000사이
		1,                                // 밉맵과 비슷하게 사용되는 것 . 1로 준다
		false,                           // 이텔릭 ( 기울기 ) 
		DEFAULT_CHARSET,       // 일단 이값으로...
		OUT_DEFAULT_PRECIS,   // 정밀도
		DEFAULT_QUALITY,        // 일단 이값으로...
		0,                                // 일단 이값으로...
		"신명조",                      // 사용 글꼴
		&mFont);                    // 인터페이스 포인터 (2차)
	// 스피어를 생성한다.
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
	viewLeft.X = 0; //뷰포트의 좌측x
	viewLeft.Y = 0; //뷰포트의 상단y축
	viewLeft.Width = 700; //가로크기
	viewLeft.Height = 700; //세로길이
	viewLeft.MaxZ = 1.0f; //랜더링의 깊이 최대값
	viewLeft.MinZ = 0.0f; //랜더링 깊이의 최소값
	g_pDevice->SetViewport(&viewLeft);



	viewCenter.X = 600; //뷰포트의 좌측x
	viewCenter.Y = 0; //뷰포트의 상단y축
	viewCenter.Width = 300; //가로크기
	viewCenter.Height = 100; //세로길이
	viewCenter.MaxZ = 1.0f; //랜더링의 깊이 최대값
	viewCenter.MinZ = 0.0f; //랜더링 깊이의 최소값
	g_pDevice->SetViewport(&viewCenter);


	viewRight.X = 800; //뷰포트의 좌측x
	viewRight.Y = 0; //뷰포트의 상단y축
	viewRight.Width = 700; //가로크기
	viewRight.Height = 700; //세로길이
	viewRight.MaxZ = 1.0f; //랜더링의 깊이 최대값
	viewRight.MinZ = 0.0f; //랜더링 깊이의 최소값
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

	t_g_pTexHeight->GetLevelDesc(0, &ddsd);	// 텍스쳐의 정보

	g_cxHeight = ddsd.Width;	// 텍스쳐의 가로크기

	g_czHeight = ddsd.Height;	// 텍스쳐의 세로크기

	/// 정점 버퍼 생성

	if (FAILED(g_pDevice->CreateVertexBuffer(ddsd.Width*ddsd.Height*sizeof(TerCUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX,

		D3DPOOL_DEFAULT, &tg_pVB, NULL)))

	{

		return E_FAIL;

	}




	// 텍스쳐 메모리 락

	t_g_pTexHeight->LockRect(0, &d3drc, NULL, D3DLOCK_READONLY);

	VOID* pVertices;

	// 정점버퍼 락

	if (FAILED(tg_pVB->Lock(0, g_cxHeight*g_czHeight*sizeof(TerCUSTOMVERTEX), (void**)&pVertices, 0)))

		return E_FAIL;
	TerCUSTOMVERTEX v;

	TerCUSTOMVERTEX* pV = (TerCUSTOMVERTEX*)pVertices;
	for (DWORD z = 0; z < g_czHeight; z++)

	{

		for (DWORD x = 0; x < g_cxHeight; x++)

		{

			v.p.x = (float)x - g_cxHeight / 2.0f;	// 정점의 x좌표 (메시를 원점에 정렬)

			v.p.z = -((float)z - g_czHeight / 2.0f);	// 정점의 z좌표 (메시를 원점에 정렬), z축이 모니터 안쪽이므로 -를 곱한다.

			v.p.y = ((float)(*((LPDWORD)d3drc.pBits + x + z*

				(d3drc.Pitch / 4)) & 0x000000ff)) / 10.0f;

			// DWORD이므로 pitch/4




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
VOID Kinect::SetupMatrics() //카메라 설치
{


	//월드행렬
	D3DXMATRIXA16 matWorld;
	//UINT iTime = timeGetTime() % 1000; 
	//FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	//D3DXMatrixRotationY(&matWorld, fAngle); // Y축을 회전축으로 회전행렬을 생성한다.
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld); //생성한 회전 행렬을 월드 행렬로 디바이스에 설정한다.

	//뷰 행렬을 정의하기 위해서는 3가지의 값이 필요함
	D3DXVECTOR3 vEyePt(CameraSpot.x, CameraSpot.y, CameraSpot.z);  //눈의 위치
	D3DXVECTOR3 vLookatPt(IronLook.x, IronLook.y, IronLook.z); //눈이 바라보는 위치
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f); //천정방향을 나타내는 상방벡터(0,1,0)

	//D3DXVECTOR3 vEyePt(0.0f, 0, 0);  //눈의 위치
	//D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 1.0f); //눈이 바라보는 위치
	//D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f); //천정방향을 나타내는 상방벡터(0,1,0)

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec); //1,2,3의 값으로 뷰 행렬 생성
	g_pDevice->SetTransform(D3DTS_VIEW, &matView); //생성한 뷰 행렬을 디바이스에 설정


	//프로제션 행렬을 정의하기 위해서는 시야각(FOV=Field of View)과 종횡비 (aspect ratio),클리핑 평면의 값이 필요하다.
	D3DXMATRIXA16 matProj;
	//D3DXMatrixOrthoOffCenterLH(&matProj, 0, (float)CLIENT_WIDTH, (float)CLIENT_HEIGHT, 0, -100.f, 100.f);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(43), (float)CLIENT_WIDTH / CLIENT_HEIGHT, 0.01f, 100.f); //시야각
	g_pDevice->SetTransform(D3DTS_PROJECTION, &matProj); //생성한 프로젝션 행렬을 디바이스에 설정

}
void Kinect::SetupLight()
{
	//재질(material 설정
	//재질은 디바이스에 단 하나만 설정될 수 있다.
	D3DMATERIAL9 mtrl;
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.2f;
	g_pDevice->SetMaterial(&mtrl);

	D3DXVECTOR3 verDir; //방향성 광원 (directional light)이 향할 빛의 방향
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;

//	D3DLIGHT_SPOT
	light.Diffuse.r = 2.0f;
	light.Diffuse.g = 2.0f;
	light.Diffuse.b = 2.0f;
	verDir = D3DXVECTOR3(350.0f, //광원의 방향
		1.0f,
		sinf(lightdir));
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &verDir);
	//광원의 방향을 단위 벡터로 만든다.
	light.Range = 1000.0f; //광원이 다다를수 있는 최대거리
	g_pDevice->SetLight(0, &light); //디바이스에 0번 광원 설치
	g_pDevice->LightEnable(0, TRUE);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);//광원 설정을 켠다.
	g_pDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);//환경 광원 (ambient light)의 값 설정
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




	// 색깔 맵

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
	/// 삼각형을 렌더링하기위해 세개의 정점을 선언
	CUSTOMVERTEX vertices[] =
	{
		{ -6.4f, -4.8f, 0.0f, 0, v },
		{ -6.4f, 4.8f, 0.0f, 0, 0 },
		{ 6.4f, -4.8f, 0.0f, u, v }, // x, y, z, color
		{ 6.4f, 4.8f, 0.0f, u, 0 },

	};
	/// 정점버퍼 생성
	/// 3개의 사용자정점을 보관할 메모리를 할당한다.
	/// FVF를 지정하여 보관할 데이터의 형식을 지정한다.
	if (FAILED(g_pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &m_pVB, NULL)))
	{
		return E_FAIL;
	}



	/// 정점버퍼를 값으로 채운다. 
	/// 정점버퍼의 Lock()함수를 호출하여 포인터를 얻어온다.
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
