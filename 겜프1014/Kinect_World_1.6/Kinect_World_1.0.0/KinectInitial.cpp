#include "CvKinect.h"
struct PositionTranslation
{
	D3DXVECTOR3 vOrigPos;
	D3DXVECTOR3 vMovePos;
	D3DXVECTOR3 vChangeValue;
};


struct PositionTranslation TransBone[20] = { { D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0) }, };



Kinect::Kinect()
{
	memset(&m_skeletonFrame, 0, sizeof(m_skeletonFrame));
}
bool Kinect::KinectInitialize()
{
	HRESULT hr;


	hr = NuiCreateSensorByIndex(0, &m_pSensor);//생성된 키넥트의 갯수 가져오고 0이면 1대이다.  //g_pSensor는 INuiSensor를 참조받을 포인트
	if (FAILED(hr))
	{
		MessageBox(NULL, "키넥트 연결이 되어있지않음 -실패원인 ", NULL, NULL);
		return false;

	}
	//NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR
	hr = m_pSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (FAILED(hr))
	{
		MessageBox(NULL, "키넥트의 초기화 실패함 - Failed.", NULL, NULL);
		return false;
	}
	hr = m_pSensor->NuiSkeletonTrackingEnable(NULL, 0);
	if (FAILED(hr))
	{
		MessageBox(NULL, "스켈레톤 트래킹 활성화 실패- Failed.", NULL, NULL);
		return false;
	}

	// 깊이 스트림 생성 
	hr = m_pSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, // <-
		NUI_IMAGE_RESOLUTION_640x480,
		0, 2,
		NULL,
		&m_hVideoStream);

	if (FAILED(hr))
	{
		MessageBox(NULL, "키넥트 스트림데이터를 꺼내는데 실패하였습니다. - Failed.", NULL, NULL);
		return false;
	}

	//색깔 스트림 생성
	hr = m_pSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR, // <- 
		NUI_IMAGE_RESOLUTION_640x480,
		0, 2,
		NULL,
		&m_ColorStream);

	if (FAILED(hr))
	{
		MessageBox(NULL, "키넥트 스트림데이터를 꺼내는데 실패하였습니다. - Failed.", NULL, NULL);
		return false;
	}
	return true;
}
void Kinect::KinectUninitialize()
{
	if (m_pSensor)
	{
		m_pSensor->NuiShutdown();
		m_pSensor->Release();
		m_pSensor = NULL;
		SoundRelease();


	}
}

