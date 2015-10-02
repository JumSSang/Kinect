/*******************************************************
AllocMeshHierarchy.cpp
						www.moon-labs.com���� ���� ����.
						ohyecloudy@gmail.com
						2005.11.21
*******************************************************/

#include "AllocMeshHierarchy.h"

void CopyString( const char* input, char** output)
{
	if ( input )
	{
		UINT length = (UINT)::strlen(input) + 1 ;
		// add 1 for terminating null character.
		*output = new char[length] ;
		::strcpy(*output, input) ;
	}
	else
		*output = 0 ;
}

HRESULT AllocMeshHierarchy::CreateFrame(
	THIS_ PCSTR Name, D3DXFRAME** ppNewFrame)
{
	// ppNewFrame�� ���� null�� �ƴ� ��(D3D_OK)�� return�ؾ� �Ѵ�.
	// �׷��� ������, D3DXLoadMeshHierarchyFromX()�� �߸��� ��������
	// �ؼ��Ѵ�.

	FrameEx* frameEx = new FrameEx() ;

	if ( Name )
		CopyString(Name, &frameEx->Name) ;
	else
		CopyString("<no name>", &frameEx->Name) ;

	frameEx->pMeshContainer = NULL ;
	frameEx->pFrameSibling = NULL ;
	frameEx->pFrameFirstChild = NULL ;
	D3DXMatrixIdentity(&frameEx->TransformationMatrix) ;
	D3DXMatrixIdentity(&frameEx->combinedTransform) ;

	*ppNewFrame = frameEx ;

	return D3D_OK ;
}

HRESULT AllocMeshHierarchy::CreateMeshContainer(
	PCSTR Name, const D3DXMESHDATA* pMeshData,
	const D3DXMATERIAL* pMaterials,
	const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials,
	const DWORD *pAdjacency,ID3DXSkinInfo* pSkinInfo, 
	D3DXMESHCONTAINER** ppNewMeshContainer)
{
	// mesh�� ������� �ʴ���, 
	// ppNewMeshContainer�� ���ؼ� null�� �ƴ� ��(D3D_OK)��
	// return �ؾ� �Ѵ�.
	// �׷��� ������ D3DXLoadMeshHierarchyFromX�� �߸��� �������� �ؼ�.

	D3DXMESHCONTAINER* meshContainer = new D3DXMESHCONTAINER() ;

	::ZeroMemory(meshContainer, sizeof(D3DXMESHCONTAINER)) ;
	if ( Name )
		CopyString( Name, &meshContainer->Name ) ;
	else
		CopyString( "<no name>", &meshContainer->Name ) ;

	*ppNewMeshContainer = meshContainer ;

	//--------------------------------------------------------------------
	// Skin info �� ������ �ִ� mesh�� ������ �ִµ� �װ��� ���� ��Ű��
	// ���ϸ� error�� return ���� ����, ��ſ� �� mesh container�� ����
	// �ٸ� ������ loading ���� ���� �׳� �Ѿ��.
	//--------------------------------------------------------------------
	if ( pSkinInfo == 0 || pMeshData->Type != D3DXMESHTYPE_MESH )
		return D3D_OK ;


	meshContainer->NumMaterials	= NumMaterials ;
	meshContainer->pMaterials	= new D3DXMATERIAL[NumMaterials] ;

	for ( DWORD i = 0 ; i < NumMaterials ; ++i )
	{
		D3DXMATERIAL* mtrls = meshContainer->pMaterials ;
		mtrls[i].MatD3D		= pMaterials[i].MatD3D ;
		mtrls[i].MatD3D.Ambient = pMaterials[i].MatD3D.Diffuse ;

		CopyString(
			pMaterials[i].pTextureFilename, &mtrls[i].pTextureFilename) ;
	}

    meshContainer->pEffects = 0 ;
	meshContainer->pAdjacency = 0 ;

	meshContainer->MeshData.Type	= D3DXMESHTYPE_MESH ;
	meshContainer->MeshData.pMesh	= pMeshData->pMesh ;
	meshContainer->pSkinInfo		= pSkinInfo ;

	pMeshData->pMesh->AddRef() ;
	pSkinInfo->AddRef() ;

	return D3D_OK ;
}

HRESULT AllocMeshHierarchy::DestroyFrame( THIS_ D3DXFRAME* pFrameToFree)
{
	DeleteArray(pFrameToFree->Name) ;
	Delete(pFrameToFree) ;

	return D3D_OK ;
}

HRESULT AllocMeshHierarchy::DestroyMeshContainer( THIS_ D3DXMESHCONTAINER* pMeshContainerBase)
{
	DeleteArray(pMeshContainerBase->Name) ;
	DeleteArray(pMeshContainerBase->pAdjacency) ;
	DeleteArray(pMeshContainerBase->pEffects) ;

	for ( DWORD i = 0 ; i < pMeshContainerBase->NumMaterials ; ++i )
		DeleteArray(pMeshContainerBase->pMaterials[i].pTextureFilename) ;

	DeleteArray(pMeshContainerBase->pMaterials) ;

	ReleaseCOM(pMeshContainerBase->MeshData.pMesh) ;
	ReleaseCOM(pMeshContainerBase->pSkinInfo) ;

	Delete(pMeshContainerBase) ;

	return D3D_OK ;
}
