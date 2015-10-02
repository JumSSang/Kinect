#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

class Sprite
{
public:
	LPDIRECT3DDEVICE9 m_pd3dDevice; //����̽� �����͸� ������ �ִ´�.
	ID3DXSprite* mSprite; // �ش� ��������Ʈ ���¸� ���ϰ� �ִ´�.
	IDirect3DTexture9* m_Image; // �ش� �̹��������� ������ �ϳ��� �ؽ��ĸ� ������ �־���Ѵ�.

	RECT rect; //�� ��ȿ �׸� �׷��� ������ �����϶�� �ǰ�����.
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Center;
	bool State;
	D3DCOLOR Color;

	Sprite();
	bool LoadFile(LPDIRECT3DDEVICE9 d3dDevice, D3DXVECTOR3 _Position, D3DXVECTOR3 _Center, char* filename);
	bool LoadFile(LPDIRECT3DDEVICE9 d3dDevice, RECT _rect, D3DXVECTOR3 _Position, D3DXVECTOR3 _Center, char* filename);
	void UpdatePosition(D3DXVECTOR3 _Position){ Position = _Position; }
	void UpdateCenter(D3DXVECTOR3 _Center){ Center = _Center; }
	void UpdateRect(RECT _rect){ rect = _rect; }
	void UpdateColor(D3DCOLOR _Color){ Color = _Color; }
	void Draw();
	bool ClickCheck(LONG PosX, LONG PosY);
};

class Sprite_Extend : public Sprite
{
	unsigned int FPS;
	unsigned int MaxClipCounter;
	bool Updating;
	float timeAccum;

public:
	unsigned int tempClipCounter;
	Sprite_Extend();

	bool
		mation(float timeDelta, unsigned int FPS, RECT* ClipBox, unsigned int MaxClipCounter);

	bool UpdateAnimationOne(float timeDelta, unsigned int FPS, RECT* ClipBox, unsigned int MaxClipCounter);
	void Draw();
};
