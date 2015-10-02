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
	// x ���� �ε�� ����
	LPD3DXMESH					g_pMesh;			// �Ž� ��ü
	D3DMATERIAL9*				g_pMeshMaterials;	// �Ž��� ���� ����
	LPDIRECT3DTEXTURE9*			g_pMeshTextures;	// �Ž��� ���� �ؽ���
	DWORD						g_dwNumMaterials;	// ������ ��
	ID3DXBuffer*				adjBuffer;
	D3DXVECTOR3					g_vObjectCenter;    // �ٿ�����Ǿ��� �߽ɰ� ���
	FLOAT						g_fObjectRadius;    // �ٿ�����Ǿ��� �ݰ� ���
	D3DXVECTOR3					g_vMax;				// �ٿ���ڽ� �ִ밪	
	D3DXVECTOR3					g_vMin;				// �ٿ���ڽ� �ּҰ�

public:
	int XFileDisplay(LPDIRECT3DDEVICE9 pD3DDevice);
	int XFileLoad(LPDIRECT3DDEVICE9 pD3DDevice, const std::string &filename);
	CXFileUtil();
	virtual ~CXFileUtil();
	std::string m_filename;

};

#endif // !defined(AFX_XFILEUTIL_H__72760B94_1A71_4559_8228_376F7FB21B99__INCLUDED_)
