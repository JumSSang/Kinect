
#include "CvKinect.h"
//static const float g_JointThickness = 3.0f;
//static const float g_TrackedBoneThickness = 6.0f;
//static const float g_InferredBoneThickness = 1.0f;
//





//D2D1_POINT_2F  Kinect::SkeletonToScreen(Vector4 skeletonPoint, int width, int height)
//{
//	LONG x, y;
//	USHORT depth;
//	//스크린에 뿌려줄 스켈레톤 위치를 계산한다.
//	//스켈레톤과 깊이 이미지를 320*240의 이미지 좌표로 공간에 할당해준다.
//	NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);
//	float screenPointX = static_cast<float>(x * width) / 640;
//	float screenPointY = static_cast<float>(y * height) / 480;
//	return D2D1::Point2F(screenPointX, screenPointY);
//}
//void Kinect::DrawBone(const NUI_SKELETON_DATA & skel, NUI_SKELETON_POSITION_INDEX joint0, NUI_SKELETON_POSITION_INDEX joint1)
//{
//	NUI_SKELETON_POSITION_TRACKING_STATE joint0State = skel.eSkeletonPositionTrackingState[joint0];
//	NUI_SKELETON_POSITION_TRACKING_STATE joint1State = skel.eSkeletonPositionTrackingState[joint1];
//
//	// If we can't find either of these joints, exit
//	if (joint0State == NUI_SKELETON_POSITION_NOT_TRACKED || joint1State == NUI_SKELETON_POSITION_NOT_TRACKED)
//	{
//		return;
//	}
//
//	// Don't draw if both points are inferred
//	if (joint0State == NUI_SKELETON_POSITION_INFERRED && joint1State == NUI_SKELETON_POSITION_INFERRED)
//	{
//		return;
//	}
//
//	// We assume all drawn bones are inferred unless BOTH joints are tracked
//	if (joint0State == NUI_SKELETON_POSITION_TRACKED && joint1State == NUI_SKELETON_POSITION_TRACKED)
//	{
//		m_pRenderTarget->DrawLine(m_Points[joint0], m_Points[joint1], m_pBrushBoneTracked, g_TrackedBoneThickness);
//	}
//	else
//	{
//		m_pRenderTarget->DrawLine(m_Points[joint0], m_Points[joint1], m_pBrushBoneInferred, g_InferredBoneThickness);
//	}
//}
//void Kinect::DrawSkeleton(const NUI_SKELETON_DATA & skel, int windowWidth, int windowHeight)
//{
//	int i;
//
//	for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
//	{
//		m_Points[i] = SkeletonToScreen(skel.SkeletonPositions[i], windowWidth, windowHeight);
//	}
//
//	// Render Torso
//	DrawBone(skel, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
//	DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
//	DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);
//	DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SPINE);
//	DrawBone(skel, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_HIP_CENTER);
//	DrawBone(skel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT);
//	DrawBone(skel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT);
//
//	// Left Arm
//	DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
//	DrawBone(skel, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
//	DrawBone(skel, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
//
//	// Right Arm
//	DrawBone(skel, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
//	DrawBone(skel, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
//	DrawBone(skel, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
//
//	// Left Leg
//	DrawBone(skel, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT);
//	DrawBone(skel, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT);
//	DrawBone(skel, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
//
//	// Right Leg
//	DrawBone(skel, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT);
//	DrawBone(skel, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT);
//	DrawBone(skel, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
//
//	// Draw the joints in a different color
//	for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
//	{
//		D2D1_ELLIPSE ellipse = D2D1::Ellipse(m_Points[i], g_JointThickness, g_JointThickness);
//
//		if (skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_INFERRED)
//		{
//			m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointInferred);
//		}
//		else if (skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_TRACKED)
//		{
//			m_pRenderTarget->DrawEllipse(ellipse, m_pBrushJointTracked);
//		}
//	}
//}