#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "Sprite.h"

Sprite::Sprite(void) : m_pd3dDevice(0), Position(0, 0, 0), Center(0.0f, 0.0f, 0.0f),
mSprite(0), m_Image(0), State(true), Color(D3DCOLOR_RGBA(255, 255, 255, 255))
{

}

bool Sprite::LoadFile(LPDIRECT3DDEVICE9 d3dDevice, D3DXVECTOR3 _Position, D3DXVECTOR3 _Center, char* filename)
{
	RECT _rect = { 0 };
	rect = _rect;
	m_pd3dDevice = d3dDevice;

	Position = _Position;
	Center = _Center;

	D3DXCreateSprite(m_pd3dDevice, &mSprite);

	if (FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &m_Image)))
	{
		MessageBoxA(NULL, filename, "Error", MB_ICONERROR);
	}
	return true;

}
bool Sprite::LoadFile(LPDIRECT3DDEVICE9 d3dDevice, RECT _rect, D3DXVECTOR3 _Position, D3DXVECTOR3 _Center, char* filename)
{
	m_pd3dDevice = d3dDevice;
	rect = _rect;
	Position = _Position;
	Center = _Center;

	D3DXCreateSprite(m_pd3dDevice, &mSprite);

	if (FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice, filename,
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &m_Image)))
		MessageBox(NULL, filename, "Error", MB_ICONERROR);

	return true;
}
void Sprite::Draw()
{
	if (State)
	{
		//스프라이트 자체에서 알파 블랜드를 걸수 있다.
		mSprite->Begin(D3DXSPRITE_ALPHABLEND);

		D3DXMATRIXA16 matTrans;

		D3DXMatrixTranslation(&matTrans, Position.x, Position.y, 0.0f);
		mSprite->SetTransform(&matTrans);

		if (rect.right == 0 && rect.bottom == 0)
			mSprite->Draw(m_Image, 0, &D3DXVECTOR3(Center.x, Center.y, 0.0f), 0, Color);
		else
			mSprite->Draw(m_Image, &rect, &D3DXVECTOR3(Center.x, Center.y, 0.0f), 0, Color);

		mSprite->End();
	}
}


Sprite_Extend::Sprite_Extend()
{
	tempClipCounter = 0;
	timeAccum = 0.0f;
}


bool Sprite_Extend::UpdateAnimationOne(float timeDelta, unsigned int FPS, RECT* ClipBox, unsigned int MaxClipCounter)
{

	timeAccum += timeDelta;
	// Play animation at 30 frames per second.
	if (timeAccum >= 1.0f / FPS)
	{
		tempClipCounter++;
		timeAccum = 0;
	}
	if (tempClipCounter >= MaxClipCounter)
	{
		rect = ClipBox[MaxClipCounter];
		State = false;
	}
	else
	{
		rect = ClipBox[tempClipCounter];
	
	}
	return true;
}
void Sprite_Extend::Draw()
{
	D3DXMATRIXA16 m;

	if (State)
	{
		//스프라이트 자체에서 알파 블랜드를 걸수 있다.
		mSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

		D3DXMatrixScaling(&m, 0.002f, 0.002f, 0.002f);
		mSprite->SetTransform(&m);

		D3DXMatrixTranslation(&m, Position.x, Position.y, Position.z);
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &m);

		if (rect.right == 0 && rect.bottom == 0)
			mSprite->Draw(m_Image, 0, NULL, NULL, Color);
		else
			mSprite->Draw(m_Image, &rect, NULL, NULL, Color);

		mSprite->End();
	}
}