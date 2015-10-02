#include "CvKinect.h"

BOOL Kinect::Collusion(float l, float t, float r, float b, float x, float y)
{
	if (l<x&& r>x&&t<y&&b>y)
	{
		return true;
	}
	return false;
}

bool Kinect::BoundingCollusion(D3DXVECTOR3 *m1, D3DXVECTOR3 *m2, float m1r, float m2r)
{

	float distance = sqrt((float)((m2->x - m1->x)*(m2->x - m1->x)) + ((m2->y - m1->y)*(m2->y - m1->y)) + ((m2->z - m1->z)*(m2->z - m1->z)));
	if (m1r + m2r >= distance)
	{
		return TRUE;
	}
	else
		return FALSE;
}