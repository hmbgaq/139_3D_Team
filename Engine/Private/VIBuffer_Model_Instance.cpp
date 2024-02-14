#include "VIBuffer_Model_Instance.h"
#include "Model.h"
#include "MyAIScene.h"
#include "Shader.h"
#include "Mesh.h"

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Model_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize(void* pArg)
{
	m_tModelDesc = *(MODEL_INSTANCE_DESC*)pArg;

	Safe_AddRef(m_tModelDesc.pModel);


	CModel* pModel = m_tModelDesc.pModel;

	vector<CMesh*> Meshes = pModel->Get_Meshes();
	m_iNumMeshes = Meshes.size();
	
	for (_int i = 0; i < m_iNumMeshes; ++i)
	{
		m_vecInstanceMesh.push_back(Meshes[i]);
		Safe_AddRef(Meshes[i]);
	}
	

	m_iNumMaterials = pModel->Get_NumMaterials();

	Init_Instance(m_tModelDesc.iNumInstance);

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Bind_VIBuffers(_uint iMeshContainerIndex)
{
	
	CMesh* pMesh = m_vecInstanceMesh[iMeshContainerIndex];

	ID3D11Buffer* pVertexBuffers[] = {
		pMesh->Get_VertexBuffer(),
		m_pVBInstance,
	};

	_uint				iStrides[] = {
		pMesh->Get_Stride(),
		m_iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};

	/* 어떤 버텍스 버퍼들을 이용할거다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	m_pContext->IASetIndexBuffer(pMesh->Get_IndexBuffer(), pMesh->Get_IndexFormat(), 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	m_pContext->IASetPrimitiveTopology(pMesh->Get_Topology());

	return S_OK;
}

void CVIBuffer_Model_Instance::Update(_float fTimeDelta)
{
	
}

void CVIBuffer_Model_Instance::Update_MeshInstance()
{
	if (0 == m_iNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	_matrix							WorldMatrix;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_int i = 0; i < m_iNumInstance; ++i)
	{

		WorldMatrix = m_vecWorldMatrix[i];


		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vRight, WorldMatrix.r[0]);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vUp, WorldMatrix.r[1]);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vLook, WorldMatrix.r[2]);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vTranslation, WorldMatrix.r[3]);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

// void CVIBuffer_Model_Instance::Add_Mesh(_fmatrix vWorldMatrix)
// {
// 	m_iNumInstance++;
// 
// 	//Init_Instance();
// 
// 	_float4x4 WorldMatrix;
// 	XMStoreFloat4x4(&WorldMatrix, vWorldMatrix);
// 	m_vecWorldMatrix.push_back(WorldMatrix);
// }

void CVIBuffer_Model_Instance::Init_Model()
{
// 	D3D11_MAPPED_SUBRESOURCE		SubResource;
// 	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
// 	
// 	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
// 
// 	for (_int i = 0; i < m_iNumInstance; ++i)
// 	{	
// 		_matrix WorldMatrix = XMLoadFloat4x4(&m_vecWorldMatrix[i]);
// 
// 		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vRight,	   WorldMatrix.r[0]);
// 		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vUp,		   WorldMatrix.r[1]);
// 		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vLook,		   WorldMatrix.r[2]);
// 		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vTranslation, WorldMatrix.r[3]);
// 	}
// 
// 	m_pContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Model_Instance::Init_Instance(_int iNumInstance)
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
		pModelInstance[i].vRight		= _float4(1.f, 0.f, 0.f, 0.f);
		pModelInstance[i].vUp			= _float4(0.f, 1.f, 0.f, 0.f);
		pModelInstance[i].vLook			= _float4(0.f, 0.f, 1.f, 0.f);
		//pModelInstance[i].vTranslation	= _float4(0.f, 0.f, 0.f, 1.f);
		XMStoreFloat4(&pModelInstance[i].vTranslation, XMVectorSet(rand() % 20, 0.f, rand() % 20, 1.f));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pModelInstance;

	Create_Buffer(&m_pVBInstance);

	Safe_Delete_Array(pModelInstance);

}





HRESULT CVIBuffer_Model_Instance::Render(_int iMeshIndex)
{
	

	CModel* pModel = m_tModelDesc.pModel;

	if(nullptr == pModel)
		return E_FAIL;
	//pModel->Bind_ShaderResource(pShader, pConstantName, iMeshIndex, eTextureType);
	
	Bind_VIBuffers(iMeshIndex);

	m_pContext->DrawIndexedInstanced(m_vecInstanceMesh[iMeshIndex]->Get_NumIndices(), m_iNumInstance, 0, 0, 0);

	return S_OK;
}

CVIBuffer_Model_Instance* CVIBuffer_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Model_Instance::Clone(void* pArg)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Model_Instance::Free()
{
	__super::Free();
	

		
	if (nullptr != m_tModelDesc.pModel)
	{
		for (_int i = 0; i < m_iNumMeshes; ++i)
		{
			Safe_Release(m_vecInstanceMesh[i]);
		}

		m_vecInstanceMesh.clear();
		Safe_Release(m_tModelDesc.pModel);

	}

	Safe_Release(m_pVBInstance);

	m_vecWorldMatrix.clear();
}
