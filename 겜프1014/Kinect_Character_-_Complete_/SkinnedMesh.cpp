#include "SkinnedMesh.h"
#include "Utility.h"

const float kMoveTransitionTime=0.25f;


CSkinnedMesh::CSkinnedMesh()
{
	SkinFrame			= NULL;
	m_pD3DDevice		= NULL;
	m_pEffect			= NULL;       // D3DX effect interface
	m_pFrameRoot		= NULL;
	m_pAnimController	= NULL;
	m_SkinningMethod	= D3DNONINDEXED; // Current skinning method
	m_pBoneMatrices		= NULL;
	Ang = 0;
	m_currentAnimationSet = 0;
	m_numAnimationSets = 0;
	m_currentTrack = 0;
	m_currentTime = 0;
	m_speedAdjust = 1.0f;

}

CSkinnedMesh::~CSkinnedMesh()
{	
	if(m_pFrameRoot!=NULL)
		ReleaseAttributeTable( m_pFrameRoot );
	delete[] m_pBoneMatrices;
	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pFont );

	CAllocateHierarchy Alloc(this);
	D3DXFrameDestroy( m_pFrameRoot, &Alloc );
	SAFE_RELEASE( m_pAnimController );
	SAFE_RELEASE(g_pAnimController);
	SAFE_RELEASE(m_pD3DDevice);
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::MeshRender()
// Desc: 매쉬를 그린다.
//--------------------------------------------------------------------------------------
void CSkinnedMesh::MeshRender()
{
	static DWORD lastTime=timeGetTime();
	float timeElapsed=0.001f*(timeGetTime()-lastTime);

	//lastTime=timeGetTime();
	FrameMove(timeElapsed);
	DrawFrame(m_pFrameRoot);
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::LoadMesh()
// Desc: 매쉬를 로드한다.
//--------------------------------------------------------------------------------------
HRESULT CSkinnedMesh::LoadMesh(LPDIRECT3DDEVICE9 pd3dDevice, const std::string &filename)
{
	m_pD3DDevice = pd3dDevice;
	/*char m_szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,m_szCurrentDirectory);	//현재 경로를 얻어오는 함수.
	strcat(m_szCurrentDirectory,MESH_DIRECTORY);
	SetCurrentDirectory(m_szCurrentDirectory);
	char szFileName[MAX_PATH];
	strcpy(szFileName, m_szCurrentDirectory);
	strcat(szFileName, _strFileName);*/

	CAllocateHierarchy Alloc(this);

	std::string currentDirectory=CUtility::GetTheCurrentDirectory();

	std::string xfilePath;
	CUtility::SplitPath(filename,&xfilePath,&m_filename);

	SetCurrentDirectory(xfilePath.c_str());

	if(FAILED(D3DXLoadMeshHierarchyFromX(filename.c_str(), D3DXMESH_MANAGED, pd3dDevice,
		&Alloc, NULL, &m_pFrameRoot, &m_pAnimController )))
	{
		MessageBox(NULL,"스킨 매쉬 로드실패","에러",MB_ICONERROR);
		return E_FAIL;
	}

	SetupBoneMatrixPointers( m_pFrameRoot ) ;
	if(FAILED(D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius )))
	{
		MessageBox(NULL,"스킨 매쉬 바운딩스피어 로드실패","에러",MB_ICONERROR);
		return E_FAIL;
	}

	if(m_pAnimController)
		m_numAnimationSets = m_pAnimController->GetMaxNumAnimationSets();

	//MeshData.Type = D3DXMESHTYPE_MESH;

	m_pD3DDevice->AddRef();
	return S_OK;
}

