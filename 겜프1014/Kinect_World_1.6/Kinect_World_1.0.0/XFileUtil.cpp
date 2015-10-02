// XFileUtil.cpp: implementation of the CXFileUtil class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "XFileUtil.h"
#include "Utility.h"
#include "d3dUtility.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CXFileUtil::CXFileUtil()
{
	g_pMesh = NULL;				// �Ž� ��ü
	g_pMeshMaterials = NULL;	// �Ž��� ���� ����
	g_pMeshTextures = NULL;		// �Ž��� ���� �ؽ���
	g_dwNumMaterials = NULL;	// �Ž� ������ ����
	adjBuffer = 0;				// �ٿ���ڽ� �ּҰ�
}

CXFileUtil::~CXFileUtil()
{

}

int CXFileUtil::XFileLoad(LPDIRECT3DDEVICE9 pD3DDevice, const std::string &filename)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

	std::string currentDirectory=CUtility::GetTheCurrentDirectory();

	std::string xfilePath;
	CUtility::SplitPath(filename,&xfilePath,&m_filename);

	SetCurrentDirectory(xfilePath.c_str());

	// x������ �ε��Ѵ�.
	if( FAILED( D3DXLoadMeshFromX( m_filename.c_str(), D3DXMESH_SYSTEMMEM,
							pD3DDevice, &adjBuffer,
							&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
							&g_pMesh ) ) )
	{
		MessageBox(NULL, "x���� �ε� ����", "�Ž� �ε� ����", MB_OK);
		return E_FAIL;
	}

	// �ؽ��� ������ �ٸ� ������ ���� ��츦 ���Ͽ� �ؽ��� �н� ��ġ ���
	char texturePath[256];
	if (strchr(filename.c_str(), '/')==NULL) // ���� ������ ���
		wsprintf(texturePath, "./");
	else
	{
		char temp[256], *pChar;
		strcpy_s(temp, filename.c_str());
		_strrev(temp);
		pChar = strchr(temp, '/');
		strcpy_s(texturePath, pChar);
		_strrev(texturePath);
	}

	BYTE* vertex = 0;
	g_pMesh->LockVertexBuffer(0, (void**)&vertex);

	if( FAILED( D3DXComputeBoundingBox((D3DXVECTOR3*)vertex, g_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(g_pMesh->GetFVF()), &g_vMin, &g_vMax)))
	{
		MessageBox(NULL, "x���� �ٿ�� �ڽ� �ε� ����", "����", MB_OK);
		return E_FAIL;
	}		

	g_pMesh->UnlockVertexBuffer();

	// x ���� �ε� �ڵ�
	D3DXMATERIAL* d3dxMaterials =
					(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for( DWORD i=0; i<g_dwNumMaterials; i++)
	{
		// ���� ����
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// ������ ���� �ں��Ʈ ���� ���� (D3DX �� ������ �����Ƿ�)
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		// �ؽ��� ������ �����ϴ� ���
		if( d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
		{
			g_pMeshTextures[i] = LoadTexture(pD3DDevice,d3dxMaterials[i].pTextureFilename);
		}
	}

	// ���� ���� ��� �� & ����
	pD3DXMtrlBuffer->Release();

	if( FAILED( g_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)adjBuffer->GetBufferPointer(),0,0,0)))
	{
		MessageBox(NULL, "x���� OptimizeInplace ����", "����", MB_OK);
		return E_FAIL;
	}

	adjBuffer->Release();

	SetCurrentDirectory(currentDirectory.c_str());

	return S_OK;
}

/*
BYTE* vertex = 0;
g_pMesh->LockVertexBuffer(0, (void**)&vertex);

if( FAILED( D3DXComputeBoundingSphere((D3DXVECTOR3*)vertex, g_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(g_pMesh->GetFVF()), &g_vObjectCenter[i], &g_fObjectRadius[i])))
{
	MessageBox(NULL, "x���� �ٿ�� ���Ǿ� �ε� ����", "����", MB_OK);
	return E_FAIL;
}

if( FAILED( D3DXComputeBoundingBox((D3DXVECTOR3*)vertex, g_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(g_pMesh->GetFVF()), &g_vMin[i], &g_vMax[i])))
{
	MessageBox(NULL, "x���� �ٿ�� �ڽ� �ε� ����", "����", MB_OK);
	return E_FAIL;
}

g_pMesh->UnlockVertexBuffer();
*/

int CXFileUtil::XFileDisplay(LPDIRECT3DDEVICE9 pD3DDevice)
{
	// �Ž� ���
	for( DWORD i=0; i<g_dwNumMaterials; i++)
	{
		// ���� Sub Set �� ���� ���� ����
		pD3DDevice->SetMaterial( &g_pMeshMaterials[i] );
		// ���� Sub Set �� ���� �ؽ��� ����
		pD3DDevice->SetTexture( 0, g_pMeshTextures[i] );
		// �Ž� subset�� �׸�
		g_pMesh->DrawSubset( i );
	}

	pD3DDevice->SetTexture( 0, NULL );

	return 0;
}