#include "Graphics.h"
#include "Utility.h"
#include "SkinnedMesh.h"


// 디스플레이 글씨 크기 정의
const int kFontSize=14;

POLAR g_Human = {0.0f, 20.0f, 0.0f, 90.0f, 3.0f, 0.0f};
POLAR g_Camera = {0.0f, 5.0f, -40.0f, 90.0f, 3.0f, 0.0f};

RECT rct;
D3DCOLOR fontColor = D3DCOLOR_XRGB(255,255,255);

D3DXVECTOR3 g_Cross=D3DXVECTOR3(0,0,0);;
float g_Angle=0.0f;

float sstart[20]={0.0f};
float eend[20]={0.0f};
float xx1[20]={0.0f};
float yy1[20]={0.0f};
float zz1[20]={0.0f};

D3DXMATRIXA16 matRotation;
int Index=0;


/// 변수 초기화
CGraphics::CGraphics(void) : m_entity(0), m_displayControls(false),
							m_font(0),g_pd3dDevice(0), g_pD3D(0), g_bFound(false)
{
}

// Destructor - clean up by deleting alloacted memory and releasing Direct3D objects
CGraphics::~CGraphics(void)
{
	if (m_entity)
	{
		//delete m_entity;
		m_entity=0;
	}

	if (m_font)
	{
		m_font->Release();
		m_font=0;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice=0;
	}
	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D=0;
	}
}

/*
	Initialise our graphics by setting up Direct3D
*/
bool CGraphics::Initialise(HWND hWnd)
{
	g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
	if (!g_pD3D)
	{
		CUtility::DebugString("Could not create Direct3D object\n");
		return false;
	}

	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	int vp=0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS presParams;
	memset(&presParams,0,sizeof(presParams));
	presParams.hDeviceWindow=hWnd;
	presParams.Windowed=true;
	presParams.SwapEffect=D3DSWAPEFFECT_DISCARD;
	presParams.BackBufferFormat=D3DFMT_A8R8G8B8;
	presParams.BackBufferCount=1;
	presParams.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE; //D3DPRESENT_INTERVAL_IMMEDIATE : 화면 주사율에 관계없이 MAX 프레임으로 갱신 , D3DPRESENT_INTERVAL_DEFAULT : 화면 주사율과 동일
	presParams.EnableAutoDepthStencil = TRUE;
	presParams.AutoDepthStencilFormat = D3DFMT_D24S8;

	HRESULT hr=g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
									vp, &presParams, &g_pd3dDevice);
	if (CUtility::FailedHr(hr))
	{
		// It may be that the machine cannot support vertex processing in hardware so try software instead
		hr=g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presParams, &g_pd3dDevice);
		if (CUtility::FailedHr(hr))
		{
			CUtility::DebugString("Could not create Direct3D device\n");
			return false;
		}
	}

	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	// Setup basic render state
	g_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,	  FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW ); //D3DCULL_NONE, D3DCULL_CW, D3DCULL_CCW
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x99999999 );
	g_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_CLIPPING, TRUE );

	// Setup states effecting texture rendering:
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	// Lighting	
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	// Create a directional light
	D3DLIGHT9 light;                        
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r  = 1.0f;
	light.Diffuse.g  = 1.0f;
	light.Diffuse.b  = 1.0f;
	light.Diffuse.a  = 1.0f;
	light.Range      = 1000.0f;

	// Direction for our light - it must be normalized - pointing down and along z
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(0.0f,-3.0f,5.0f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	g_pd3dDevice->SetLight( 0, &light );
	g_pd3dDevice->LightEnable( 0, TRUE );
	D3DXVECTOR3 vecDir1;
	vecDir1 = D3DXVECTOR3(-0.0f,3.0f,-5.0f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir1 );
	g_pd3dDevice->SetLight( 1, &light );
	g_pd3dDevice->LightEnable( 1, TRUE );

	// Plus some non directional ambient lighting
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(20,20,20));
	
	// Create a font to display info on the screen
	D3DXCreateFont( g_pd3dDevice, kFontSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font );
	
	SetupCamera();

	return true;
}

/*
	Called as often as possible from main this function calculates the time passed and 
	animates the entity. It also implements the Direct3D render loop.
*/

void CGraphics::SetupCamera()
{
	D3DXVECTOR3 vEyePt(g_Camera.x, g_Camera.y, -10);
	D3DXVECTOR3 vLookatPt(g_Human.x, g_Camera.y, g_Human.z);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	
	D3DXMATRIX matProj;	
	
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 600.0f/1000.0f, 1.0f, 1500.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

void CGraphics::Update()
{		

	D3DXMATRIXA16  mScale;

	// Clear the render target and the zbuffer 
	HRESULT hr=g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	if (CUtility::FailedHr(hr))
		return;
	
	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); 
	// Render the scene
	
	if(SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		InputKey();
		{
			//D3DXMatrixRotationY( &matRotY, g_Human.RotationY*(D3DX_PI / 180.0f)); 
			//D3DXMatrixMultiply( &matWorld, &matRotY, &matWorld);

			if(g_bFound)
				m_entity->MeshRender();//0.016f
		}

		DisplayText();

		g_pd3dDevice->EndScene();
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL ); 
	}
}

