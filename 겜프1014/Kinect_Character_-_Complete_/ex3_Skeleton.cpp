#include <Windows.h>
#include <NuiApi.h>
#include "ImageRenderer.h"
#include "Graphics.h"
#include "stdafx.h"
#include <stdio.h>

template <typename T> void SkeletonLoop(int index,D3DXMATRIX matrix,T lambda);
inline void GenerateMatrixFromVector(D3DXMATRIX* out ,const D3DXVECTOR3 &x,const D3DXVECTOR3 &y,const D3DXVECTOR3 &z);
inline void GenerateMatrixFromVectorY(D3DXMATRIX* out ,const D3DXVECTOR3 &toX,const D3DXVECTOR3 &toY);
inline void GenerateMatrixFromVectorX(D3DXMATRIX* out ,const D3DXVECTOR3 &toX,const D3DXVECTOR3 &toY);
inline void GenerateMatrixFromVectorZ(D3DXMATRIX* out ,const D3DXVECTOR3 &toY,const D3DXVECTOR3 &toZ);
inline void SetParentMatrix(D3DXMATRIX* pChild,D3DXMATRIX* pFrom);

ID2D1Factory*           m_pD2DFactory;
CGraphics *gGraphics=0;
HWND g_hWnd = NULL;
HWND g_hChild1 = NULL;
HWND g_hChild2 = NULL;
HWND g_hChild3 = NULL;

const int g_nWidth	= 1200;
const int g_nHeight = 1000;

INuiSensor *	g_pSensor = NULL;
ImageRenderer*  m_pDrawColor;
HANDLE			m_pColorStreamHandle;
HANDLE			m_hNextColorFrameEvent;



bool	FirstState = true;
bool	g_bFound = false;
POINT	g_ColorPoints[NUI_SKELETON_POSITION_COUNT] = {0, };

HDC		g_SkeletonDC = NULL;
HPEN	g_hPen[NUI_SKELETON_COUNT] = {NULL, };

D3DXMATRIX g_matrix[NUI_SKELETON_POSITION_COUNT];

const COLORREF g_JointColorTable[NUI_SKELETON_POSITION_COUNT] = 
{
	RGB(169, 176, 155),	RGB(169, 176, 155),
	RGB(168, 230,  29),	RGB(200,   0,   0),
	RGB( 79,  84,  33),	RGB( 83,  33,  42),
	RGB(255, 126,   0),	RGB(215,  86,   0),
	RGB( 33,  79,  84),	RGB( 33,  33,  84),
	RGB( 77, 109, 243),	RGB( 37,  69, 243),
	RGB( 77, 109, 243),	RGB( 69,  33,  84),
	RGB(229, 170, 122),	RGB(255, 126,   0),
	RGB(181, 165, 213),	RGB( 71, 222,  76),
	RGB(245, 228, 156),	RGB( 77, 109, 243)
};

struct PositionTranslation
{
	D3DXVECTOR3 vOrigPos;
	D3DXVECTOR3 vMovePos;
	D3DXVECTOR3 vChangeValue;
};

struct PositionTranslation TransBone[20]={{D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0)}, };

