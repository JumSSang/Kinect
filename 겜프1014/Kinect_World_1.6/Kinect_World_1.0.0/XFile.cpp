#include "XFile.h"
#include "d3d.h"

HRESULT XFile::InitMesh(LPSTR filename)
{
	/// ������ �ӽ÷� ������ ���ۼ���
	LPD3DXBUFFER pD3DXMtrlBuffer;

	/// Tiger.x������ �޽÷� �о���δ�. �̶� ���������� �Բ� �д´�.
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM,
		g_pDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
		&m_pMesh)))
	{
		/// ���� ������ ������ ������ �������� �˻�
		if (FAILED(D3DXLoadMeshFromX("..\\filename", D3DXMESH_SYSTEMMEM,
			g_pDevice, NULL,
			&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials,
			&m_pMesh)))
		{
			MessageBox(NULL, "X ������ ã���� �����ϴ�.", "Meshes.exe", MB_OK);
			return E_FAIL;
		}
	}
	

	/// ���������� �ؽ��� ������ ���� �̾Ƴ���.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];			/// ����������ŭ ��������ü �迭 ����
	m_pMeshTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];	/// ����������ŭ �ؽ��� �迭 ����

	for (DWORD i = 0; i<m_dwNumMaterials; i++)
	{
		/// ���������� ����
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		/// �ֺ����������� Diffuse������
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			/// �ؽ��ĸ� ���Ͽ��� �ε��Ѵ�
			if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
				d3dxMaterials[i].pTextureFilename,
				&m_pMeshTextures[i])))
			{
				/// �ؽ��İ� ���� ������ ������ �������� �˻�
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

	/// �ӽ÷� ������ �������� �Ұ�
	pD3DXMtrlBuffer->Release();

	return S_OK;
}
void XFile::MeshDraw()
{
	for (DWORD i = 0; i<m_dwNumMaterials; i++)
	{
		/// �κ����� �޽��� ������ �ؽ��� ����
		g_pDevice->SetMaterial(&m_pMeshMaterials[i]);
		g_pDevice->SetTexture(0, m_pMeshTextures[i]);
		/// �κ����� �޽� ���
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