void Kinect::VideoFrameReady(float dt)
{
	//Animate();



	HRESULT hr;



	///////////////////////////////////////////////



	NUI_IMAGE_FRAME ImageFrame;
	NUI_SKELETON_FRAME skeletonFrame;
	hr = m_pSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if (SUCCEEDED(hr))
	{
		m_pSensor->NuiTransformSmooth(&skeletonFrame, NULL);
		m_skeletonFrame = skeletonFrame;
	}

	//깊이 출력부분
	hr = m_pSensor->NuiImageStreamGetNextFrame(m_hVideoStream, 0, &ImageFrame);
	if (SUCCEEDED(hr))
	{
		INuiFrameTexture* pFrameTexture = ImageFrame.pFrameTexture;
		NUI_LOCKED_RECT	LockedRect;

		pFrameTexture->LockRect(0, &LockedRect, 0, 0);
		if (LockedRect.Pitch != 0)
		{
			WORD* pBuf = (WORD*)LockedRect.pBits;
			DWORD * buffer = new DWORD[g_nWidth * g_nHeight];

			int nSize = g_nWidth*g_nHeight;

			for (int i = 0; i < nSize; i++)
			{
				int c = NuiDepthPixelToDepth(pBuf[i]) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
				buffer[i] = (c) | (c << 8) | (c << 16);
			}
			//DepthSetTextureFromBuffer(m_Depth_pVideoTexture, buffer, 640, 0);
			//SetTextureFromBuffer(m_pVideoTexture, buffer, 640, 0);

			delete buffer;
			// 네거


		}
		m_pSensor->NuiImageStreamReleaseFrame(m_hVideoStream, &ImageFrame);
	}

	//색 출력부분
	hr = m_pSensor->NuiImageStreamGetNextFrame(m_ColorStream, 0, &ImageFrame);
	if (SUCCEEDED(hr))
	{
		INuiFrameTexture* pFrameTexture = ImageFrame.pFrameTexture;
		NUI_LOCKED_RECT	LockedRect;
		pFrameTexture->LockRect(0, &LockedRect, 0, 0);

		if (LockedRect.Pitch != 0)
		{
			BYTE * pBuffer = (BYTE*)LockedRect.pBits;
			//cvSetData(KinectColorImg, pBuffer, LockedRect.Pitch);
				SetTextureFromBuffer(m_pVideoTexture, LockedRect.pBits, 0, 0);
		}
		m_pSensor->NuiImageStreamReleaseFrame(m_ColorStream, &ImageFrame);
	}




	static const D3DXCOLOR COLOR[] = {
			{ 1, 0, 0, 1 },
			{ 0, 1, 0, 1 },
			{ 0, 0, 1, 1 },
			{ 1, 1, 0, 1 },
			{ 1, 0.5f, 0, 1 },
			{ 1, 0, 1, 1 },
	};

	D3DXVECTOR3 Acc[8];
	for (int i = 0; i < 9; i++)
	{
	}


	for (int player = 0; player < NUI_SKELETON_COUNT; player++)
	{
		long x = 0, y = 0;
		long lx = 0, ly = 0;
		NUI_SKELETON_DATA *dat = &m_skeletonFrame.SkeletonData[player];
		if (dat->eTrackingState == NUI_SKELETON_NOT_TRACKED) continue;
		D3DXVECTOR3 sc = (D3DXVECTOR3&)dat->SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
		D3DXVECTOR3 sl = (D3DXVECTOR3&)dat->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		D3DXVECTOR3 sr = (D3DXVECTOR3&)dat->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
		D3DXVECTOR3 templook, rc, lc;
		m_handRight = (D3DXVECTOR3&)dat->SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
		m_handLeft = (D3DXVECTOR3&)dat->SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
		center_head = sc;
		//CameraSpot.x = sc.x;
		//CameraSpot.y = sc.y;
		rc = sr - sc;
		lc = sl - sc;
		D3DXVec3Cross(&templook, &rc, &lc);
		D3DXVec3Normalize(&templook, &templook);

		IronLook = templook;
		tempLookat = IronLook;



		//3가지 구해서 앞방향 구하고 


		GenerateRotateMatrix(dat, m_matrix);
		bCaptured = true;
		//D3DXMatrixScaling(&playerScale, 0.02f, 0.02f, 0.02f); //크기변환
		irondmanposx = 0.0f + center_head.x;
		irondmanposz = 2.0f + center_head.z;
		ironmanposy = -1.0f;
		D3DXMatrixTranslation(&playerTrans, irondmanposx, ironmanposy, irondmanposz);
		playerScale *= playerTrans;
		SetWorldMatrix(0, playerScale);
		SetKinectMatrix(0, m_matrix);

		for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
		{
			long DepthX, DepthY;
			USHORT DepthValue;

			NuiTransformSkeletonToDepthImage(
				dat->SkeletonPositions[j],
				&DepthX, &DepthY, &DepthValue);
			TransBone[j].vOrigPos.x = dat->SkeletonPositions[j].x;
			TransBone[j].vOrigPos.y = dat->SkeletonPositions[j].y;
			TransBone[j].vOrigPos.z = dat->SkeletonPositions[j].z;
			TransBone[j].vMovePos.x = dat->SkeletonPositions[j].x;
			TransBone[j].vMovePos.y = dat->SkeletonPositions[j].y;
			TransBone[j].vMovePos.z = dat->SkeletonPositions[j].z;
			D3DXVECTOR3 vCross, vOrigPos, vMovePos;
			float Angle;
			vOrigPos = D3DXVECTOR3(TransBone[j + 1].vOrigPos - TransBone[j].vOrigPos);
			vMovePos = D3DXVECTOR3(TransBone[j + 1].vMovePos - TransBone[j].vMovePos);
			D3DXVec3Cross(&vCross, &vOrigPos, &vMovePos);

			//D3DXVec3Normalize(&vPos, &vPos);
			//D3DXVec3Normalize(&vNextPos, &vNextPos);

			Angle = asin(D3DXVec3Length(&vCross) / (D3DXVec3Length(&vOrigPos)*D3DXVec3Length(&vMovePos)))*100.0f;

			D3DXVec3Normalize(&vCross, &vCross);


		}
	}

	if (gamestart == 0)
	{

		GameUpdate(dt);
		DrawLeftScene(dt);
		DrawRightScene(dt);
		DrawUpScene(dt);
	}
	if (gamestart == 1)
	{
		MenuUpdate(dt);
		DrawMenuSceen(dt);
	}
	SetWindowText(m_hwnd, bCaptured ? "[Captured]" : "[Uncaptured]");
	g_pDevice->Present(nullptr, nullptr, nullptr, nullptr);


}