void CSkinnedMesh::SetWorldMatrix(const D3DXMATRIX &Matrix)
{
	m_matWorld= Matrix;
}
void CSkinnedMesh::SetKinectMatrix(unsigned int index, const D3DXMATRIX &Matrix)
{
	static const char *strBoneName[]={
		"Bip01_Pelvis",
		"Bip01_Spine",
		"Bip01_Neck",
		"Bip01_Head",
		"Bip01_R_UpperArm",
		"Bip01_R_Forearm",
		NULL,
		"Bip01_L_Hand",
		"Bip01_L_UpperArm",
		"Bip01_L_Forearm",
		"Bip01_R_Hand",
		NULL,
		"Bip01_R_Thigh",
		"Bip01_R_Calf",
		"Bip01_R_Ankle",
		"Bip01_R_Foot",
		"Bip01_L_Thigh",
		"Bip01_L_Calf",
		"Bip01_L_Ankle",
		"Bip01_L_Foot"
	};

	//static const char *strBoneName[] = { "Bip_Pelvis", "Bip_Spine", "Bip_Neck", "Bip_Head", "Bip_R_UpperArm", "Bip_R_ForeArm", "Bip_R_Wrist", NULL,
	//	"Bip_L_UpperArm", "Bip_L_ForeArm", "Bip_L_Wrist", NULL, "Bip_R_Thigh", "Bip_R_Calf", "Bip_R_Ankle",
	//	"Bip_R_Foot", "Bip_L_Thigh", "Bip_L_Calf", "Bip_L_Ankle", "Bip_L_Foot" };
	
	if(index >= sizeof(strBoneName)/sizeof(const char*)) return;

	const char * strBone = strBoneName[index];
	if(strBone == NULL) return;

	D3DXFRAME_DERIVED *r = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, strBone );
	if (r != nullptr)
	{
		r->KinectMatrix = Matrix;
		r->bKinected = true;
	}
	else
	{
		char temp[1024];
		wsprintf(temp, "%s was not found.\r\n", strBone);
		OutputDebugStringA(temp);
	}
}

