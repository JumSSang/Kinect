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
	g_pMesh = NULL;				// 매쉬 객체
	g_pMeshMaterials = NULL;	// 매쉬에 대한 재질
	g_pMeshTextures = NULL;		// 매쉬에 대한 텍스쳐
	g_dwNumMaterials = NULL;	// 매쉬 재질의 갯수
	adjBuffer = 0;				// 바운딩박스 최소값
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

	// x파일을 로딩한다.
	if( FAILED( D3DXLoadMeshFromX( m_filename.c_str(), D3DXMESH_SYSTEMMEM,
							pD3DDevice, &adjBuffer,
							&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
							&g_pMesh ) ) )
	{
		MessageBox(NULL, "x파일 로드 실패", "매쉬 로드 실패", MB_OK);
		return E_FAIL;
	}

	// 텍스쳐 파일이 다른 폴더에 있을 경우를 위하여 텍스쳐 패스 위치 얻기
	char texturePath[256];
	if (strchr(filename.c_str(), '/')==NULL) // 현재 폴더의 경우
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
		MessageBox(NULL, "x파일 바운딩 박스 로드 실패", "에러", MB_OK);
		return E_FAIL;
	}		

	g_pMesh->UnlockVertexBuffer();

	// x 파일 로딩 코드
	D3DXMATERIAL* d3dxMaterials =
					(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for( DWORD i=0; i<g_dwNumMaterials; i++)
	{
		// 재질 복사
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// 재질에 대한 앰비언트 색상 설정 (D3DX 가 해주지 않으므로)
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		// 텍스쳐 파일이 존재하는 경우
		if( d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
		{
			g_pMeshTextures[i] = LoadTexture(pD3DDevice,d3dxMaterials[i].pTextureFilename);
		}
	}

	// 재질 버퍼 사용 끝 & 해제
	pD3DXMtrlBuffer->Release();

	if( FAILED( g_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)adjBuffer->GetBufferPointer(),0,0,0)))
	{
		MessageBox(NULL, "x파일 OptimizeInplace 실패", "에러", MB_OK);
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
	MessageBox(NULL, "x파일 바운딩 스피어 로드 실패", "에러", MB_OK);
	return E_FAIL;
}

if( FAILED( D3DXComputeBoundingBox((D3DXVECTOR3*)vertex, g_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(g_pMesh->GetFVF()), &g_vMin[i], &g_vMax[i])))
{
	MessageBox(NULL, "x파일 바운딩 박스 로드 실패", "에러", MB_OK);
	return E_FAIL;
}

g_pMesh->UnlockVertexBuffer();
*/

int CXFileUtil::XFileDisplay(LPDIRECT3DDEVICE9 pD3DDevice)
{
	// 매쉬 출력
	for( DWORD i=0; i<g_dwNumMaterials; i++)
	{
		// 현재 Sub Set 에 대한 재질 설정
		pD3DDevice->SetMaterial( &g_pMeshMaterials[i] );
		// 현재 Sub Set 에 대한 텍스쳐 설정
		pD3DDevice->SetTexture( 0, g_pMeshTextures[i] );
		// 매쉬 subset을 그림
		g_pMesh->DrawSubset( i );
	}

	pD3DDevice->SetTexture( 0, NULL );

	return 0;
}