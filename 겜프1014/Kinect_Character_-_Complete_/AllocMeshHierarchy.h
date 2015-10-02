/*******************************************************
AllocMeshHierarchy.h
						www.moon-labs.comø°º≠ ∞≈¿« ª©≥¶.
						ohyecloudy@gmail.com
						2005.11.21
*******************************************************/
#pragma once

#include <d3dx9.h>

#define Delete(x)     { if(x){ delete   x;  x = 0; } }
#define DeleteArray(x){ if(x){ delete[] x;  x = 0; } }
#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

struct FrameEx : public D3DXFRAME
{
	D3DXMATRIX combinedTransform ;
};


class AllocMeshHierarchy : public ID3DXAllocateHierarchy
{
public:
	HRESULT STDMETHODCALLTYPE CreateFrame(
		THIS_ PCSTR Name,
		D3DXFRAME** ppNewFrame) ;

	HRESULT STDMETHODCALLTYPE CreateMeshContainer(
		PCSTR Name, 
		const D3DXMESHDATA* pMeshData, 
		const D3DXMATERIAL* pMaterials, 
		const D3DXEFFECTINSTANCE* pEffectInstances, 
		DWORD NumMaterials, 
		const DWORD *pAdjacency, 
		ID3DXSkinInfo* pSkinInfo, 
		D3DXMESHCONTAINER** ppNewMeshContainer) ;

	HRESULT STDMETHODCALLTYPE DestroyFrame(
		THIS_ D3DXFRAME* pFrameToFree) ;

	HRESULT STDMETHODCALLTYPE DestroyMeshContainer(
		THIS_ D3DXMESHCONTAINER* pMeshContainerBase) ;
};