void CSkinnedMesh::BoneTranslation(TCHAR* boneName, float x, float y, float z) 
{
	D3DXMATRIXA16 boneMatrix, matTrans, matRotx, matRoty, matRotz, returnMatrix;
	D3DXFRAME_DERIVED *r = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, boneName );
	//boneMatrix = r->CombinedTransformationMatrix;
	//D3DXMatrixTranslation(&r->TransformationMatrix, x, y, z);
	//r->TransformationMatrix._41-=z;
	//r->TransformationMatrix._42-=x;
	//r->TransformationMatrix._43-=y;
	//r->TransformationMatrix._41+=x;
	//r->TransformationMatrix._42+=y;
	//r->TransformationMatrix._43+=z;

	r->TransformationMatrix._41+=x;
	r->TransformationMatrix._42+=y;
	r->TransformationMatrix._43+=z;
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::GenerateSkinnedMesh()
// Desc: 스킨 매쉬를 생성한다
//--------------------------------------------------------------------------------------
HRESULT CSkinnedMesh::GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED* pMeshContainer)
{
	HRESULT hr = S_OK;
	D3DCAPS9 d3dCaps;
	m_pD3DDevice->GetDeviceCaps( &d3dCaps );

	if( pMeshContainer->pSkinInfo == NULL )
		return hr;

	m_bUseSoftwareVP = false;

	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );

	// if non-indexed skinning mode selected, use ConvertToBlendedMesh to generate drawable mesh
	if( m_SkinningMethod == D3DNONINDEXED )
	{

		hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh
			(
			pMeshContainer->pOrigMesh,
			D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
			pMeshContainer->pAdjacency, 
			NULL, NULL, NULL, 
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups, 
			&pMeshContainer->pBoneCombinationBuf, 
			&pMeshContainer->MeshData.pMesh
			);
		if (FAILED(hr))
			goto e_Exit;


		// If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
		// Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
		// drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
		LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		// look for any set of bone combinations that do not fit the caps
		for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; pMeshContainer->iAttributeSW++)
		{
			DWORD cInfl   = 0;

			for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++)
			{
				if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
				{
					++cInfl;
				}
			}

			if (cInfl > d3dCaps.MaxVertexBlendMatrices)
			{
				break;
			}
		}

		// if there is both HW and SW, add the Software Processing flag
		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
		{
			LPD3DXMESH pMeshTmp;

			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|pMeshContainer->MeshData.pMesh->GetOptions(), 
				pMeshContainer->MeshData.pMesh->GetFVF(),
				m_pD3DDevice, &pMeshTmp);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			pMeshContainer->MeshData.pMesh->Release();
			pMeshContainer->MeshData.pMesh = pMeshTmp;
			pMeshTmp = NULL;
		}
	}
	// if indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
	else if( m_SkinningMethod == D3DINDEXED )
	{
		DWORD NumMaxFaceInfl;
		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

		LPDIRECT3DINDEXBUFFER9 pIB;
		hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
		if (FAILED(hr))
			goto e_Exit;

		hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
		pIB->Release();
		if (FAILED(hr))
			goto e_Exit;

		// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
		// can be handled
		NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

		if( d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl )
		{
			// HW does not support indexed vertex blending. Use SW instead
			pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
			pMeshContainer->UseSoftwareVP = true;
			m_bUseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}
		else
		{
			// using hardware - determine palette size from caps and number of bones
			// If normals are present in the vertex data that needs to be blended for lighting, then 
			// the number of matrices is half the number specified by MaxVertexBlendMatrixIndex.
			pMeshContainer->NumPaletteEntries = min( ( d3dCaps.MaxVertexBlendMatrixIndex + 1 ) / 2, 
				pMeshContainer->pSkinInfo->GetNumBones() );
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
			(
			pMeshContainer->pOrigMesh,
			Flags, 
			pMeshContainer->NumPaletteEntries, 
			pMeshContainer->pAdjacency, 
			NULL, NULL, NULL, 
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups, 
			&pMeshContainer->pBoneCombinationBuf, 
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;
	}
	// if vertex shader indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
	else if( ( m_SkinningMethod == D3DINDEXEDVS ) || ( m_SkinningMethod == D3DINDEXEDHLSLVS ) )
	{
		// Get palette size
		// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
		// (96 - 9) /3 i.e. Maximum constant count - used constants 
		UINT MaxMatrices = 26; 
		pMeshContainer->NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
		if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
		{
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}
		else
		{
			pMeshContainer->UseSoftwareVP = true;
			m_bUseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}

		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
			(
			pMeshContainer->pOrigMesh,
			Flags, 
			pMeshContainer->NumPaletteEntries, 
			pMeshContainer->pAdjacency, 
			NULL, NULL, NULL,             
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups, 
			&pMeshContainer->pBoneCombinationBuf, 
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;


		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, m_pD3DDevice, &pMesh);
			if (!FAILED(hr))
			{
				pMeshContainer->MeshData.pMesh->Release();
				pMeshContainer->MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}

		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
			goto e_Exit;

		// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
		//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
		//          this is more of a "cast" operation
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
				pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
			pDeclCur++;
		}

		hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if( m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
		{
			m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			delete[] m_pBoneMatrices; 
			m_pBoneMatrices  = new D3DXMATRIXA16[m_NumBoneMatricesMax];
			if( m_pBoneMatrices == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}

	}
	// if software skinning selected, use GenerateSkinnedMesh to create a mesh that can be used with UpdateSkinnedMesh
	else if( m_SkinningMethod == SOFTWARE )
	{
		hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
			m_pD3DDevice, &pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
		if (FAILED(hr))
			goto e_Exit;

		delete[] pMeshContainer->pAttributeTable;
		pMeshContainer->pAttributeTable  = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
		if (pMeshContainer->pAttributeTable == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if (m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
		{
			m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			delete[] m_pBoneMatrices; 
			m_pBoneMatrices  = new D3DXMATRIXA16[m_NumBoneMatricesMax];
			if( m_pBoneMatrices == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}
	}
	else  // invalid m_SkinningMethod value
	{        
		// return failure due to invalid skinning method value
		hr = E_INVALIDARG;
		goto e_Exit;
	}

e_Exit:
	return hr;
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::DrawMeshContainer()
// Desc: 매쉬 컨테이너를 드로우한다
//--------------------------------------------------------------------------------------
void CSkinnedMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	//HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	UINT NumBlend;
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	m_pD3DDevice->GetDeviceCaps( &d3dCaps );

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		if( m_SkinningMethod == D3DNONINDEXED )
		{
			AttribIdPrev = UNUSED32; 
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

			// Draw using default vtx processing of the device (typically HW)
			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{
				NumBlend = 0;
				for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
				{
					if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if( d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1 )
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
							m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp);
						}
					}

					m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
					{
						m_pD3DDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
						m_pD3DDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
						AttribIdPrev = pBoneComb[iAttrib].AttribId;
					}

					// draw the subset now that the correct material and matrices are loaded
					pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
				}
			}

			// If necessary, draw parts that HW could not handle using SW
			if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
			{
				AttribIdPrev = UNUSED32; 
				m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);
				for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
				{
					NumBlend = 0;
					for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
					{
						if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
						{
							NumBlend = i;
						}
					}

					if (d3dCaps.MaxVertexBlendMatrices < NumBlend + 1)
					{
						// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
						for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
						{
							iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
							if (iMatrixIndex != UINT_MAX)
							{
								D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
								m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp);
							}
						}

						m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

						// lookup the material used for this subset of faces
						if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
						{
							m_pD3DDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
							m_pD3DDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
							AttribIdPrev = pBoneComb[iAttrib].AttribId;
						}

						// draw the subset now that the correct material and matrices are loaded
						pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
					}
				}
				m_pD3DDevice->SetSoftwareVertexProcessing( FALSE);
			}

			m_pD3DDevice->SetRenderState( D3DRS_VERTEXBLEND, 0);
		}
		else if (m_SkinningMethod == D3DINDEXED)
		{
			// if hw doesn't support indexed vertex processing, switch to software vertex processing
			if (pMeshContainer->UseSoftwareVP)
			{
				// If hw or pure hw vertex processing is forced, we can't render the
				// mesh, so just exit out.  Typical applications should create
				// a device with appropriate vertex processing capability for this
				// skinning method.
				if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
					return;

				m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);
			}

			// set the number of vertex blend indices to be blended
			if (pMeshContainer->NumInfl == 1)
			{
				m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
			}
			else
			{
				m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1);
			}

			if (pMeshContainer->NumInfl)
				m_pD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

			// for each attribute group in the mesh, calculate the set of matrices in the palette and then draw the mesh subset
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
						m_pD3DDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp );
					}
				}

				// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
				m_pD3DDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
				m_pD3DDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

				// finally draw the subset with the current world matrix palette and material state
				pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
			}

			// reset blending state
			m_pD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
			m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

			// remember to reset back to hw vertex processing if software was required
			if (pMeshContainer->UseSoftwareVP)
			{
				m_pD3DDevice->SetSoftwareVertexProcessing(FALSE);
			}
		}
		else if (m_SkinningMethod == D3DINDEXEDVS) 
		{
			// Use COLOR instead of UBYTE4 since Geforce3 does not support it
			// vConst.w should be 3, but due to COLOR/UBYTE4 issue, mul by 255 and add epsilon
			D3DXVECTOR4 vConst( 1.0f, 0.0f, 0.0f, 765.01f );

			if (pMeshContainer->UseSoftwareVP)
			{
				// If hw or pure hw vertex processing is forced, we can't render the
				// mesh, so just exit out.  Typical applications should create
				// a device with appropriate vertex processing capability for this
				// skinning method.
				if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
					return;

				m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);
			}

			m_pD3DDevice->SetVertexShader( m_pIndexedVertexShader[pMeshContainer->NumInfl - 1]);

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
						D3DXMatrixMultiplyTranspose(&matTemp, &matTemp, &m_matView);
						m_pD3DDevice->SetVertexShaderConstantF(iPaletteEntry*3 + 9, (float*)&matTemp, 3);
					}
				}

				// Sum of all ambient and emissive contribution
				D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
				D3DXCOLOR color2(.25, .25, .25, 1.0);
				D3DXCOLOR ambEmm;
				D3DXColorModulate(&ambEmm, &color1, &color2);
				ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

				// set material color properties 
				m_pD3DDevice->SetVertexShaderConstantF(8, (float*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse), 1);
				m_pD3DDevice->SetVertexShaderConstantF(7, (float*)&ambEmm, 1);
				vConst.y = pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Power;
				m_pD3DDevice->SetVertexShaderConstantF(0, (float*)&vConst, 1);

				m_pD3DDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

				// finally draw the subset with the current world matrix palette and material state
				pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
			}

			// remember to reset back to hw vertex processing if software was required
			if (pMeshContainer->UseSoftwareVP)
			{
				m_pD3DDevice->SetSoftwareVertexProcessing(FALSE);
			}
			m_pD3DDevice->SetVertexShader( NULL);
		}
		else if (m_SkinningMethod == D3DINDEXEDHLSLVS) 
		{
			if (pMeshContainer->UseSoftwareVP)
			{
				// If hw or pure hw vertex processing is forced, we can't render the
				// mesh, so just exit out.  Typical applications should create
				// a device with appropriate vertex processing capability for this
				// skinning method.
				if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
					return;

				m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);
			}

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{ 
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
						D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
					}
				}
				m_pEffect->SetMatrixArray( "mWorldMatrixArray", m_pBoneMatrices, pMeshContainer->NumPaletteEntries);

				// Sum of all ambient and emissive contribution
				D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
				D3DXCOLOR color2(.25, .25, .25, 1.0);
				D3DXCOLOR ambEmm;
				D3DXColorModulate(&ambEmm, &color1, &color2);
				ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

				// set material color properties 
				m_pEffect->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
				m_pEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

				// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
				m_pD3DDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

				// Set CurNumBones to select the correct vertex shader for the number of bones
				m_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1);

				// Start the effect now all parameters have been updated
				UINT numPasses;
				m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE);
				for( UINT iPass = 0; iPass < numPasses; iPass++ )
				{
					m_pEffect->BeginPass( iPass);

					// draw the subset with the current world matrix palette and material state
					pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );

					m_pEffect->EndPass();
				}

				m_pEffect->End();

				m_pD3DDevice->SetVertexShader(NULL);
			}

			// remember to reset back to hw vertex processing if software was required
			if (pMeshContainer->UseSoftwareVP)
			{
				m_pD3DDevice->SetSoftwareVertexProcessing(FALSE);
			}
		}
		else if (m_SkinningMethod == SOFTWARE)
		{
			D3DXMATRIX  Identity;
			DWORD       cBones  = pMeshContainer->pSkinInfo->GetNumBones();
			DWORD       iBone;
			PBYTE       pbVerticesSrc;
			PBYTE       pbVerticesDest;

			// set up bone transforms
			for (iBone = 0; iBone < cBones; ++iBone)
			{
				D3DXMatrixMultiply
					(
					&m_pBoneMatrices[iBone],                 // output
					&pMeshContainer->pBoneOffsetMatrices[iBone], 
					pMeshContainer->ppBoneMatrixPtrs[iBone]
				);
			}

			// set world transform
			D3DXMatrixIdentity(&Identity);
			m_pD3DDevice->SetTransform(D3DTS_WORLD, &Identity);

			pMeshContainer->pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);
			pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);

			// generate skinned mesh
			pMeshContainer->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest);

			pMeshContainer->pOrigMesh->UnlockVertexBuffer();
			pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{
				m_pD3DDevice->SetMaterial(&(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D));
				m_pD3DDevice->SetTexture(0, pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId]);
				pMeshContainer->MeshData.pMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId);
			}
		}
		else // bug out as unsupported mode
		{
			return;
		}
	}
	else  // standard mesh, just draw it after setting material properties
	{
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			m_pD3DDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D);
			m_pD3DDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] );
			pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
		}
	}
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::DrawFrame()
// Desc: 매쉬를 화면에 뿌림.
//--------------------------------------------------------------------------------------
void CSkinnedMesh::DrawFrame(LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while (pMeshContainer != NULL)
	{
		DrawMeshContainer(pMeshContainer, pFrame );

		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		DrawFrame( pFrame->pFrameSibling);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		DrawFrame( pFrame->pFrameFirstChild );
	}
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::FrameMove()
// Desc: 프레임을 움직인다..
//--------------------------------------------------------------------------------------
void CSkinnedMesh::FrameMove(float elapsedTime)
{
	elapsedTime/=m_speedAdjust;

	if( m_pAnimController != NULL )
		m_pAnimController->AdvanceTime(elapsedTime, NULL );

	m_currentTime+=elapsedTime;

	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld, &m_matWorld);
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::DrawMeshContainer()
// Desc: 매쉬 컨테이너를 드로우한다
//--------------------------------------------------------------------------------------
float CSkinnedMesh::GetElapsedTime()
{
	static double fLastElapsedTime  = 0.0;
	double fTime;
	double fElapsedTime;

	fTime = timeGetTime()* TIME_DELTA; 

	// Return the elapsed time
	fElapsedTime = (double) (fTime - fLastElapsedTime);
	fLastElapsedTime = fTime;

	return (float) fElapsedTime;
	//return (float) 0.02f;
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::SetupBoneMatrixPointersOnMesh()
// Desc: 메시컨테이너에 매트릭스를 달아주는 함수
//--------------------------------------------------------------------------------------
HRESULT CSkinnedMesh::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase )
{
	UINT iBone, cBones;
	D3DXFRAME_DERIVED *pFrame;

	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	// if there is a skinmesh, then setup the bone matrices
	if (pMeshContainer->pSkinInfo != NULL)
	{
		cBones = pMeshContainer->pSkinInfo->GetNumBones();

		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
			return E_OUTOFMEMORY;

		for (iBone = 0; iBone < cBones; iBone++)
		{
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone) );
			if (pFrame == NULL)
				return E_FAIL;

			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::SetupBoneMatrixPointers()
// Desc: 트리 구조의 뼈를 돌면서 메시컨테이너에 최종 변환 매트릭스를 할당
//--------------------------------------------------------------------------------------
HRESULT CSkinnedMesh::SetupBoneMatrixPointers(LPD3DXFRAME pFrame )
{
	HRESULT hr;

	if (pFrame->pMeshContainer != NULL)
	{
		hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

void Multiply3x3(D3DXMATRIX *out,const D3DXMATRIX * a,const D3DXMATRIX * b)
{
	*out = *a * *b;

	out->_41 = a->_41;
	out->_42 = a->_42;
	out->_43 = a->_43;
	out->_44 = a->_44;
}
//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::DrawMeshContainer()
// Desc: 매쉬 컨테이너를 드로우한다
//--------------------------------------------------------------------------------------
void CSkinnedMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParent, LPD3DXMATRIX pKinected)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	D3DXMATRIX mCombinedTransform;
	D3DXMATRIX mKinectedTransform;
	

	if(pFrame->bKinected)
	{
		if(pParent != NULL)
		{
			mCombinedTransform = pFrame->TransformationMatrix * (*pParent);
			D3DXMATRIX mKinected = pFrame->TransformationMatrix * (*pKinected);
			mCombinedTransform._41 = mKinected._41;
			mCombinedTransform._42 = mKinected._42;
			mCombinedTransform._43 = mKinected._43;
		}
		else mCombinedTransform = pFrame->TransformationMatrix;
		Multiply3x3(&mKinectedTransform, &mCombinedTransform, &pFrame->KinectMatrix);
	}
	else
	{
		if(pParent != NULL)
		{
			mCombinedTransform = pFrame->TransformationMatrix * (*pParent);
			mKinectedTransform = pFrame->TransformationMatrix * (*pKinected);
			mCombinedTransform._41 = mKinectedTransform._41;
			mCombinedTransform._42 = mKinectedTransform._42;
			mCombinedTransform._43 = mKinectedTransform._43;
		}
		else
		{
			mKinectedTransform = mCombinedTransform = pFrame->TransformationMatrix;
		}
	}

	pFrame->CombinedTransformationMatrix = mKinectedTransform;

	if (pFrame->pFrameSibling != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameSibling, pParent, pKinected);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &mCombinedTransform, &mKinectedTransform);
	}
}

