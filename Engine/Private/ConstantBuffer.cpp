#include "ConstantBuffer.h"

CConstantBuffer::CConstantBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CConstantBuffer::Initialize(const _uint& iSize)
{
	m_iSize = iSize;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU_Write + GPU_Read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = m_iSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pConstantBuffer)))
		return E_FAIL;

	return S_OK;
}

void CConstantBuffer::CopyData(const void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, data, m_iSize);
	m_pContext->Unmap(m_pConstantBuffer, 0);
}

CConstantBuffer* CConstantBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint& iSize)
{
	CConstantBuffer* pInstance = new CConstantBuffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSize)))
	{
		MSG_BOX("Failed to Created : CConstantBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConstantBuffer::Free()
{
	Super::Free();
	Safe_Release(m_pConstantBuffer);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
