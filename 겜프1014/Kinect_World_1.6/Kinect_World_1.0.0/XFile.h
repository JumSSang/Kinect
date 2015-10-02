

#ifndef _XFILE_H_
#define _XFILE_H_



#include <d3dx9.h>
#include <string>

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);
struct Mtrl
{
	Mtrl()
	:ambient(WHITE), diffuse(WHITE), spec(WHITE), specPower(8.0f){}
	Mtrl(const D3DXCOLOR& a, const D3DXCOLOR& d,
		const D3DXCOLOR& s, float power)
		:ambient(a), diffuse(d), spec(s), specPower(power){}

	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	float specPower;
};

struct AABB
{
	// Initialize to an infinitely small bounding box.
	AABB();

	D3DXVECTOR3 center();

	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;
};
struct VertexPN
{
	VertexPN();
	VertexPN(float x, float y, float z, float nx, float ny, float nz);
	VertexPN(const D3DXVECTOR3& v, const D3DXVECTOR3& n);

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	static IDirect3DVertexDeclaration9* Decl;
};

struct VertexPNT
{
	VertexPNT()
	:pos(0.0f, 0.0f, 0.0f),
	normal(0.0f, 0.0f, 0.0f),
	tex0(0.0f, 0.0f){}
	VertexPNT(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v) :pos(x, y, z), normal(nx, ny, nz), tex0(u, v){}
	VertexPNT(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const D3DXVECTOR2& uv)
		:pos(v), normal(n), tex0(uv){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex0;

	static IDirect3DVertexDeclaration9* Decl;
};


class XFile
{
public:

	LPD3DXMESH              m_pMesh = NULL; // 메시 객체
	D3DMATERIAL9*           m_pMeshMaterials = NULL; // 메시에서 사용할 재질
	LPDIRECT3DTEXTURE9*     m_pMeshTextures = NULL; // 메시에서 사용할 텍스쳐
	DWORD                   m_dwNumMaterials = 0L;   // 메시에서 사용중인 재질의 개수
	ID3DXMesh*  mBox;
	D3DXMATRIX  mBoundingBoxOffset;
	Mtrl        mBoxMtrl;


public:

	HRESULT InitMesh(LPSTR filename);
	void MeshDraw();
	void MeshDraw(const D3DXMATRIX &mat);



}; 

#endif //