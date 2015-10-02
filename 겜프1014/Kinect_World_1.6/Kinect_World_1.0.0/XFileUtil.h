// XFileUtil.h: interface for the CXFileUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XFILEUTIL_H__72760B94_1A71_4559_8228_376F7FB21B99__INCLUDED_)
#define AFX_XFILEUTIL_H__72760B94_1A71_4559_8228_376F7FB21B99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include <string>

class CXFileUtil
{
public:
	// x 파일 로드용 변수
	LPD3DXMESH					g_pMesh;			// 매쉬 객체
	D3DMATERIAL9*				g_pMeshMaterials;	// 매쉬에 대한 재질
	LPDIRECT3DTEXTURE9*			g_pMeshTextures;	// 매쉬에 대한 텍스쳐
	DWORD						g_dwNumMaterials;	// 재질의 수
	ID3DXBuffer*				adjBuffer;
	D3DXVECTOR3					g_vObjectCenter;    // 바운딩스피어의 중심값 계산
	FLOAT						g_fObjectRadius;    // 바운딩스피어의 반경 계산
	D3DXVECTOR3					g_vMax;				// 바운딩박스 최대값	
	D3DXVECTOR3					g_vMin;				// 바운딩박스 최소값

public:
	int XFileDisplay(LPDIRECT3DDEVICE9 pD3DDevice);
	int XFileLoad(LPDIRECT3DDEVICE9 pD3DDevice, const std::string &filename);
	CXFileUtil();
	virtual ~CXFileUtil();
	std::string m_filename;

};

#endif // !defined(AFX_XFILEUTIL_H__72760B94_1A71_4559_8228_376F7FB21B99__INCLUDED_)