VOID Kinect::Animate()
{
	static DWORD t = 0;
	static bool flag = false;
	/// 0 ~ 2PI 까지(0~360도) 값을 변화시킴 Fixed Point기법 사용
	DWORD d = GetTickCount() % ((int)((D3DX_PI * 2) * 1000));
	/// Y축 회전행렬
	D3DXMatrixRotationY(&g_matAni, d / 1000.0f);
	//	D3DXMatrixIdentity( &g_matAni );

	/// 카메라 행렬설정
	SetupMatrics();
	SetupLight();

	if (d < t)
		flag = !flag;
	g_pDevice->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	t = d;


}

void Kinect::EffectSoundUpdate(int value)
{
	m_pSystem->playSound(FMOD_CHANNEL_REUSE, m_pSound[value], 0, &m_pChannel[1]);
	m_pChannel[1]->setVolume(1.0f);
}

void Kinect::SoundUpdate(int value)
{
	m_pSystem->playSound(FMOD_CHANNEL_REUSE, m_pSound[value], 0, &m_pChannel[2]);
	m_pChannel[2]->setVolume(0.5f);
}
void Kinect::BackSound(int value)
{
	m_pSystem->playSound(FMOD_CHANNEL_REUSE, m_pSound[value], 0, &m_pChannel[0]);
	m_pChannel[0]->setVolume(0.2f);

}
void Kinect::SoundRelease()
{
	m_pSystem->release();
	m_pSystem->close();
}




void Kinect::DrawLeftScene(float dt)
{
	g_pDevice->SetViewport(&viewLeft);

	// D3D 렌더링 시작

	g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x000000, 1.f, 0);
	g_pDevice->BeginScene();
	IronLook = { 0.0f, 0.0f, 1.0f };
	m_cameray = 1.0f;
	CameraSpot.x = 0.0f;
	CameraSpot.y = 1.0f;
	CameraSpot.z = -1.0f;
	lightdir = 90.0f;
	SetupMatrics();
	static char sbuffer[256];
	sprintf_s(sbuffer, "?? : %d ", Score);
	RECT RS = { 0, 0, 0, 0 };
	//DrawMesh(&mBoundingBoxAreaTran[0]);
	if (gamestart == 0)
	{
		m_goalpost.MeshDraw(m_GoalpostS*m_GoalPostT);
	
		mFont->DrawText(NULL, sbuffer, -1, &RS, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
		enemy.MeshDraw(enemyS*enemyT);
		for (int i = 0; i < footballArr.size(); i++)
		{
			if (footballArr.size() != NULL)
			{ 
				if (footballArr[i].position.z > 2)
				{
					m_football.m_pMeshMaterials->Diffuse.r = 255;
				}
				else
				{
					m_football.m_pMeshMaterials->Diffuse.r = 0;
				}
			m_football.MeshDraw(footballArr[i].m_Sclae * footballArr[i].m_Trans);
			}
		}
		for (int i = 0; i < 13; i++)
		{
			g_pDevice->SetTransform(D3DTS_WORLD, &(mBoundingBoxAreaScale[i] * mBoundingBoxAreaTran[i]));
		//	m_pBox->DrawSubset(0);
		}
		if (bCaptured == true)
		{
			m_player->MeshRender();
		}
		for (int i = 0; i < fire.size(); i++)
		{
			fire[i].Draw();
		}

		
		stadium.MeshDraw(S*R*T);

	}
	else if (gamestart == 1)
	{

	}
	//fireball.Draw();
	//	DrawMesh(&mBoundingBoxAreaTran[0]);


	g_pDevice->EndScene();
}

