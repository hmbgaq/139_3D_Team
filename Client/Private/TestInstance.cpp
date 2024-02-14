#include "stdafx.h"
#include "..\Public\TestInstance.h"

#include "GameInstance.h"
#include "VIBuffer_Model_Instance.h"

CTestInstance::CTestInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{

}

CTestInstance::CTestInstance(const CTestInstance & rhs)
	: CLandObject(rhs)
{
}

HRESULT CTestInstance::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CTestInstance::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 0.f, rand() % 20, 1.f));

	//m_pInstanceModelCom->Add_Mesh(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CTestInstance::Priority_Tick(_float fTimeDelta)
{

}

void CTestInstance::Tick(_float fTimeDelta)
{

}

void CTestInstance::Late_Tick(_float fTimeDelta)
{

// 	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());
// 
// 	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
// 	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
/*	}*/

}

HRESULT CTestInstance::Render()
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
		

		m_pShaderCom->Begin(1);
		m_pInstanceModelCom->Render(i);

		//m_pModelCom->Render(i);
	}

		
	

	return S_OK;
}

HRESULT CTestInstance::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc;

	Desc.pModel = m_pModelCom;
	
	Desc.iNumInstance = 10; // 5만개 해보니 내 컴기준 프레임 45까지 떨어짐

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pInstanceModelCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestInstance::Bind_ShaderResources()
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

CTestInstance * CTestInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestInstance*		pInstance = new CTestInstance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestInstance::Clone(void* pArg)
{
	CTestInstance*		pInstance = new CTestInstance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestInstance::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pInstanceModelCom);
}

