#include "..\Public\VIBuffer_Particle_Rect.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{

}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect & rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(_uint iNumInstance)
{


	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXPOSTEX);
	m_iInstanceStride = sizeof(VTXINSTANCE);
	m_iIndexCountPerInstance = 6;
	m_iNumInstance = iNumInstance;

	m_pSpeeds = new _float[m_iNumInstance];
	m_pLifeTimes = new _float[m_iNumInstance];

	m_iNumIndices = 6 * iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

		m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iStride;

		ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

		VTXPOSTEX*		pVertices = new VTXPOSTEX[m_iNumVertices];
		pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
		pVertices[0].vTexcoord = _float2(0.0f, 0.f);

		pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
		pVertices[1].vTexcoord = _float2(1.0f, 0.f);

		pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
		pVertices[2].vTexcoord = _float2(1.0f, 1.0f);

		pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
		pVertices[3].vTexcoord = _float2(0.0f, 1.0f);

		m_SubResourceData.pSysMem = pVertices;

		/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_ushort*		pIndices = new _ushort[m_iNumIndices];

	_uint			iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}
	
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Particle_Rect::Initialize(void * pArg)
{
	/* 인스턴스용 버퍼를 생성한다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

//void CVIBuffer_Particle_Rect::Update(_float fTimeDelta)
//{
//
//}

CVIBuffer_Particle_Rect * CVIBuffer_Particle_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Particle_Rect*		pInstance = new CVIBuffer_Particle_Rect(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Particle_Rect::Clone(void* pArg)
{
	CVIBuffer_Particle_Rect*		pInstance = new CVIBuffer_Particle_Rect(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{
	__super::Free();

}