void Kinect::DrawRightScene(float dt)
{
	//오른쪽 ViewPort
	g_pDevice->SetViewport(&viewRight);
	// D3D 렌더링 시작

	g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0000000, 1.f, 0);
	g_pDevice->BeginScene();
	IronLook = tempLookat;
	m_cameray = 1.0f;
	CameraSpot.y = 1.0f;
	CameraSpot.z = 8.0f;
	lightdir = 180.0f;
	SetupMatrics();
	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	D3DXMATRIX matView, mirror;
	g_pDevice->GetTransform(D3DTS_VIEW, &matView); //생성한 뷰 행렬을 디바이스에 설정
	D3DXMatrixScaling(&mirror, -1, 1, 1);
	matView = mirror * matView;
	g_pDevice->SetTransform(D3DTS_VIEW, &matView); //생성한 뷰 행렬을 디바이스에 설정
	if (gamestart == 0)
	{
		//m_pBox->DrawSubset(0);
		

		enemy.MeshDraw(enemyS*enemyT);
		static char sbuffer[256];
		sprintf_s(sbuffer, "점수 : %d  ", DefenceScore);
		RECT RS = { 900, 0, 1600, 0 };
		mFont->DrawText(NULL, sbuffer, -1, &RS, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

		m_football.MeshDraw(rox * ballScale * ballTrans);
		stadium.MeshDraw(S*R*T);
		m_goalpost.MeshDraw(m_GoalpostS*m_GoalPostT);
		g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		if (bCaptured == true)
		{
			m_player->MeshRender();
		}

		g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		for (int i = 0; i < footballArr.size(); i++)
		{
			if (footballArr.size() != NULL)
			m_football.MeshDraw(footballArr[i].m_Sclae * footballArr[i].m_Trans);
		}
	}

	else if (gamestart == 1)
	{
	}

	g_pDevice->EndScene();
}
void Kinect::DrawUpScene(float dt)
{
	g_pDevice->SetViewport(&viewCenter);
	D3DXMATRIX matrix;


	g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffff, 1.f, 0);
	g_pDevice->BeginScene();
	CameraSpot = { 0.0f, 0.0f, 0.9f };
	IronLook = { 0.0f, 0.0f, 1.0f };
	SetupMatrics();
	if (gamestart == 0)
	{
		
		//stadium.MeshDraw(S*R*T);
		//
		RenderTexture(m_pVideoTexture, -0, -0, 12.2f); //카메라 시점문제입니다~ 카메라 설정변경 부탁드림
		//RenderTexture(m_Depth_pVideoTexture, -3, -3, 12.2f); //카메라 시점문제입니다~ 카메라 설정변경 부탁드림
	}

	g_pDevice->EndScene();

}
void Kinect::GameUpdate(float dt)
{

	D3DXMatrixTranslation(&playerTrans, 0.0f, -1.0f, 4.0f);
	D3DXMatrixScaling(&playerScale, 0.005f, 0.005f, 0.005f);
	
	mBoxMtrl.ambient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f);
	mBoxMtrl.diffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.1f);
	mBoxMtrl.spec = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
	mBoxMtrl.specPower = 8.0f;
	D3DXMatrixTranslation(&T, 0.0f, -1.0f, 4.0f);
	D3DXMatrixRotationX(&R, 30);
	D3DXMatrixScaling(&S, 0.002f, 0.002f, 0.002f);
	D3DXMatrixTranslation(&mBoundingRightTr, m_handRight.x, m_handRight.y, m_handRight.z * 2);

	D3DXMatrixTranslation(&enemyT, 0.0f, -1.0f, 0.0f);
	D3DXMatrixScaling(&enemyS, 10.0f, 7.0f, 10.0f);


	for (int i = 0; i < 12; i++)
	{
		D3DXMatrixTranslation(&mBoundingBoxAreaTran[i], BoundingArea[i].x, BoundingArea[i].y, BoundingArea[i].z);
	}

	for (int i = 0; i < 12; i++)
	{
		D3DXMatrixScaling(&mBoundingBoxAreaScale[i], 1.0f, 0.5f, 1.0f);
	}
	D3DXMatrixTranslation(&m_GoalPostT, -0.5f, -1.6f, 5.2);
	D3DXMatrixScaling(&m_GoalpostS, 0.17f, 0.09f, 0.05f);


	//D3DXMatrixRotationY(&mBoundRotate, 90);
	//m_pBox->Matrial
	/////////////////////////////////////////////// 공생성
		
	if (footballArr.size() != NULL)	{
		for (int i = 0; i < footballArr.size(); i++)
		{
			float x, y, z;
			D3DXVECTOR3 tempVector;
			tempVector = BoundingArea[footballArr[i].gospot] + D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			x = pow(tempVector.x, 2);
			y = pow(tempVector.y, 2);
			z = pow(tempVector.z, 2);
			float 제곱 = sqrt(x + y + z);
			x = (tempVector.x / 제곱)*0.003f;
			y = (tempVector.y / 제곱)*0.003f;
			z = (tempVector.z / 제곱) *0.003f;

			if (footballArr[i].state == false)
			{
				footballArr[i].xspeed += x;
				footballArr[i].yspeed += y;
				footballArr[i].zspeed += z;
				footballArr[i].position.x += footballArr[i].xspeed;
				footballArr[i].position.y += footballArr[i].yspeed;
				footballArr[i].position.z += footballArr[i].zspeed;
			}
			else
			{
				footballArr[i].xspeed += x;
				footballArr[i].yspeed += y;
				footballArr[i].zspeed += z;
				footballArr[i].position.x += footballArr[i].xspeed;
				footballArr[i].position.y += footballArr[i].yspeed;
				footballArr[i].position.z -= footballArr[i].zspeed;
			}
			//if BoundingCollusion(&footballArr[i].position,m)
			D3DXVECTOR3 aa;
			aa = m_handRight;

			aa.z *= 3;
			if (footballArr.size() != NULL)
			D3DXMatrixTranslation(&footballArr[i].m_Trans, footballArr[i].position.x, footballArr[i].position.y, footballArr[i].position.z);

			if (BoundingCollusion(&footballArr[i].position, &aa, 0.2f, 0.2f))
			{
				footballArr[i].state = true;
				EffectSoundUpdate(1);
				DefenceScore++;
				
			}

			for (int j = 0; j < 11; j++)
			{
				if (BoundingCollusion(&footballArr[i].position, &BoundingArea[j], 0.5f, 0.5f))
				{
					//	fire.Position = { footballArr[i].position.x, footballArr[i].position.y, footballArr[i].position.z };
					Sprite_Extend tempSprite;
					tempSprite = fireball;
					tempSprite.Position = { footballArr[i].position.x, footballArr[i].position.y, footballArr[i].position.z };

					fire.push_back(tempSprite);
				//	footballArr.erase(footballArr.begin() + i);
					Score++;
					}
			}


			for (int k = 0; k < fire.size(); k++)
			{

				fire[k].UpdateAnimationOne(dt, 200, fireRect, 5);
				if (fire[k].State == false)
				{
					fire.erase(fire.begin() + k);
				}

			}

			

			if (footballArr[i].position.z > 8.0f)
			{
				footballArr.erase(footballArr.begin() + i);
			}

		}
	}
}

