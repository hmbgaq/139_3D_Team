#include "VIBuffer_SSAO.h"

CVIBuffer_SSAO::CVIBuffer_SSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_SSAO::CVIBuffer_SSAO(const CVIBuffer_SSAO& rhs)
	: CVIBuffer(rhs)

{

}

HRESULT CVIBuffer_SSAO::Initialize_Prototype()
{
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXNORTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

#pragma region DF
	VTXNORTEX* pVertices = new VTXNORTEX[4];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * 4);

	pVertices[0].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[0].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 1.f);

	//pVertices[1].vPosition = Vec3(0.5f, 0.5f, 0.f);
	pVertices[1].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[1].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[1].vTexcoord = _float2(0.f, 0.f);

	//pVertices[2].vPosition = Vec3(0.5f, -0.5f, 0.f);
	pVertices[2].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[2].vNormal = _float3(2.f, 0.f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 0.f);

	//pVertices[3].vPosition = Vec3(-0.5f, -0.5f, 0.f);
	pVertices[3].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[3].vNormal = _float3(3.f, 0.f, 0.f);
	pVertices[3].vTexcoord = _float2(1.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	FAILED_CHECK(__super::Create_Buffer(&m_pVB));

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumVertices = 2;
	m_iStride = sizeof(FACEINDICES16);
	//m_iNumIndicesofPrimitive = 3;
	m_iNumIndices = m_iNumVertices * 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumVertices];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumVertices);

	pIndices[0]._1 = 0;
	pIndices[0]._2 = 1;
	pIndices[0]._3 = 2;

	pIndices[1]._1 = 0;
	pIndices[1]._2 = 2;
	pIndices[1]._3 = 3;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	FAILED_CHECK(__super::Create_Buffer(&m_pIB));

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
