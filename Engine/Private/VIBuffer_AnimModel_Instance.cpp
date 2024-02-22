#include "VIBuffer_AnimModel_Instance.h"
#include "Model.h"
#include "MyAIScene.h"
#include "Shader.h"
#include "Mesh.h"

CVIBuffer_AnimModel_Instance::CVIBuffer_AnimModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_AnimModel_Instance::CVIBuffer_AnimModel_Instance(const CVIBuffer_AnimModel_Instance& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_AnimModel_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_AnimModel_Instance::Initialize(void* pArg)
{
	m_tModelDesc = *(ANIMINSTANCE_BUFFER_DESC*)pArg;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_tModelDesc.iMaxInstanceCount * m_tModelDesc.iSizePerSecond;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_tModelDesc.iSizePerSecond;

	D3D11_SUBRESOURCE_DATA		InitialData;

	InitialData.pSysMem = m_tModelDesc.pByte;

	FAILED_CHECK(m_pDevice->CreateBuffer(&m_BufferDesc, &InitialData, &m_pVB));

	//FAILED_CHECK(__super::Create_Buffer(&m_pVB));

	return S_OK;
}

HRESULT CVIBuffer_AnimModel_Instance::Bind_VIBuffers(CMesh* pMesh)
{
	if (nullptr == pMesh)
		return E_FAIL;


	_uint iInstanceStride = sizeof(_uint) + sizeof(_float4x4);

	ID3D11Buffer* pVertexBuffers[] = {
		pMesh->Get_VertexBuffer(),
		m_pVB,
	};

	_uint				iStrides[] = {
		pMesh->Get_Stride(),
		iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};

	/* 어떤 버텍스 버퍼들을 이용할거다. */
	m_pContext->IASetVertexBuffers(0, 2, pVertexBuffers, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	m_pContext->IASetIndexBuffer(pMesh->Get_IndexBuffer(), pMesh->Get_IndexFormat(), 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	m_pContext->IASetPrimitiveTopology(pMesh->Get_Topology());

	return S_OK;

}

HRESULT CVIBuffer_AnimModel_Instance::Render(CMesh* pMesh, _uint iSize)
{
	if(nullptr == pMesh || iSize <= 0)
		return E_FAIL;

	Bind_VIBuffers(pMesh);
	 
	m_pContext->DrawIndexedInstanced(pMesh->Get_NumIndices(), iSize, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_AnimModel_Instance::Update(_float fTimeDelta)
{

}




CVIBuffer_AnimModel_Instance* CVIBuffer_AnimModel_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_AnimModel_Instance* pInstance = new CVIBuffer_AnimModel_Instance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_AnimModel_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_AnimModel_Instance::Clone(void* pArg)
{
	CVIBuffer_AnimModel_Instance* pInstance = new CVIBuffer_AnimModel_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_AnimModel_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_AnimModel_Instance::Free()
{
	__super::Free();


	if (m_isCloned == true)
	{
		//Safe_Delete(m_tModelDesc.pValue);

		//Safe_Delete(m_tModelDesc);

	}

}