void Kinect::DrawMenuSceen(float dt)
{
	g_pDevice->SetViewport(&viewMenu);
	


	g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffff, 1.f, 0);
	g_pDevice->BeginScene();

	m_main_img.Draw();
	m_ballCall_img.Draw();
	if (menustate == true)
	{
		m_StartButton_img.Draw();
		m_SettingButton_img.Draw();
		m_EndButton_img.Draw();
		m_autioButton.Draw();
	}
	g_pDevice->EndScene();

}
void Kinect::MenuUpdate(float dt)
{
	if (m_ballCall_img.Position.y < 650)
	{
		m_ballCall_img.Position.y += 1;
		
		
	}
	else
	{
		menustate = true;
		RECT R_start;
		RECT R_Setting;
		RECT R_END;
		RECT R_A;
		R_start.left = m_StartButton_img.Position.x;
		R_start.top = m_StartButton_img.Position.y;
		R_start.right = R_start.left + 410;
		R_start.bottom = R_start.left + 94;
		////
		R_Setting.left = m_SettingButton_img.Position.x;
		R_Setting.top = m_SettingButton_img.Position.y;
		R_Setting.right = R_Setting.left + 410;
		R_Setting.bottom = R_Setting.left + 94;
		//
		R_END.left = m_EndButton_img.Position.x;
		R_END.top = m_EndButton_img.Position.y;
		R_END.right = R_END.left + 410;
		R_END.bottom = R_END.left + 94;
		//
		R_A.left = m_autioButton.Position.x;
		R_A.top = m_autioButton.Position.y;
		R_A.right = R_A.left + 410;
		R_A.bottom = R_A.left + 94;
		
		if (0 <m_mousex&& 410>m_mousex&&200<m_mousey&&293>m_mousey)
		{
			g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffff, 1.f, 0);
			//MessageBox(m_hwnd, "dwqd", NULL, NULL);
			m_StartButton_img.rect = ButtonRECT[1];
			gamestart = 0;
		}
		if (0 <m_mousex && 410>m_mousex && 300<m_mousey && 393>m_mousey)
		{

			m_SettingButton_img.rect = ButtonRECT[1];
		}

		
	
	}
}

void Kinect::FootBallAcc(int a)
{
	MatStruct tempArr;
	tempArr.position = { 0.0f, 0.0f, 0.0f };
	D3DXMatrixScaling(&tempArr.m_Sclae, 1.0f, 1.0f, 1.0f);
	tempArr.gospot = a;
	footballArr.push_back(tempArr);
	m_ballcreate = 0;
	SoundUpdate(2);
	
}
bool Kinect::Collision(int x, int y, RECT &rs)
{
	if (rs.left<x && rs.right>x &&rs.top<y&&rs.bottom>y)
	{
		return true;
	}
	return false;

}