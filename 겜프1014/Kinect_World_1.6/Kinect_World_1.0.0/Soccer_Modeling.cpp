#include "CvKinect.h"


void Kinect::InitMesh(int number)
{

	System_Create(&m_pSystem);
	m_pSystem->init(3, FMOD_INIT_NORMAL, 0);
	m_pSystem->createSound("Data/Soccer/¾û¶×.mp3", FMOD_LOOP_NORMAL, 0, &m_pSound[0]);
	m_pSystem->createSound("Data/Soccer/Å¬¸¯À½3.wav", FMOD_HARDWARE, 0, &m_pSound[2]);
	m_pSystem->createSound("Data/Soccer/Å¸°ÝÀ½1.wav", FMOD_HARDWARE, 0, &m_pSound[1]);
	m_pSystem->createSound("Data/Soccer/Å©¶óÀ× ³Ó-03-¿À ÇÊ½Â ÄÚ¸®¾Æ.mp3", FMOD_HARDWARE, 0, &m_pSound[3]);
	m_football.InitMesh("Data/Soccer/FootBall.x");
	
	fireball.LoadFile(g_pDevice, { 0, 0, 0 }, { 0, 0, 0 }, "Data/Soccer/Fireball.png");
	//m_football.m_pMeshMaterials->Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.3f);
	//m_football.m_pMeshMaterials ->Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.3f);
	////m_football.m_pMeshMaterials ->Specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
	//m_football.m_pMeshMaterials ->Power = 8.0f;
	m_ballCall_img.LoadFile(g_pDevice, { 0, 0, 0 }, { 0, 0, 0 }, "Data/Soccer/ball.png");
	m_main_img.LoadFile(g_pDevice, { 0, 0, 0 }, { 0, 0, 0 }, "Data/Soccer/background.png");
	m_StartButton_img.LoadFile(g_pDevice, { 0, 200, 0 }, { 0, 0, 0 }, "Data/Soccer/start.png");
	m_SettingButton_img.LoadFile(g_pDevice, { 0, 300, 0 }, { 0, 0, 0 }, "Data/Soccer/setting.png");
	m_EndButton_img.LoadFile(g_pDevice, { 0, 400, 0 }, { 0, 0, 0 }, "Data/Soccer/endingbutton.png");
	m_autioButton.LoadFile(g_pDevice, { 1000, 100, 0 }, { 0, 0, 0 }, "Data/Soccer/Audio.png");

	ButtonRECT[0].left = 0;
	ButtonRECT[0].right = 410;
	ButtonRECT[0].top = 0;
	ButtonRECT[0].bottom = 94;


	ButtonRECT[1].left = 410;
	ButtonRECT[1].right = 820;
	ButtonRECT[1].top = 0;
	ButtonRECT[1].bottom = 94;


	m_StartButton_img.rect = ButtonRECT[0];
	m_SettingButton_img.rect = ButtonRECT[0];
	m_EndButton_img.rect = ButtonRECT[0];
	m_autioButton.rect = ButtonRECT[0];
	fireRect[0].left = 0;
	fireRect[0].right = 192;
	fireRect[0].top = 0;
	fireRect[0].bottom = 192;
	int tempnum = 192;

	for (int i = 1; i < 5; i++)
	{
		fireRect[i].left = tempnum;
		tempnum += 192;
		fireRect[i].top = 0;
		fireRect[i].bottom = 192;
		fireRect[i].right = tempnum;
		
	}
	enemy.InitMesh("Data/Soccer/football.x");
	m_goalpost.InitMesh("Data/Soccer/GoalPost2.X");
	m_teaspot.InitMesh("Data/Soccer/teapot.X");
	stadium.InitMesh("Data/Soccer/Stadium.X");
	m_player = new CSkinnedMesh[2];
	m_player[0].LoadMesh(g_pDevice, CUtility::GetTheCurrentDirectory() + "/Data/Soccer/ironman2.X");//¸ðµ¨¸µÀ§Ä¡
	
	D3DXCreateBox(g_pDevice, 1, 1, 0, &m_pBox, NULL);
	D3DXCreateSphere(g_pDevice, 1, 1, 1, &m_pSphere, NULL);
	VertexPNT* v = 0;
	/*m_football.m_pMesh->LockVertexBuffer(0, (void**)&v);

	D3DXComputeBoundingBox(&v[0].pos, m_football.m_pMesh->GetNumVertices(),
		sizeof(VertexPNT), &m_football.mBoundingBox.minPt, &m_football.mBoundingBox.maxPt);

	m_football.m_pMesh->UnlockVertexBuffer();*/

	//float width = m_football.mBoundingBox.maxPt.x - m_football.mBoundingBox.minPt.x;
	//float height = m_football.mBoundingBox.maxPt.y - m_football.mBoundingBox.minPt.y;
	//float depth = m_football.mBoundingBox.maxPt.z - m_football.mBoundingBox.minPt.z;
	
	//m_player2 = new CSkinnedMesh[2];
	//m_player2[0].LoadMesh(g_pDevice,CUtility::GetTheCurrentDirectory() + "/Data/Soccer/ironman.X");//¸ðµ¨¸µÀ§Ä¡
	
	

}

void Kinect::SetWorldMatrix(int entityIndex, const D3DXMATRIX& mat)
{
	m_player[0].SetWorldMatrix(mat);
}

void Kinect::SetKinectMatrix(int entityIndex, const D3DXMATRIX* mat)
{
	CSkinnedMesh &mesh = m_player[0];
	for (int i = 0; i<NUI_SKELETON_POSITION_COUNT; i++)
	{
		mesh.SetKinectMatrix(i, mat[i]);
	}
}

