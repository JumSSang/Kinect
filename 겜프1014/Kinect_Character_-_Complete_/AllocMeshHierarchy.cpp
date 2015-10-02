/*******************************************************
AllocMeshHierarchy.cpp
						www.moon-labs.com에서 거의 빼낌.
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
	// ppNewFrame를 위해 null이 아닌 값(D3D_OK)을 return해야 한다.
	// 그렇지 않으면, D3DXLoadMeshHierarchyFromX()는 잘못된 동작으로
	// 해석한다.

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
	// mesh를 사용하지 않더라도, 
	// ppNewMeshContainer를 위해서 null이 아닌 값(D3D_OK)을
	// return 해야 한다.
	// 그렇지 않으면 D3DXLoadMeshHierarchyFromX는 잘못된 동작으로 해석.

	D3DXMESHCONTAINER* meshContainer = new D3DXMESHCONTAINER() ;

	::ZeroMemory(meshContainer, sizeof(D3DXMESHCONTAINER)) ;
	if ( Name )
		CopyString( Name, &meshContainer->Name ) ;
	else
		CopyString( "<no name>", &meshContainer->Name ) ;

	*ppNewMeshContainer = meshContainer ;

	//--------------------------------------------------------------------
	// Skin info 를 가지고 있는 mesh에 관심이 있는데 그것을 만족 시키지
	// 못하면 error를 return 하지 말고, 대신에 이 mesh container에 대해
	// 다른 정보를 loading 하지 말고 그냥 넘어간다.
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
