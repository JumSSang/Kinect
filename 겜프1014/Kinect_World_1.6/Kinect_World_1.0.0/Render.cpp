#include "CvKinect.h"
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)


void Kinect::RenderTexture(LPDIRECT3DTEXTURE9 pTex, float x, float y, float z)
{

	
	SetupLight();
	
	//g_pDevice->SetRenderState(D3DRS_LIGHTING, false); // 라이팅 끄기
	//g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // 컬링 끄기
	//g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // 컬링 끄기
	g_pDevice->SetTexture(0, pTex); // 텍스처 설정
	g_pDevice->SetRenderState(D3DRS_ZENABLE, false);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);  //오퍼레이터에 의해서 Argument를 게산
	g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTSS_ALPHAARG1);
	//g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);  //오퍼레이터에 의해서 Argument를 게산
	//g_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
	D3DXMATRIX matrix;
	D3DXMatrixTranslation(&matrix, x, y, z);
	g_pDevice->SetTransform(D3DTS_WORLD, &matrix);
	g_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	

	g_pDevice->SetRenderState(D3DRS_ZENABLE, true);

}


void Kinect::GenerateRotateMatrix(NUI_SKELETON_DATA* data, D3DXMATRIX * mat)
{
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

	for (int i = 0; i<NUI_SKELETON_POSITION_COUNT; i++)
	{
		D3DXMatrixIdentity(&mat[i]);
	}

	static const D3DXVECTOR3 axisZ(0, 0, 1);

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HIP_CENTER],
		vHipRight - vHipLeft,
		vHipCenter - (vHipLeft + vHipRight) * 0.5f); //힙회전

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_SPINE],
		(vShoulderRight + vHipRight)*0.5f - vSpine,
		vShoulderCenter - vSpine); //상체회전

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_SHOULDER_CENTER],
		vShoulderRight - vShoulderLeft,
		vShoulderCenter - vSpine); //숄더 센터부분에서 회전
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HEAD],
		vShoulderRight - vShoulderLeft,
		vHead - vShoulderCenter); //머리회전

	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_SHOULDER_RIGHT],
		vElbowRight - vShoulderRight,
		vShoulderCenter - vSpine); //오른쪽어깨 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_ELBOW_RIGHT],
		vWristRight - vElbowRight,
		vShoulderCenter - vSpine); //팔꿈치 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_WRIST_RIGHT],
		vHandRight - vWristRight,
		vShoulderCenter - vSpine); //손목회전
	mat[NUI_SKELETON_POSITION_HAND_RIGHT] = mat[NUI_SKELETON_POSITION_WRIST_RIGHT];// NUI_SKELETON_POSITION_HAND_RIGHT - 손목과 동일한 각도로 밖에 계산이 불가능. 손회전

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HIP_RIGHT],
		vHipRight - vHipCenter,
		vHipRight - vKneeRight); //오른쪽 엉덩이 회전
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_KNEE_RIGHT],
		vHipRight - vHipCenter,
		vKneeRight - vAnkleRight); //무릎회전
	GenerateMatrixFromVectorZ(&mat[NUI_SKELETON_POSITION_ANKLE_RIGHT],
		vKneeRight - vAnkleRight,
		vAnkleRight - vFootRight);// 발목회전
	mat[NUI_SKELETON_POSITION_FOOT_RIGHT] = mat[NUI_SKELETON_POSITION_ANKLE_RIGHT]; //오른쪽 발회전

	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_SHOULDER_LEFT],
		vShoulderLeft - vElbowLeft,
		vShoulderCenter - vSpine); //왼쪽어깨 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_ELBOW_LEFT],
		vElbowLeft - vWristLeft,
		vShoulderCenter - vSpine); //왼쪽팔꿈치 회전
	GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_WRIST_LEFT],
		vWristLeft - vHandLeft,
		vShoulderCenter - vSpine); //왼쪽 손목회전
	mat[NUI_SKELETON_POSITION_HAND_LEFT] = mat[NUI_SKELETON_POSITION_WRIST_LEFT]; //왼손회전

	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_HIP_LEFT],
		vHipCenter - vHipLeft,
		vHipLeft - vKneeLeft); //왼쪽 엉덩이 회전
	GenerateMatrixFromVectorY(&mat[NUI_SKELETON_POSITION_KNEE_LEFT],
		vHipCenter - vHipLeft,
		vKneeLeft - vAnkleLeft); //왼쪽 무릎회전
	GenerateMatrixFromVectorZ(&mat[NUI_SKELETON_POSITION_ANKLE_LEFT],
		vKneeLeft - vAnkleLeft,
		vAnkleLeft - vFootLeft); //왼쪽 발목 회전
	mat[NUI_SKELETON_POSITION_FOOT_LEFT] = mat[NUI_SKELETON_POSITION_ANKLE_LEFT]; //왼쪽 발회전


	//GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_ELBOW_RIGHT]		,vShoulderRight		,vElbowRight		,axisZ);
	//GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_WRIST_RIGHT]		,vElbowRight		,vWristRight		,axisZ);
	//GenerateMatrixFromVectorX(&mat[NUI_SKELETON_POSITION_HAND_RIGHT]		,vWristRight		,vHandRight			,axisZ);
}