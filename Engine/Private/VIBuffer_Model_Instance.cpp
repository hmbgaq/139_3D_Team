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
	m_iNumMeshes = (_int)Meshes.size();
	
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
	if (nullptr != m_vecInstanceMesh[iMeshContainerIndex])
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
	}


	return S_OK;
}

void CVIBuffer_Model_Instance::Update(_float fTimeDelta)
{
	
}

void CVIBuffer_Model_Instance::Init_Instance(_int iNumInstance)
{
	

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
