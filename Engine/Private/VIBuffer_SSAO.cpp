#include "VIBuffer_SSAO.h"

CVIBuffer_SSAO::CVIBuffer_SSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_SSAO::CVIBuffer_SSAO(const CVIBuffer_SSAO& rhs)
	: CVIBuffer(rhs)
{
}

/* 	m_pContext->DrawIndexed(m_iNumIndices, 0, 0); */
HRESULT CVIBuffer_SSAO::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXNORTEX);

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;


	VTXNORTEX* pVertices = new VTXNORTEX[4];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * 4);

	pVertices[0].vPosition	= _float3(-0.5f, -0.5f, 0.f);
	pVertices[0].vNormal	= _float3(0.f, 0.f, 0.f);
	pVertices[0].vTexcoord	= _float2(0.f, 1.f);

	pVertices[1].vPosition	= _float3(-0.5f, 0.5f, 0.f);
	pVertices[1].vNormal	= _float3(1.f, 0.f, 0.f);
	pVertices[1].vTexcoord	= _float2(0.f, 0.f);

	pVertices[2].vPosition	= _float3(0.5f, 0.5f, 0.f);
	pVertices[2].vNormal	= _float3(2.f, 0.f, 0.f);
	pVertices[2].vTexcoord	= _float2(1.f, 0.f);

	pVertices[3].vPosition	= _float3(0.5f, -0.5f, 0.f);
	pVertices[3].vNormal	= _float3(3.f, 0.f, 0.f);
	pVertices[3].vTexcoord	= _float2(1.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitives = 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iIndexStride = sizeof(FACEINDICES16);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_iNumIndices = m_iNumPrimitives * m_iNumIndicesofPrimitive;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	pIndices[0]._1 = 0;
	pIndices[1]._2 = 1;
	pIndices[2]._3 = 2;

	pIndices[3]._1 = 0;
	pIndices[4]._2 = 2;
	pIndices[5]._3 = 3;

	ZeroMemory(&m_SubResourceData, sizeof(m_SubResourceData));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_SSAO::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_SSAO* CVIBuffer_SSAO::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_SSAO* pInstance = new CVIBuffer_SSAO(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_SSAO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_SSAO::Clone(void* pArg)
{
	CVIBuffer_SSAO* pInstance = new CVIBuffer_SSAO(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_SSAO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SSAO::Free()
{
	__super::Free();
}