bool Initialize(HWND hWnd)
{
	HRESULT hr;

	RECT Rc={0,0,600,400};
    // Init Direct2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

    // Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
    // We'll use this to draw the data we receive from the Kinect to the screen
    m_pDrawColor = new ImageRenderer();
    m_pDrawColor->Initialize(hWnd, m_pD2DFactory, 640, 480, 640 * sizeof(long));


	hr = NuiCreateSensorByIndex(0, &g_pSensor);
	if(FAILED(hr))
	{
		MessageBox(NULL, "NuiCreateSensorByIndex - Failed.", NULL, NULL);
		return false;
	}

	hr = g_pSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if(FAILED(hr))
	{
		MessageBox(NULL, "NuiInitialize - Failed.", NULL, NULL);
		return false;
	}

	if(HasSkeletalEngine(g_pSensor))
	{
		hr = g_pSensor->NuiSkeletonTrackingEnable(NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE);
		if(FAILED(hr))
		{
			MessageBox(NULL, "NuiImageStreamOpen - Failed.", NULL, NULL);
			return false;
		}
	}

	m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    g_pSensor->NuiImageStreamOpen(
        NUI_IMAGE_TYPE_COLOR,
        NUI_IMAGE_RESOLUTION_640x480,
        0,
        2,
        m_hNextColorFrameEvent,
        &m_pColorStreamHandle);

	g_hPen[0] = CreatePen(PS_SOLID, g_nWidth/240, RGB(255,0,0));
	g_hPen[1] = CreatePen(PS_SOLID, g_nWidth/240, RGB(0,255,0));
	g_hPen[2] = CreatePen(PS_SOLID, g_nWidth/240, RGB(0,255,255));
	g_hPen[3] = CreatePen(PS_SOLID, g_nWidth/240, RGB(255,255,0));
	g_hPen[4] = CreatePen(PS_SOLID, g_nWidth/240, RGB(255,0,255));
	g_hPen[5] = CreatePen(PS_SOLID, g_nWidth/240, RGB(0,0,255));

	HDC hDC = GetDC(g_hChild2);
	HBITMAP hBmp = CreateCompatibleBitmap(hDC, g_nWidth/2, 560);
	g_SkeletonDC = CreateCompatibleDC(hDC);
	SelectObject(g_SkeletonDC, hBmp);
	DeleteObject(hBmp);
	ReleaseDC(g_hChild2, hDC);
	
	return true;
}

void DrawSegment(POINT pt1, POINT pt2)
{
	MoveToEx(g_SkeletonDC, pt1.x, pt1.y, NULL);
	LineTo(g_SkeletonDC, pt2.x, pt2.y);
}

void DrawVector(POINT pt,float x,float y,float z)
{
	MoveToEx(g_SkeletonDC, pt.x, pt.y, NULL);
	LineTo(g_SkeletonDC, pt.x + (int)(x * 30), pt.y - (int)(y * 30));
}

void DrawSkeleton()
{	
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_SPINE]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SPINE], g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_HEAD]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_WRIST_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_WRIST_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_HAND_LEFT]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_WRIST_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_WRIST_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_HAND_RIGHT]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_HIP_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_KNEE_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_KNEE_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_FOOT_LEFT]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_HIP_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_KNEE_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_KNEE_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_FOOT_RIGHT]);

	D3DXMATRIX mIdentity;
	D3DXMatrixIdentity(&mIdentity);

	SetTextColor(g_SkeletonDC,0xffffff);
	SetBkMode(g_SkeletonDC,TRANSPARENT);
	
	for(int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
	{
		POINT pt = g_ColorPoints[j];		
		D3DXVECTOR3 v = TransBone[j].vOrigPos;

		TCHAR temp[1024];
		//sprintf_s(temp,_T("%.02f, %.02f, %.02f"),v.x,v.y,v.z);
		//TextOut(g_SkeletonDC, pt.x, pt.y, temp, strlen(temp));
					
		
		D3DXMATRIX &mat = g_matrix[j];
		Ellipse(g_SkeletonDC, pt.x-5, pt.y-5, pt.x+5, pt.y+5);
		
		HPEN hPen, OldPen;
		hPen = CreatePen(PS_SOLID, 2, RGB(255,0,0));
		OldPen = (HPEN)SelectObject(g_SkeletonDC, hPen);
		//DrawVector(pt,mat._11,mat._12,mat._13);
		SelectObject(g_SkeletonDC, OldPen);
		DeleteObject(hPen);
		
		hPen = CreatePen(PS_SOLID, 2, RGB(0,255,0));
		OldPen = (HPEN)SelectObject(g_SkeletonDC, hPen);
		//DrawVector(pt,mat._21,mat._22,mat._23);
		SelectObject(g_SkeletonDC, OldPen);
		DeleteObject(hPen);
		
		hPen = CreatePen(PS_SOLID, 2, RGB(0,0,255));
		OldPen = (HPEN)SelectObject(g_SkeletonDC, hPen);
		//DrawVector(pt,mat._31,mat._32,mat._33);
		SelectObject(g_SkeletonDC, OldPen);
		DeleteObject(hPen);
	}
}

