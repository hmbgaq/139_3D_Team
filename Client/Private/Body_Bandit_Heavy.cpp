#include "stdafx.h"
#include "Body_Bandit_Heavy.h"
#include "GameInstance.h"

CBody_Bandit_Heavy::CBody_Bandit_Heavy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Monster(pDevice, pContext, strPrototypeTag)
{
}

CBody_Bandit_Heavy::CBody_Bandit_Heavy(const CBody_Bandit_Heavy& rhs)
	: CBody_Monster(rhs)
{
}

HRESULT CBody_Bandit_Heavy::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Bandit_Heavy::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CBody_Bandit_Heavy::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Bandit_Heavy::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Bandit_Heavy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Bandit_Heavy::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Bandit_Heavy::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CBody_Bandit_Heavy::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_Bandit_Heavy::OnCollisionStay(CCollider* other)
{
	__super::OnCollisionStay(other);
}

void CBody_Bandit_Heavy::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}

HRESULT CBody_Bandit_Heavy::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Bandit_Heavy"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.vExtents = _float3(0.7f, 1.5f, 0.7f);
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);


	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));

	return S_OK;
}

HRESULT CBody_Bandit_Heavy::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));
	return S_OK;
}

//CBody_Bandit_Heavy* CBody_Bandit_Heavy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
//{
//	CBody_Bandit_Heavy* pInstance = new CBody_Bandit_Heavy(pDevice, pContext, strPrototypeTag);
//
//	/* 원형객체를 초기화한다.  */
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX("Failed to Created : CBody_Bandit_Heavy");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}

//CGameObject* CBody_Bandit_Heavy::Clone(void* pArg)
//{
//	CBody_Bandit_Heavy* pInstance = new CBody_Bandit_Heavy(*this);
//
//	/* 원형객체를 초기화한다.  */
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Cloned : CBody_Bandit_Heavy");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CGameObject* CBody_Bandit_Heavy::Pool()
//{
//	return new CBody_Bandit_Heavy(*this);
//}

void CBody_Bandit_Heavy::Free()
{
	__super::Free();
}
