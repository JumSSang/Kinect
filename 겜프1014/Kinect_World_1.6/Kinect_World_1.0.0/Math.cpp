#include "CvKinect.h"

void  Kinect::GenerateMatrixFromVectorY(D3DXMATRIX* out, const D3DXVECTOR3 &toX, const D3DXVECTOR3 &toY)
{
	D3DXVECTOR3 x, y, z;

	D3DXVec3Normalize(&y, &toY); //Y벡터를 1로 만들기 위해서
	D3DXVec3Cross(&z, &toX, &y); //X랑 Y를 외적 Z직교구함
	D3DXVec3Normalize(&z, &z); // 노말라이즈를 해서 길이를 1짜리로 만듬
	D3DXVec3Cross(&x, &y, &z); //Y,Z길이 외적 해서X를 직교된 벡터를 만든다.

	GenerateMatrixFromVector(out, x, y, z);
}
void Kinect::GenerateMatrixFromVectorZ(D3DXMATRIX* out, const D3DXVECTOR3 &toY, const D3DXVECTOR3 &toZ)
{
	D3DXVECTOR3 x, y, z;

	D3DXVec3Normalize(&z, &toZ);
	D3DXVec3Cross(&x, &toY, &z);
	D3DXVec3Normalize(&x, &x);
	D3DXVec3Cross(&y, &z, &x);

	GenerateMatrixFromVector(out, x, y, z);
}
//벡터 3개로 회전행렬 구하는 함수
void Kinect::GenerateMatrixFromVector(D3DXMATRIX* out, const D3DXVECTOR3 &x, const D3DXVECTOR3 &y, const D3DXVECTOR3 &z)
{
	D3DXMATRIX m, mInv;
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
void Kinect::GenerateMatrixFromVectorX(D3DXMATRIX* out, const D3DXVECTOR3 &toX, const D3DXVECTOR3 &toY)
{
	D3DXVECTOR3 x, y, z;

	D3DXVec3Normalize(&x, &toX);
	D3DXVec3Cross(&z, &x, &toY);
	D3DXVec3Normalize(&z, &z);
	D3DXVec3Cross(&y, &z, &x);

	GenerateMatrixFromVector(out, x, y, z);
}

bool Kinect::AABBvsAABB(AABB& a, AABB& b)
{
	if (a.maxPt.x < b.minPt.x || a.minPt.x>b.maxPt.x) return false;
	if (a.maxPt.y<b.minPt.y || a.minPt.y >b.maxPt.y)return false;

	return  true;
}