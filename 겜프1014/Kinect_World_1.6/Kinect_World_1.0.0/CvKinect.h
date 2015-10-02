#ifndef KINECT_H
#define KINECT_H

#include "d3d.h"
#include "XFile.h"
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#include <NuiSkeleton.h>
#include <stdlib.h>
#include "Sprite.h"
#include <math.h>
#include <time.h>
#include <math.h>
#include "Step1_SkinnedMesh.h"
#include "XFile.h"
#include <vector>

#include "Utility.h"
#include "inc/fmod.hpp"
#pragma comment (lib, "fmodex_vc.lib")
using namespace std;
using namespace FMOD;
#define BMP_HEIGHTMAP "Data/Soccer/map128.bmp"

class MatStruct
{
public:
	
	D3DXMATRIX m_Trans, m_Rotata, m_Sclae;
	D3DXVECTOR3 position{ 0.0f, 0.0f, 0.0f };
	float xspeed = 0.00f;
	float yspeed = 0.00f;
	float zspeed = 0.00f;
	int gospot=0;
	bool state=false;

};

class Kinect 
{
public:
	System* m_pSystem;
	Sound* m_pSound[7];
	Channel* m_pChannel[3];
	int Score = 0;
	int DefenceScore = 0;
	vector <Sprite_Extend> fire;
	Sprite_Extend fireball;
	Sprite m_main_img;
	Sprite m_StartButton_img;
	Sprite m_SettingButton_img;
	Sprite m_EndButton_img;
	Sprite m_ballCall_img;
	Sprite m_autioButton;
	RECT fireRect[5];
	RECT ButtonRECT[2];
	bool menustate = false;
	LPDIRECT3DVERTEXBUFFER9	tg_pVB = NULL;

	LPDIRECT3DINDEXBUFFER9 tg_pIB = NULL;
	LPDIRECT3DTEXTURE9 t_g_pTexHeight = NULL; // 텍스쳐 높이맵
	LPDIRECT3DTEXTURE9 t_g_pTexDiffuse = NULL; // 텍스쳐 색깔맵

	D3DXMATRIXA16 g_matAni;
	DWORD g_cxHeight = 0;
	DWORD g_czHeight = 0;

	struct TerCUSTOMVERTEX

	{

		D3DXVECTOR3 p;

		D3DXVECTOR3 n;

		D3DXVECTOR3 t;

	};




	struct MYINDEX

	{

		WORD _0, _1, _2;	// 일반적으로 인덱스는 16비트의 크기를 갖는다

	};


	HRESULT TInitVB();
	HRESULT TInitIB();


	////////////////////////////




	D3DXVECTOR3 CameraSpot{ 0.0f, 0.0f, 0.0f };
	vector <MatStruct> footballArr;
	D3DXVECTOR3 IronLook{ 0.0f, 0.0f, 0.0f };
	
	ID3DXMesh*  mSphere;
	Mtrl        mBoxMtrl;
	AABB mBoundingSpear;
	D3DXVECTOR3 Gravity;
	D3DXVECTOR3 Accellaration;
	D3DXVECTOR3 Velocity;
	D3DXVECTOR3 Position;
	D3DXHANDLE   mhMtrl;
	IDirect3DTexture9* mWhiteTex;
	D3DXHANDLE   mhTex;
	int m_mousex=0;
	int m_mousey = 0;
	//typedef struct _D3DVIEWPORT9
	//{
	//	DWORD x; //뷰포트의 좌측 상단 X좌표
	//	DWORD y; //뷰포트의 좌측 상단 Y좌표
	//	DWORD Width; //뷰포트의 가로크기
	//	DWORD Height;//뷰포트의 세로크기
	//	float MinZ; //랜더링 깊이 z최소값
	//	float  MaxZ; //랜더링 깊이 z최대값
	//}D3DVIEWPORT9;




	D3DVIEWPORT9 viewRight;
	D3DVIEWPORT9 viewLeft;
	D3DVIEWPORT9 viewCenter;
	D3DVIEWPORT9 viewMenu;

	int 난이도 = 1;










	class depthspot
	{
	public:
		long m_x;
		long m_y;
		float m_depth;
		float power;
		depthspot(long x, long y, short depth)
		{
			m_x = x;
			m_y = y;
			m_depth = depth;

		};
		~depthspot()
		{




		}
	};
private:


	//다이렉트 관련 변수들
	IDirect3DTexture9 *m_pVideoTexture;
	IDirect3DTexture9 *m_Depth_pVideoTexture;
	LPDIRECT3DTEXTURE9      m_pTexture = NULL; /// 텍스쳐 정보
	LPDIRECT3DVERTEXBUFFER9 m_pVB = NULL; /// 정점을 보관할 정점버퍼
	LPDIRECT3DVERTEXBUFFER9 m_RectVB = NULL; /// 정점을 보관할 정점버퍼

	LPD3DXMESH m_pSphere;
	LPD3DXMESH m_pBox;




	NUI_SKELETON_FRAME		m_skeletonFrame;
	unsigned short depth;
	unsigned short ldepth;
private:
	INuiSensor* m_pSensor = NULL;
	HANDLE m_hVideoStream = NULL;
	HANDLE m_ColorStream = NULL;
	HANDLE m_SkeletonStream = NULL;

	vector <D3DXVECTOR3>ballspot;
	vector <D3DXVECTOR3>ballspeed;

	vector <depthspot> depthstorage;
	vector <int > state;

	RECT Button1[2];
	RECT Button2[2];
	RECT CardRect[2];
	float rotationAngle = 0.00f;
	float randomz;	ID3DXEffect* mFX;

public:
	D3DXVECTOR3 BoundingArea[12];
	
