#include "XFile.h"
#include "d3d.h"

HRESULT XFile::InitMesh(LPSTR filename)
{
	/// 재질을 임시로 보관할 버퍼선언
	LPD3DXBUFFER pD3DXMtrlBuffer;

	/// Tiger.x파일을 메시로 읽어들인다. 이때 재질정보도 함께 읽는다.
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM,
		g_pDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
		&m_pMesh)))
	{
		/// 현재 폴더에 파일이 없으면 상위폴더 검색
		if (FAILED(D3DXLoadMeshFromX("..\\filename", D3DXMESH_SYSTEMMEM,
			g_pDevice, NULL,
			&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
			&m_pMesh)))
		{
			MessageBox(NULL, "X 파일을 찾을수 없습니다.", "Meshes.exe", MB_OK);
			return E_FAIL;
		}
	}
	

	/// 재질정보와 텍스쳐 정보를 따로 뽑아낸다.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];			/// 재질개수만큼 재질구조체 배열 생성
	m_pMeshTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];	/// 재질개수만큼 텍스쳐 배열 생성

	for (DWORD i = 0; i<m_dwNumMaterials; i++)
	{
		/// 재질정보를 복사
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		/// 주변광원정보를 Diffuse정보로
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			/// 텍스쳐를 파일에서 로드한다
			if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
				d3dxMaterials[i].pTextureFilename,
				&m_pMeshTextures[i])))
			{
				/// 텍스쳐가 현재 폴더에 없으면 상위폴더 검색
				const TCHAR* strPrefix = TEXT("..\\");
				const int lenPrefix = lstrlen(strPrefix);
				TCHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix);
				if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
					strTexture,
					&m_pMeshTextures[i])))
				{
					//MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
				}
			}
		}
	}

	/// 임시로 생성한 재질버퍼 소거
	pD3DXMtrlBuffer->Release();

	return S_OK;
}
void XFile::MeshDraw()
{
	for (DWORD i = 0; i<m_dwNumMaterials; i++)
	{
		/// 부분집합 메시의 재질과 텍스쳐 설정
		g_pDevice->SetMaterial(&m_pMeshMaterials[i]);
		g_pDevice->SetTexture(0, m_pMeshTextures[i]);
		/// 부분집합 메시 출력
		m_pMesh->DrawSubset(i);
	}
}
void XFile::MeshDraw(const D3DXMATRIX &mat)
{
	g_pDevice->SetTransform(D3DTS_WORLD, &mat);
	MeshDraw();
}


AABB::AABB()
	: minPt(INFINITY, INFINITY, INFINITY),
	maxPt(-INFINITY, -INFINITY, -INFINITY){}

D3DXVECTOR3 AABB::center()
{
	return 0.5f*(minPt + maxPt);
}

VertexPN::VertexPN()
	:pos(0.0f, 0.0f, 0.0f),
	normal(0.0f, 0.0f, 0.0f){}
VertexPN::VertexPN(float x, float y, float z,
	float nx, float ny, float nz) :pos(x, y, z), normal(nx, ny, nz){}
VertexPN::VertexPN(const D3DXVECTOR3& v, const D3DXVECTOR3& n)
	:pos(v), normal(n){}