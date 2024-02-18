#include "VIBuffer_Environment_Model_Instance.h"
#include "Mesh.h"

CVIBuffer_Environment_Model_Instance::CVIBuffer_Environment_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Model_Instance(pDevice,pContext)
{
}

CVIBuffer_Environment_Model_Instance::CVIBuffer_Environment_Model_Instance(const CVIBuffer_Environment_Model_Instance& rhs)
	: CVIBuffer_Model_Instance(rhs)
{
}

HRESULT CVIBuffer_Environment_Model_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Environment_Model_Instance::Initialize(void* pArg)
{
	ENVIRONMENT_MODEL_INSTANCE_DESC Desc = *(ENVIRONMENT_MODEL_INSTANCE_DESC*)pArg;

	m_tInstanceInfo = Desc.vecBufferInstanceInfo;

	__super::Initialize(pArg);

	

	return S_OK;
}

HRESULT CVIBuffer_Environment_Model_Instance::Bind_VIBuffers(_uint iMeshContainerIndex)
{
	
	__super::Bind_VIBuffers(iMeshContainerIndex);

	return S_OK;
}

void CVIBuffer_Environment_Model_Instance::Update(vector<INSTANCE_INFO_DESC>& vecInstanceDesc)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_matrix RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&vecInstanceDesc[i].vRotation));

		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vRight, RotationMatrix.r[0] * vecInstanceDesc[i].vScale.x);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vUp, RotationMatrix.r[1] * vecInstanceDesc[i].vScale.y);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vLook, RotationMatrix.r[2] * vecInstanceDesc[i].vScale.z);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vTranslation, XMVectorSetW(XMLoadFloat3(&vecInstanceDesc[i].vTranslation), 1.f));
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Environment_Model_Instance::Update(INSTANCE_INFO_DESC InstanceDesc, _int iIndex)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	
		_matrix RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&InstanceDesc.vRotation));

		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[iIndex].vRight, RotationMatrix.r[0] * InstanceDesc.vScale.x);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[iIndex].vUp, RotationMatrix.r[1] * InstanceDesc.vScale.y);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[iIndex].vLook, RotationMatrix.r[2] * InstanceDesc.vScale.z);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[iIndex].vTranslation, XMVectorSetW(XMLoadFloat3(&InstanceDesc.vTranslation), 1.f));
	

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Environment_Model_Instance::Bind_ShaderResources(CShader* pShader, _int iIndex)
{
	
	
	return S_OK;
}

void CVIBuffer_Environment_Model_Instance::Init_Instance(_int iNumInstance)
{

	m_iInstanceStride = sizeof(VTXMODELINSTANCE);
	m_iNumInstance = iNumInstance;
	m_iNumVertexBuffers = 2;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMODELINSTANCE* pModelInstance = new VTXMODELINSTANCE[m_iNumInstance];

	

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		//_matrix WorldMatrix = XMLoadFloat4x4(&m_vecWorldMatrix[i]);
		//
		//XMStoreFloat4(&pModelInstance[i].vRight, WorldMatrix.r[0]);
		//XMStoreFloat4(&pModelInstance[i].vUp, WorldMatrix.r[1]);
		//XMStoreFloat4(&pModelInstance[i].vLook, WorldMatrix.r[2]);
		//XMStoreFloat4(&pModelInstance[i].vTranslation, WorldMatrix.r[3]);
		_matrix RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_tInstanceInfo[i].vRotation));

		//XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vRight, RotationMatrix.r[0] * vecInstanceDesc[i].vScale.x);
		//XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vUp, RotationMatrix.r[1] * vecInstanceDesc[i].vScale.y);
		//XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vLook, RotationMatrix.r[2] * vecInstanceDesc[i].vScale.z);
		//XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vTranslation, XMVectorSetW(XMLoadFloat3(&vecInstanceDesc[i].vTranslation), 1.f));

		XMStoreFloat4(&pModelInstance[i].vRight, RotationMatrix.r[0] * m_tInstanceInfo[i].vScale.x);
		XMStoreFloat4(&pModelInstance[i].vUp, RotationMatrix.r[1] * m_tInstanceInfo[i].vScale.y);
		XMStoreFloat4(&pModelInstance[i].vLook, RotationMatrix.r[2] * m_tInstanceInfo[i].vScale.z);
		XMStoreFloat4(&pModelInstance[i].vTranslation, XMVectorSetW(XMLoadFloat3(&m_tInstanceInfo[i].vTranslation), 1.f));
		
		//pModelInstance[i].vRight			= _float4(1.f, 0.f, 0.f, 0.f);
		//pModelInstance[i].vUp			= _float4(0.f, 1.f, 0.f, 0.f);
		//pModelInstance[i].vLook			= _float4(0.f, 0.f, 1.f, 0.f);
		//pModelInstance[i].vTranslation	= _float4(0.f, 0.f, 0.f, 1.f);
		/*XMStoreFloat4(&pModelInstance[i].vTranslation, XMVectorSet(rand() % 20, 0.f, rand() % 20, 1.f));*/
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pModelInstance;

	Create_Buffer(&m_pVBInstance);

	Safe_Delete_Array(pModelInstance);

}

HRESULT CVIBuffer_Environment_Model_Instance::Render(_int iMeshIndex)
{
	

	CModel* pModel = m_tModelDesc.pModel;

	if(nullptr == pModel)
		return E_FAIL;
	//pModel->Bind_ShaderResource(pShader, pConstantName, iMeshIndex, eTextureType);
	
	Bind_VIBuffers(iMeshIndex);

	m_pContext->DrawIndexedInstanced(m_vecInstanceMesh[iMeshIndex]->Get_NumIndices(), m_iNumInstance, 0, 0, 0);

	return S_OK;
}

CVIBuffer_Environment_Model_Instance* CVIBuffer_Environment_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Environment_Model_Instance* pInstance = new CVIBuffer_Environment_Model_Instance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Environment_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Environment_Model_Instance::Clone(void* pArg)
{
	CVIBuffer_Environment_Model_Instance* pInstance = new CVIBuffer_Environment_Model_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Environment_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Environment_Model_Instance::Free()
{
	__super::Free();
	
}