	float m_ballcreate=0.0f;
	static const int CLIENT_WIDTH = 1500;
	static const int CLIENT_HEIGHT = 700;
	static const int g_nWidth = 640;


	static const int g_nHeight = 480;
	XFile m_football;
	XFile m_goalpost;
	XFile m_teaspot;
	XFile m_shadermodel;
	XFile stadium;
	XFile enemy;
	float change_model_time=0;


	CSkinnedMesh* m_player;
	CSkinnedMesh* m_player2;
	float stay_time = 0; //시간 변수
	float m_handx;
	float m_handy;
	float m_testdepth = 0;
	D3DXVECTOR3 m_handRight{ 1000, 1000, 1000 };
	D3DXVECTOR3 m_handLeft{ 1000, 1000, 1000 };
	depthspot m_tempspotdepth{ 0, 0, 0 }; 
	int handobject = 0;
	D3DXMATRIX m_matrix[NUI_SKELETON_POSITION_COUNT];
	///
	float irondmanposx;
	float irondmanposz;
	float ironmanposy;

	D3DXVECTOR3 tempLookat;
	D3DXMATRIXA16 mTranslate, mScale, rox, roy, roz, ballTrans, ballScale, m_GoalpostS, m_GoalPostT, enemyT, enemyS, enemyR;
	D3DXMATRIXA16 BoneScale, BoneTrans;
	D3DXMATRIXA16 playerTrans, playerScale;
	D3DXMATRIXA16 mBoundingBoxAreaTran[13], mBoundingBoxAreaScale[13], mBoundRotate[13];
	D3DXMATRIXA16 mBoundingRightTr, mBoundingRightSc;
	D3DXMATRIXA16 T, S, R;
	
	bool bCaptured = false;
public:

	class spear
	{
	public:
		int left;
		int top;
		int right;
		int bottom;
		float zpoint;
		spear(int l, int t, int r, int b, float z)
		{
			left = l;
			top = t;
			right = r;
			bottom = b;
			zpoint = z;
		}
	};

public:
	Vector4 v;
	Vector4 a;
	Vector4 s;
	Vector4 d;
	
	bool rightcheck;
	bool leftcheck;
	bool gamestart = 1;
	HWND m_hwnd = NULL;
	float m_cameray = 1.0f;
	float m_eyespot = 0.0f;
	float lightdir = 90.0f;
	D3DXVECTOR3 center_head;
	Kinect();
	Kinect(HWND hwnd)
	{
	};

	/*void DrawSkeleton(const NUI_SKELETON_DATA & skel, int windowWidth, int windowHeight);
	void DrawBone(const NUI_SKELETON_DATA & skel, NUI_SKELETON_POSITION_INDEX bone0, NUI_SKELETON_POSITION_INDEX bone1);
	D2D1_POINT_2F           SkeletonToScreen(Vector4 skeletonPoint, int width, int height);*/

	LPD3DXFONT mFont;
	bool KinectInitialize();
	void VideoFrameReady(float time);
	void SetTextureFromBuffer(LPDIRECT3DTEXTURE9 pTex, void * buffer, int x, int y);
	static void onMouse(int event, int x, int y, int, void*);
	void tracking();
	void SoundUpdate(int value);
	void BackSound(int value);
	void SoundRelease();
	//void skeleton();
	void KinectUninitialize();
	void EffectSoundUpdate(int value);
	void DepthSetTextureFromBuffer(LPDIRECT3DTEXTURE9 pTex, void * buffer, int x, int y);
	virtual void DrawLeftScene(float dt) final;
	void DrawRightScene(float dt);
	void DrawUpScene(float dt);
	void DrawMenuSceen(float dt);
	void MenuUpdate(float dt);
	bool Collision(int x,int y, RECT &rs);


public:
	//Direct관련 구현부분
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z; /// 정점의 변환된 좌표(rhw값이 있으면 변환이 완료된 정점이다.
		FLOAT u, v; //텍스처 좌표 
		DWORD color;
	};
	HRESULT D3DStartup(HWND hWnd);
	void RenderTexture(LPDIRECT3DTEXTURE9 pTex, float x, float y, float z);
	void SetupMatrics();
	void D3DCleanup();
	HRESULT InitVB();
	HRESULT InitTexture();
	void SetupLight();
	void InitMesh(int number);
	void DrawMesh();
	void SpriteInit();
	VOID Animate();
	BOOL Collusion(float l, float t, float r, float b, float x, float y);
	bool BoundingCollusion(D3DXVECTOR3 *m1, D3DXVECTOR3 *m2, float m1r, float m2r);
	void GenerateRotateMatrix(NUI_SKELETON_DATA* data, D3DXMATRIX * mat);
	void GenerateMatrixFromVectorY(D3DXMATRIX* out, const D3DXVECTOR3 &toX, const D3DXVECTOR3 &toY);
	void GenerateMatrixFromVectorX(D3DXMATRIX* out, const D3DXVECTOR3 &toX, const D3DXVECTOR3 &toY);
	//벡터 3개로 회전행렬 구하는 함수
	void GenerateMatrixFromVector(D3DXMATRIX* out, const D3DXVECTOR3 &x, const D3DXVECTOR3 &y, const D3DXVECTOR3 &z);
	void GenerateMatrixFromVectorZ(D3DXMATRIX* out, const D3DXVECTOR3 &toY, const D3DXVECTOR3 &toZ);

	void SetWorldMatrix(int entityIndex, const D3DXMATRIX& mat);
	void SetKinectMatrix(int entityIndex, const D3DXMATRIX* mat);
	void BoneTranslation(int index, float x, float y, float z);
	bool AABBvsAABB(AABB&a, AABB&b);
	void ballUpdate();
	void DrawMesh(D3DXMATRIXA16* pMat);
	void GameUpdate(float dt);
	void FootBallAcc(int a);
};



#endif