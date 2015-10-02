#include "CvKinect.h"


char xpoint[10];
int pixeltext;
char ypoint[10];
//char zPoint[10];

int x;
int y;
int z;

Kinect Twit;

char skelzbuffer[10];



LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_NUMPAD1:
			Twit.FootBallAcc(1);
			break;
		case VK_NUMPAD2:
			Twit.FootBallAcc(2);
			break;
		case VK_NUMPAD3:
			Twit.FootBallAcc(3);
			break;
		case VK_NUMPAD4:
			Twit.FootBallAcc(4);
			break;
		case VK_NUMPAD5:
			//Twit.FootBallAcc(5);
			break;
		case VK_NUMPAD6:
			Twit.FootBallAcc(6);
			break;
		case VK_NUMPAD7:
			Twit.FootBallAcc(7);
			break;
		case VK_NUMPAD8:
			Twit.FootBallAcc(8);
			break;
		}

		break;
	case WM_LBUTTONDOWN:
		Twit.m_mousex = LOWORD(lParam);
		Twit.m_mousey = HIWORD(lParam);
		
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	static float lastTime = (float)timeGetTime();
	LPTSTR lpClassName = "≈∞≥ÿ∆Æ";
	WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, MsgProc, 0, 0,
		hInst, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, lpClassName };
	RegisterClass(&wc);
	RECT	rtWnd = { 0, 0, Twit.CLIENT_WIDTH, Twit.CLIENT_HEIGHT };
	AdjustWindowRect(&rtWnd, WS_CAPTION | WS_SYSMENU, FALSE);
	Twit.m_hwnd = CreateWindow(lpClassName, lpClassName, WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, rtWnd.right - rtWnd.left, rtWnd.bottom - rtWnd.top,
		NULL, NULL, wc.hInstance, NULL);
	ShowWindow(Twit.m_hwnd, SW_SHOW);
	//cvShowImage("OpenCV_Kinect!",Twit.KinectColorImg);
	UpdateWindow(Twit.m_hwnd);
	//CV.popupcv();
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	/// ≈∞≥ÿ∆Æ√ ±‚»≠
	if (SUCCEEDED(Twit.D3DStartup(Twit.m_hwnd)))
	{
		Twit.BackSound(3);
		
		if (SUCCEEDED(Twit.InitVB()))
		{


			if (SUCCEEDED(Twit.KinectInitialize()))
			{

				while (msg.message != WM_QUIT)
				{
					if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
					{
					
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else
					{
						float currTime = (float)timeGetTime();
						float timeDelta = (currTime - lastTime)*0.001f;
						Twit.VideoFrameReady(timeDelta);

						lastTime = currTime;
					}
					//CV.KinectColorImg = Twit.KinectColorImg;

				}
				Twit.KinectUninitialize();
			}
		}
		Twit.D3DCleanup();

	}
	return (int)msg.wParam;

}