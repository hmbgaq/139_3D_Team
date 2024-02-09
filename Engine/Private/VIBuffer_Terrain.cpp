#include "..\Public\VIBuffer_Terrain.h"
#include "GameInstance.h"

#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER		fh;
	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);

	BITMAPINFOHEADER		ih;
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);
	
	_ulong*		pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iStride = sizeof(VTXNORTEX);

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;		

	/*		11111111 10010011 10010011 10010011

		&	00000000 00000000 00000000 11111111

			00000000 00000000 00000000 10010011*/		

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, (pPixel[iIndex] & 0x000000ff) / 5.f, i);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.0f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));			
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_uint*		pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1, 
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Cross(vSourDir, vDestDir);

			vNormal = XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(vNormal));

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Cross(vSourDir, vDestDir);

			vNormal = XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal;
 			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(vNormal));
		}
	}	

#pragma endregion
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	m_pQuadTree->Make_Neighbors();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Height(_fvector vLocalPos)
{
	/* 250, 50 */
	// _uint		iIndex = _uint(pt.y / tilesizey) * tilecntx + _uint(pt.x / tilesizex);

	/* 어떤 네모안에 있어!!! */
	_uint		iIndex = (_uint)XMVectorGetZ(vLocalPos) * m_iNumVerticesX + (_uint)XMVectorGetX(vLocalPos);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = XMVectorGetX(vLocalPos) - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - XMVectorGetZ(vLocalPos);

	_vector		vPlane;

	/* 평면방정식의 해를 구한다. */
	/* a, b, c, d */
	/* x, y, z */

	/* ax + by + cz + d = 0 */	
	/* 오른쪽 위 삼각형에 있다. */
	if (fWidth >= fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), 
			XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
	}

	/* 왼쪽 아래 삼각형에 있다. */
	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[3]]));
	}

	/* ax + by + cz + d = 0 */

	/* y = (-ax - cz - d) / b; */

	return (-vPlane.m128_f32[0] * vLocalPos.m128_f32[0] - vPlane.m128_f32[2] * vLocalPos.m128_f32[2] - vPlane.m128_f32[3]) / vPlane.m128_f32[1];	
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(WorldMatrix);

	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	_uint		iNumIndices = 0;	

	m_pQuadTree->Culling(m_pVerticesPos, (_uint*)MappedSubResource.pData, &iNumIndices);

	/*for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool		isIn[4] = {
				m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), 0.f), 
				m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), 0.f),
				m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), 0.f),
				m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]), 0.f),
			};

			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				((_uint*)MappedSubResource.pData)[iNumIndices++] = iIndices[0];
				((_uint*)MappedSubResource.pData)[iNumIndices++] = iIndices[1];
				((_uint*)MappedSubResource.pData)[iNumIndices++] = iIndices[2];				
			}

			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				((_uint*)MappedSubResource.pData)[iNumIndices++] = iIndices[0];
				((_uint*)MappedSubResource.pData)[iNumIndices++] = iIndices[2];
				((_uint*)MappedSubResource.pData)[iNumIndices++] = iIndices[3];
			}
		}
	}*/

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strHeightMapFilePath)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);
}