void Draw()
{
	HDC hDC = GetDC(g_hChild2);
	BitBlt(hDC, 0, 0, g_nWidth/2, 560, g_SkeletonDC, 0, 0, SRCCOPY);
	ReleaseDC(g_hChild2, hDC);
}

void ProcessColor()
{
    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the color frame
    hr = g_pSensor->NuiImageStreamGetNextFrame(m_pColorStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return;
    }

    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we've received valid data
    if (LockedRect.Pitch != 0)
    {
        // Draw the data with Direct2D
        m_pDrawColor->Draw(static_cast<BYTE *>(LockedRect.pBits), LockedRect.size);
    }

    // We're done with the texture so unlock it
    pTexture->UnlockRect(0);

    // Release the frame
    g_pSensor->NuiImageStreamReleaseFrame(m_pColorStreamHandle, &imageFrame);
}

void GenerateRotateMatrix(NUI_SKELETON_DATA* data,D3DXMATRIX * mat)
{
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_SPINE]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SPINE], g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_HEAD]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_WRIST_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_WRIST_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_HAND_LEFT]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_SHOULDER_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ELBOW_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_WRIST_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_WRIST_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_HAND_RIGHT]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_HIP_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_KNEE_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_KNEE_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_LEFT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_LEFT], g_ColorPoints[NUI_SKELETON_POSITION_FOOT_LEFT]);

	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_CENTER], g_ColorPoints[NUI_SKELETON_POSITION_HIP_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_HIP_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_KNEE_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_KNEE_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_RIGHT]);
	DrawSegment(g_ColorPoints[NUI_SKELETON_POSITION_ANKLE_RIGHT], g_ColorPoints[NUI_SKELETON_POSITION_FOOT_RIGHT]);


	D3DXVECTOR3 vHipCenter = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER];
	D3DXVECTOR3 vSpine = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_SPINE];
	D3DXVECTOR3 vShoulderCenter = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];
	D3DXVECTOR3 vHead = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
	D3DXVECTOR3 vShoulderLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
	D3DXVECTOR3 vElbowLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
	D3DXVECTOR3 vWristLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT];
	D3DXVECTOR3 vHandLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];

	D3DXVECTOR3 vShoulderRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
	D3DXVECTOR3 vElbowRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
	D3DXVECTOR3 vWristRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT];
	D3DXVECTOR3 vHandRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
	
	D3DXVECTOR3 vHipLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_HIP_LEFT];
	D3DXVECTOR3 vKneeLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_KNEE_LEFT];
	D3DXVECTOR3 vAnkleLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_ANKLE_LEFT];
	D3DXVECTOR3 vFootLeft = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT];

	D3DXVECTOR3 vHipRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_HIP_RIGHT];
	D3DXVECTOR3 vKneeRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_KNEE_RIGHT];
	D3DXVECTOR3 vAnkleRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_ANKLE_RIGHT];
	D3DXVECTOR3 vFootRight = (D3DXVECTOR3&)data->SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT];
	
	for(int i=0;i<NUI_SKELETON_POSITION_COUNT;i++)
	{
		D3DXMatrixIdentity(&mat[i]);
	}
	
	static const D3DXVECTOR3 axisZ(0,0,1);

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HIP_CENTER],
		vHipRight-vHipLeft,
		vHipCenter-(vHipLeft + vHipRight) * 0.5f); //힙회전

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_SPINE],
		(vShoulderRight+vHipRight)*0.5f - vSpine,
		vShoulderCenter - vSpine); //상체회전

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_SHOULDER_CENTER],
		vShoulderRight-vShoulderLeft, 
		vShoulderCenter-vSpine); //숄더 센터부분에서 회전
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HEAD],
		vShoulderRight-vShoulderLeft,
		vHead-vShoulderCenter); //머리회전
	
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_SHOULDER_RIGHT],
		vElbowRight-vShoulderRight,
		vShoulderCenter-vSpine); //오른쪽어깨 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_ELBOW_RIGHT],
		vWristRight-vElbowRight,
		vShoulderCenter-vSpine); //팔꿈치 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_WRIST_RIGHT],
		vHandRight-vWristRight,
		vShoulderCenter-vSpine); //손목회전
	mat[NUI_SKELETON_POSITION_HAND_RIGHT] = mat[NUI_SKELETON_POSITION_WRIST_RIGHT];// NUI_SKELETON_POSITION_HAND_RIGHT - 손목과 동일한 각도로 밖에 계산이 불가능. 손회전
	
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HIP_RIGHT],
		vHipRight-vHipCenter,
		vHipRight-vKneeRight); //오른쪽 엉덩이 회전
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_KNEE_RIGHT],
		vHipRight-vHipCenter,
		vKneeRight-vAnkleRight); //무릎회전
	GenerateMatrixFromVectorZ(&mat[NUI_SKELETON_POSITION_ANKLE_RIGHT],
		vKneeRight-vAnkleRight,
		vAnkleRight-vFootRight);// 발목회전
	mat[NUI_SKELETON_POSITION_FOOT_RIGHT] = mat[NUI_SKELETON_POSITION_ANKLE_RIGHT]; //오른쪽 발회전
	
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_SHOULDER_LEFT],
		vShoulderLeft-vElbowLeft,
		vShoulderCenter-vSpine); //왼쪽어깨 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_ELBOW_LEFT],
		vElbowLeft-vWristLeft,
		vShoulderCenter-vSpine); //왼쪽팔꿈치 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_WRIST_LEFT],
		vWristLeft-vHandLeft,
		vShoulderCenter-vSpine); //왼쪽 손목회전
	mat[NUI_SKELETON_POSITION_HAND_LEFT] = mat[NUI_SKELETON_POSITION_WRIST_LEFT]; //왼손회전
	
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HIP_LEFT],
		vHipCenter-vHipLeft,
		vHipLeft-vKneeLeft); //왼쪽 엉덩이 회전
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_KNEE_LEFT],
		vHipCenter-vHipLeft,
		vKneeLeft-vAnkleLeft); //왼쪽 무릎회전
	GenerateMatrixFromVectorZ(&mat[NUI_SKELETON_POSITION_ANKLE_LEFT],
		vKneeLeft-vAnkleLeft,
		vAnkleLeft-vFootLeft); //왼쪽 발목 회전
	mat[NUI_SKELETON_POSITION_FOOT_LEFT] = mat[NUI_SKELETON_POSITION_ANKLE_LEFT]; //왼쪽 발회전
	

	//GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_ELBOW_RIGHT]		,vShoulderRight		,vElbowRight		,axisZ);
	//GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_WRIST_RIGHT]		,vElbowRight		,vWristRight		,axisZ);
	//GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_HAND_RIGHT]		,vWristRight		,vHandRight			,axisZ);
}