/*
	Display data about the model and the controls
*/
void CGraphics::DisplayText() const
{
	static DWORD lastTime=timeGetTime();
	static int numFrames=0;
	static float fps=0;	

	// Calculate frame rate every second
	numFrames++;
	DWORD timePassed=timeGetTime()-lastTime;
	if (timePassed>1000)
	{
		fps=0.001f*timePassed*numFrames;
		numFrames=0;
		lastTime=timeGetTime();
	}
	
	rct.left=kFontSize;
	rct.right=800;
	rct.top=kFontSize;
	rct.bottom=rct.top+kFontSize;

	// Show frame rate
	std::string fpsString="FPS: "+ToString(fps);
	m_font->DrawText(NULL, fpsString.c_str(), -1, &rct, 0, fontColor );
	rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;

	std::string fnameString="캐릭터 파일명: "+m_entity[0].GetFilename();
	m_font->DrawText(NULL,fnameString.c_str(), -1, &rct, 0, fontColor );
	rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;
	std::string vCross="vCross : (" +ToString(g_Cross.x) + ", " + ToString(g_Cross.y) + ", " + ToString(g_Cross.z)+")";
	m_font->DrawText(NULL,vCross.c_str(), -1, &rct, 0, fontColor );
	rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;
	std::string Angle="Angle : " +ToString(g_Angle);
	m_font->DrawText(NULL,Angle.c_str(), -1, &rct, 0, fontColor );
	
	if (m_displayControls)
	{
		rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;
		rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;
		rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;
		std::string SkinnedMesh="SkinnedMesh - X : " +ToString(g_Human.x) + " Y :  "+ ToString(g_Human.y) + " Z : " + ToString(g_Human.z);
		m_font->DrawText(NULL,SkinnedMesh.c_str(), -1, &rct, 0, fontColor );

		for(int index=0; index<20; index++)
		{
			rct.top+=kFontSize;rct.bottom=rct.top+kFontSize;
			std::string Translation1=ToString(index) +"번 시작 : " + ToString(sstart[index]) + "  끝 : " + ToString(eend[index]) + "   X : " +ToString(xx1[index]) + "   Y :  "+ ToString(yy1[index]) + 
				"  Z : " +ToString(zz1[index]);
			m_font->DrawText(NULL,Translation1.c_str(), -1, &rct, 0, fontColor );
		}
	}
}

/*
	This function attempts to load the requested .x file from filename
	If it fails (probably cannot find the file) it returns false
	The startAnimation optionally allows the first animation to play to be set
*/
bool CGraphics::LoadSXFile()
{
	m_entity=new CSkinnedMesh[2];
	m_entity[0].LoadMesh(g_pd3dDevice, CUtility::GetTheCurrentDirectory()+"/data/ironman.x");//모델링위치
	return true;
}

void CGraphics::InputKey()
{
	if(GetKeyState(VK_LEFT) & 0x800)
		g_Human.x+=1.0f;
	if(GetKeyState(VK_RIGHT) & 0x800)
		g_Human.x-=1.0f;
	if(GetKeyState(VK_UP) & 0x800)
		g_Human.y+=1.0f;
	if(GetKeyState(VK_DOWN) & 0x800)
		g_Human.y-=1.0f;
}

void CGraphics::SetWorldMatrix(int entityIndex,const D3DXMATRIX& mat)
{
	m_entity[entityIndex].SetWorldMatrix(mat);
}

void CGraphics::SetKinectMatrix(int entityIndex,const D3DXMATRIX* mat)
{
	CSkinnedMesh &mesh = m_entity[entityIndex];
	for(int i=0;i<NUI_SKELETON_POSITION_COUNT;i++)
	{
		mesh.SetKinectMatrix(i,mat[i]);
	}
}

void CGraphics::BoneTranslation(int index, float x, float y, float z)
{
	char TestBoneName[20][20]={{"Bip01_Pelvis"},{"Bip01_Spine"},{"Bip01_Neck"},{"Bip01 HeadNub"},{"Bip01_L_UpperArm"},{"Bip01_L_Forearm"},{"Bip01_L_Hand"},{"NULL"},
						{"Bip01_R_UpperArm"},{"Bip01_R_Forearm"},{"Bip01_R_Hand"},{"NULL"},{"Bip01_L_Thigh"},{"Bip01_L_Calf"},{"Bip01_L_Foot"},
						{"NULL"},{"Bip01_R_Thigh"},{"Bip01_R_Calf"},{"Bip01_R_Foot"},{"NULL"}};

	m_entity[0].BoneTranslation(TestBoneName[index], -z, y, x);
}

void CGraphics::CharacterMove(float RotateY, float x, float y, float z)
{
	//g_Human.RotationY+=RotateY;
	//g_Human.x-=x;
	//g_Human.y-=y;
	//g_Human.z-=z;
}



void CGraphics::ToggleControlsDisplay()
{
	/*if(m_displayControls)
		m_displayControls=false;
	else
		m_displayControls=true;*/
}