//--------------------------------------------------------------------------------------
// Name: CSkinnedMesh::ReleaseAttributeTable()
// Desc: 메모리 해제
//--------------------------------------------------------------------------------------
void CSkinnedMesh::ReleaseAttributeTable( LPD3DXFRAME pFrameBase )
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer;

	pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pFrame->pMeshContainer;

	while( pMeshContainer != NULL )
	{
		delete[] pMeshContainer->pAttributeTable;

		pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pMeshContainer->pNextMeshContainer;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		ReleaseAttributeTable(pFrame->pFrameSibling);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		ReleaseAttributeTable(pFrame->pFrameFirstChild);
	}
}

std::string CSkinnedMesh::GetAnimationSetName(unsigned int index)
{
	if (index>=m_numAnimationSets)
		return "Error: No set exists";

	// Get the animation set
	LPD3DXANIMATIONSET set;
	m_pAnimController->GetAnimationSet(m_currentAnimationSet, &set );

	std::string nameString(set->GetName());

	set->Release();

	return nameString;
}

//--------------------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Name을 받아서 pnewname에 저장 매쉬의 본에 써저있는 이름을 읽어들인다.
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::AllocateName(LPCSTR Name, LPSTR* pNewName)
{
	UINT cbLength;

	if (Name != NULL)
	{
		cbLength = (UINT)strlen(Name) + 1;
		*pNewName = new CHAR[cbLength];
		if (*pNewName == NULL)
			return E_OUTOFMEMORY;
		memcpy(*pNewName, Name, cbLength*sizeof(CHAR));

		if (cbLength > 1)
		{
			char temp[1024];
			sprintf_s(temp, "%s was founded.\r\n", Name);
			OutputDebugStringA(temp);
		}
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateFrame()
// Desc: 프레임을 만듬.. 본을 생성하기 위한 준비작업.
//--------------------------------------------------------------------------------------
//
HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
	HRESULT hr = S_OK;
	D3DXFRAME_DERIVED *pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_DERIVED;
	if (pFrame == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	hr = AllocateName(Name, &pFrame->Name);
	if (FAILED(hr))
		goto e_Exit;

	// initialize other data members of the frame
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	pFrame->pMeshContainer = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pFrameFirstChild = NULL;

	*ppNewFrame = pFrame;
	pFrame = NULL;

e_Exit:
	delete pFrame;
	return hr;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateMeshContainer()
// Desc: 메시 컨테이너를 만들어주는 함수. 매쉬를 담을 구조를 만듬.. 트리구조
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(
	LPCSTR Name, 
	CONST D3DXMESHDATA *pMeshData,
	CONST D3DXMATERIAL *pMaterials, 
	CONST D3DXEFFECTINSTANCE *pEffectInstances, 
	DWORD NumMaterials, 
	CONST DWORD *pAdjacency, 
	LPD3DXSKININFO pSkinInfo, 
	LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
	UINT NumFaces;
	UINT iMaterial;
	UINT iBone, cBones;
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;

	LPD3DXMESH pMesh = NULL;

	*ppNewMeshContainer = NULL;

	// this sample does not handle patch meshes, so fail when one is found
	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// get the pMesh interface pointer out of the mesh data structure
	pMesh = pMeshData->pMesh;

	// this sample does not FVF compatible meshes, so fail when one is found
	if (pMesh->GetFVF() == 0)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// allocate the overloaded structure to return as a D3DXMESHCONTAINER
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if (pMeshContainer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
		goto e_Exit;        

	pMesh->GetDevice(&pd3dDevice);
	NumFaces = pMesh->GetNumFaces();

	// if no normals are in the mesh, add them
	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		// clone the mesh to make room for the normals
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			pMesh->GetFVF() | D3DFVF_NORMAL, 
			pd3dDevice, &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
			goto e_Exit;

		// get the new pMesh pointer back out of the mesh container to use
		// NOTE: we do not release pMesh because we do not have a reference to it yet
		pMesh = pMeshContainer->MeshData.pMesh;

		// now generate the normals for the pmesh
		D3DXComputeNormals( pMesh, NULL );
	}
	else  // if no normals, just add a reference to the mesh for the mesh container
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}

	// allocate memory to contain the material information.  This sample uses
	//   the D3D9 materials and texture names instead of the EffectInstance style materials
	pMeshContainer->NumMaterials = max(1, NumMaterials);
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
	if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);
	memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

	// if materials provided, copy them
	if (NumMaterials > 0)            
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
		{
			if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
			{
				TCHAR strTexturePath[MAX_PATH];
				strcpy_s(strTexturePath, pMeshContainer->pMaterials[iMaterial].pTextureFilename);
				if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, strTexturePath,
					&pMeshContainer->ppTextures[iMaterial] ) ) )
					pMeshContainer->ppTextures[iMaterial] = NULL;

				// don't remember a pointer into the dynamic memory, just forget the name after loading
				pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
		}
	}
	else // if no materials provided, use a default one
	{
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// if there is skinning information, save off the required data and then setup for HW skinning
	if (pSkinInfo != NULL)
	{
		// first save off the SkinInfo and original mesh data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		// Will need an array of offset matrices to move the vertices from the figure space to the bone's space
		cBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if (pMeshContainer->pBoneOffsetMatrices == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// get each of the bone offset matrices so that we don't need to get them later
		for (iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
		}

		// GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
		hr =m_pApp->GenerateSkinnedMesh(pMeshContainer );
		if (FAILED(hr))
			goto e_Exit;
	}

	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;

e_Exit:
	SAFE_RELEASE(pd3dDevice);

	// call Destroy function to properly clean up the memory allocated 
	if (pMeshContainer != NULL)
	{
		DestroyMeshContainer(pMeshContainer);
	}

	return hr;
}


//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 프레임 종료. 릴리즈한다.
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	SAFE_DELETE_ARRAY( pFrameToFree->Name );
	SAFE_DELETE( pFrameToFree );
	return S_OK; 
}




//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 매쉬 콘테이너를 종료.. 릴리즈한다.
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	UINT iMaterial;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
	SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

	// release all the allocated textures
	if (pMeshContainer->ppTextures != NULL)
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			SAFE_RELEASE( pMeshContainer->ppTextures[iMaterial] );
		}
	}

	SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );
	SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_RELEASE( pMeshContainer->pOrigMesh );
	SAFE_DELETE( pMeshContainer );
	return S_OK;
}