void SkeletonFrameReady()
{
	NUI_SKELETON_FRAME SkeletonFrame;

	HRESULT hr = g_pSensor->NuiSkeletonGetNextFrame(0, &SkeletonFrame);

	if(SUCCEEDED(hr))
	{
		NUI_TRANSFORM_SMOOTH_PARAMETERS	SmoothParam;
		
		SmoothParam.fCorrection			= 0.3f;
		SmoothParam.fJitterRadius		= 1.0f;
		SmoothParam.fMaxDeviationRadius	= 0.5f;
		SmoothParam.fPrediction			= 0.4f;
		SmoothParam.fSmoothing			= 0.7f;
		
		hr = g_pSensor->NuiTransformSmooth(&SkeletonFrame, &SmoothParam);
		if(SUCCEEDED(hr))
		{
			for(int i = 0; i < NUI_SKELETON_COUNT; i++)
			{
				if(SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
				{
					g_bFound = true;
					gGraphics->g_bFound = true;
					break;
				}
				else
				{
					g_bFound = false;
					gGraphics->g_bFound = false;
				}
			}

			PatBlt(g_SkeletonDC, 0, 0, g_nWidth/2, 560, BLACKNESS);

			if(!g_bFound)
				return;
			

			for(int i = 0; i < NUI_SKELETON_COUNT; i++)
			{
				NUI_SKELETON_DATA *pSkeletonData = &SkeletonFrame.SkeletonData[i];
				
				if(pSkeletonData->eTrackingState != NUI_SKELETON_TRACKED)
					continue;
				
				GenerateRotateMatrix(pSkeletonData,g_matrix);
				
				D3DXVECTOR3 pos = (D3DXVECTOR3&)pSkeletonData->SkeletonPositions[0];
				D3DXMATRIX mat,mat2;
				D3DXMatrixScaling(&mat, 0.01f, 0.01f, 0.01f); //크기변환
				D3DXMatrixTranslation(&mat2, 0.0f, 0.0f, 0.0f);
				mat *= mat2;

				gGraphics->SetWorldMatrix(0,mat);
				gGraphics->SetKinectMatrix(0,g_matrix);

				for(int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
				{
					long DepthX, DepthY;
					USHORT DepthValue;

					NuiTransformSkeletonToDepthImage(
						pSkeletonData->SkeletonPositions[j],
						&DepthX, &DepthY, &DepthValue);

					g_pSensor->NuiImageGetColorPixelCoordinatesFromDepthPixel(
						NUI_IMAGE_RESOLUTION_640x480,
						0,
						DepthX, DepthY, DepthValue,
						&g_ColorPoints[j].x, &g_ColorPoints[j].y);

					//if(FirstState)
					//{
						TransBone[j].vOrigPos.x = pSkeletonData->SkeletonPositions[j].x;
						TransBone[j].vOrigPos.y = pSkeletonData->SkeletonPositions[j].y; 
						TransBone[j].vOrigPos.z = pSkeletonData->SkeletonPositions[j].z;						
					//}
					TransBone[j].vMovePos.x = pSkeletonData->SkeletonPositions[j].x;
					TransBone[j].vMovePos.y = pSkeletonData->SkeletonPositions[j].y;
					TransBone[j].vMovePos.z = pSkeletonData->SkeletonPositions[j].z;

					D3DXVECTOR3 vCross, vOrigPos, vMovePos;
					float Angle;
					vOrigPos = D3DXVECTOR3(TransBone[j+1].vOrigPos-TransBone[j].vOrigPos);
					vMovePos = D3DXVECTOR3(TransBone[j+1].vMovePos-TransBone[j].vMovePos);

					D3DXVec3Cross(&vCross, &vOrigPos, &vMovePos);					

					//D3DXVec3Normalize(&vPos, &vPos);
					//D3DXVec3Normalize(&vNextPos, &vNextPos);

					Angle = asin(D3DXVec3Length(&vCross)/(D3DXVec3Length(&vOrigPos)*D3DXVec3Length(&vMovePos)))*100.0f;

					D3DXVec3Normalize(&vCross, &vCross);

					//D3DXVECTOR3 vTransCross(vCross.y, -vCross.x, vCross.z);

					//else
						//gGraphics->BoneTranslation(j, TransBone[j].vChangeValue.x, TransBone[j].vChangeValue.y, TransBone[j].vChangeValue.z);
					//if(!FirstState)
						//TransBone[j].vOrigPos = TransBone[j].vMovePos;
				}
				FirstState=false;
				SelectObject(g_SkeletonDC, g_hPen[i%NUI_SKELETON_COUNT]);
				DrawSkeleton();				
			}
		}
	}
}

void Uninitialize()
{
	if(g_pSensor)
	{
		g_pSensor->NuiShutdown();
		g_pSensor->Release();
		g_pSensor = NULL;
	}

	for(int i = 0; i < NUI_SKELETON_COUNT; i ++)
	{
		if(g_hPen[i])
		{
			DeleteObject(g_hPen[i]);
			g_hPen[i] = NULL;
		}
	}

	if(g_SkeletonDC)
	{
		DeleteDC(g_SkeletonDC);
		g_SkeletonDC = NULL;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_F1:
					gGraphics->ToggleControlsDisplay();
				break;
				default:				
					break;
			}
			break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	LPTSTR lpClassName = "Kinect Skeleton Control";
	LPTSTR lpChildClass = "child";


	WNDCLASS wc1 = { CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0,
		hInstance, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, lpClassName};
	WNDCLASS wc2 = { CS_HREDRAW | CS_VREDRAW , DefWindowProc, 0, 0,
		hInstance, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, lpChildClass};
	WNDCLASS wc3 = { CS_HREDRAW | CS_VREDRAW , DefWindowProc, 0, 0,
		hInstance, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, lpChildClass};
	RegisterClass(&wc1);
	RegisterClass(&wc2);
	RegisterClass(&wc3);

	RECT	rtWnd = {0, 0, g_nWidth, g_nHeight};
	DWORD	dwStyle = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&rtWnd, dwStyle, FALSE);

	g_hWnd = CreateWindow(lpClassName, lpClassName, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rtWnd.right - rtWnd.left, rtWnd.bottom - rtWnd.top,
		NULL, NULL, wc1.hInstance, NULL);
	g_hChild1 = CreateWindow(lpChildClass,lpClassName, WS_VISIBLE | WS_CHILD,
		0,0,600,440,
		g_hWnd, NULL, wc1.hInstance, NULL);
	g_hChild2 = CreateWindow(lpChildClass,lpClassName, WS_VISIBLE | WS_CHILD,
		0,440,600,1000,
		g_hWnd, NULL, wc1.hInstance, NULL);
	g_hChild3 = CreateWindow(lpChildClass, lpClassName, WS_VISIBLE | WS_CHILD,
		600,0,600,1000,
		g_hWnd, NULL, wc1.hInstance, NULL);
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

	// CGraphics 클래스 객체 생성
	gGraphics=new CGraphics();
	if (!gGraphics->Initialise(g_hChild3))
	{
		delete gGraphics;
		return 0;
	}

	gGraphics->LoadSXFile();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime(); 

	if(Initialize(g_hChild1))
	{
		while(msg.message != WM_QUIT)
		{
			if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				float currTime  = (float)timeGetTime();
				float timeDelta = (currTime - lastTime)*0.001f;

				SkeletonFrameReady();
				ProcessColor();
				Draw();
				gGraphics->SetupCamera();	
				gGraphics->Update();
			}
		}
		Uninitialize();
	}
	
	delete gGraphics;
	return (int)msg.wParam;
}


