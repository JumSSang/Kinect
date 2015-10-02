#include "CvKinect.h"

void Kinect::SetTextureFromBuffer(LPDIRECT3DTEXTURE9 pTex, void * buffer, int x, int y)
{
	D3DLOCKED_RECT lock;
	if (SUCCEEDED(pTex->LockRect(0, &lock, NULL, D3DLOCK_DISCARD)))
	{
		int srcpitch = g_nWidth * 4;
		for (int y = 0; y < g_nHeight; y++)
		{
			void* src = (BYTE*)buffer + y*srcpitch;
			void* dest = (BYTE*)lock.pBits + y*lock.Pitch;
			memcpy(dest, src, srcpitch);
		}
		pTex->UnlockRect(0);

	}



}

void Kinect::DepthSetTextureFromBuffer(LPDIRECT3DTEXTURE9 pTex, void * buffer, int x, int y)
{
	D3DLOCKED_RECT lock;
	if (SUCCEEDED(pTex->LockRect(0, &lock, NULL, D3DLOCK_DISCARD)))
	{
		int srcpitch = g_nWidth * 4;
		for (int y = 0; y < g_nHeight; y++)
		{
			void* src = (BYTE*)buffer + y*srcpitch;
			void* dest = (BYTE*)lock.pBits + y*lock.Pitch;
			memcpy(dest, src, srcpitch);
		}
		pTex->UnlockRect(0);

	}
}