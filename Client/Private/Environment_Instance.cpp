#include "stdafx.h"
#include "Environment_Instance.h"

#include "GameInstance.h"
#include "VIBuffer_Environment_Model_Instance.h"

CEnvironment_Instance::CEnvironment_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CEnvironment_Instance::CEnvironment_Instance(const CEnvironment_Instance & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_Instance::Initialize_Prototype()
{	
	
	return S_OK;
}

HRESULT CEnvironment_Instance::Initialize(void* pArg)
{	
	m_tInstanceDesc = {};//*(MAPTOOL_INSTANCE_DESC*)pArg;

	
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CEnvironment_Instance::Priority_Tick(_float fTimeDelta)
{

}

void CEnvironment_Instance::Tick(_float fTimeDelta)
{

}

void CEnvironment_Instance::Late_Tick(_float fTimeDelta)
{

// 	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());
// 
// 	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
// 	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
/*	}*/

}

HRESULT CEnvironment_Instance::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		//m_pInstanceModelCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		//m_pInstanceModelCom->Bind_ShaderResources(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		

		m_pShaderCom->Begin(m_tInstanceDesc.iShaderPassIndex);
		m_pInstanceModelCom->Render((_uint)i);

		//m_pModelCom->Render(i);
	}

		
	

	return S_OK;
}

HRESULT CEnvironment_Instance::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Shader_Model_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), m_tInstanceDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	CVIBuffer_Environment_Model_Instance::ENVIRONMENT_MODEL_INSTANCE_DESC Desc;
	
	Desc.pModel = m_pModelCom;
	Desc.iNumInstance = m_tInstanceDesc.iNumInstance; // 5만개 해보니 내 컴기준 프레임 45까지 떨어짐
	Desc.vecBufferInstanceInfo = m_tInstanceDesc.vecInstanceInfoDesc;
	
	// For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pInstanceModelCom), &Desc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEnvironment_Instance::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CEnvironment_Instance * CEnvironment_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnvironment_Instance*		pInstance = new CEnvironment_Instance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnvironment_Instance::Clone(void* pArg)
{
	CEnvironment_Instance*		pInstance = new CEnvironment_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnvironment_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pInstanceModelCom);
}