//template <typename T> void SkeletonLoop(int index,D3DXMATRIX matrix,T lambda)
//{
//	matrix = g_matrix[index] * matrix;
//	lambda(index,matrix);
//
//	switch(index)
//	{
//	case NUI_SKELETON_POSITION_HIP_CENTER:
//		SkeletonLoop(NUI_SKELETON_POSITION_SPINE,matrix,lambda);
//		SkeletonLoop(NUI_SKELETON_POSITION_HIP_LEFT,matrix,lambda);
//		SkeletonLoop(NUI_SKELETON_POSITION_HIP_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_SPINE:
//		SkeletonLoop(NUI_SKELETON_POSITION_SHOULDER_CENTER,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_SHOULDER_CENTER:
//		SkeletonLoop(NUI_SKELETON_POSITION_HEAD,matrix,lambda);
//		SkeletonLoop(NUI_SKELETON_POSITION_SHOULDER_LEFT,matrix,lambda);
//		SkeletonLoop(NUI_SKELETON_POSITION_SHOULDER_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_SHOULDER_LEFT:
//		SkeletonLoop(NUI_SKELETON_POSITION_ELBOW_LEFT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_ELBOW_LEFT:
//		SkeletonLoop(NUI_SKELETON_POSITION_WRIST_LEFT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_WRIST_LEFT:
//		SkeletonLoop(NUI_SKELETON_POSITION_HAND_LEFT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_SHOULDER_RIGHT:
//		SkeletonLoop(NUI_SKELETON_POSITION_ELBOW_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_ELBOW_RIGHT:
//		SkeletonLoop(NUI_SKELETON_POSITION_WRIST_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_WRIST_RIGHT:
//		SkeletonLoop(NUI_SKELETON_POSITION_HAND_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_HIP_LEFT:
//		SkeletonLoop(NUI_SKELETON_POSITION_KNEE_LEFT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_KNEE_LEFT:
//		SkeletonLoop(NUI_SKELETON_POSITION_ANKLE_LEFT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_ANKLE_LEFT:
//		SkeletonLoop(NUI_SKELETON_POSITION_FOOT_LEFT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_HIP_RIGHT:
//		SkeletonLoop(NUI_SKELETON_POSITION_KNEE_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_KNEE_RIGHT:
//		SkeletonLoop(NUI_SKELETON_POSITION_ANKLE_RIGHT,matrix,lambda);
//		break;
//	case NUI_SKELETON_POSITION_ANKLE_RIGHT:
//		SkeletonLoop(NUI_SKELETON_POSITION_FOOT_RIGHT,matrix,lambda);
//		break;
//	}
//}


