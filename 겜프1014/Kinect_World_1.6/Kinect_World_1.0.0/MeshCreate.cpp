#include "CvKinect.h"

void Kinect::InitMesh()
{
	//CSkinnedMesh m_football;
	//CSkinnedMesh m_goalpost;
	m_football.LoadMesh(m_pd3dDevice, "..Data/Soccer/FootBall.X");
}