//벡터 3개로 회전행렬 구하는 함수
inline void GenerateMatrixFromVector(D3DXMATRIX* out ,const D3DXVECTOR3 &x,const D3DXVECTOR3 &y,const D3DXVECTOR3 &z)
{
	D3DXMATRIX m,mInv;
	(D3DXVECTOR3&)m._11 = x;
	(D3DXVECTOR3&)m._21 = y;
	(D3DXVECTOR3&)m._31 = z;
	m._14 = 0;
	m._24 = 0;
	m._34 = 0;
	m._41 = 0;
	m._42 = 0;
	m._43 = 0;
	m._44 = 1.f;

	*out = m;
}
inline void GenerateMatrixFromVectorY(D3DXMATRIX* out ,const D3DXVECTOR3 &toX,const D3DXVECTOR3 &toY)
{
	D3DXVECTOR3 x,y,z;
	
	D3DXVec3Normalize(&y,&toY); //Y벡터를 1로 만들기 위해서
	D3DXVec3Cross(&z ,&toX, &y); //X랑 Y를 외적 Z직교구함
	D3DXVec3Normalize(&z,&z); // 노말라이즈를 해서 길이를 1짜리로 만듬
	D3DXVec3Cross(&x ,&y, &z); //Y,Z길이 외적 해서X를 직교된 벡터를 만든다.

	GenerateMatrixFromVector(out,x,y,z);
}
inline void GenerateMatrixFromVectorX(D3DXMATRIX* out ,const D3DXVECTOR3 &toX,const D3DXVECTOR3 &toY)
{
	D3DXVECTOR3 x,y,z;
	
	D3DXVec3Normalize(&x,&toX);
	D3DXVec3Cross(&z ,&x, &toY);
	D3DXVec3Normalize(&z,&z);
	D3DXVec3Cross(&y ,&z, &x);

	GenerateMatrixFromVector(out,x,y,z);
}
inline void GenerateMatrixFromVectorZ(D3DXMATRIX* out ,const D3DXVECTOR3 &toY,const D3DXVECTOR3 &toZ)
{
	D3DXVECTOR3 x,y,z;
	
	D3DXVec3Normalize(&z,&toZ);
	D3DXVec3Cross(&x ,&toY, &z);
	D3DXVec3Normalize(&x,&x);
	D3DXVec3Cross(&y ,&z, &x);

	GenerateMatrixFromVector(out,x,y,z);
}
inline void SetParentMatrix(D3DXMATRIX* pChild,D3DXMATRIX* pFrom)
{
	D3DXMATRIX mInv;
	D3DXMatrixInverse(&mInv,NULL,pFrom);

	*pChild *= mInv;
}