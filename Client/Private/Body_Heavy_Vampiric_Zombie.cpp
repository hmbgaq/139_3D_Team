#include "stdafx.h"
#include "Body_Heavy_Vampiric_Zombie.h"
#include "GameInstance.h"
#include "..\Public\Body_Heavy_Vampiric_Zombie.h"

CBody_Heavy_Vampiric_Zombie::CBody_Heavy_Vampiric_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Bandit_Heavy(pDevice, pContext, strPrototypeTag)
{
}

CBody_Heavy_Vampiric_Zombie::CBody_Heavy_Vampiric_Zombie(const CBody_Heavy_Vampiric_Zombie& rhs)
	: CBody_Bandit_Heavy(rhs)
{
}

HRESULT CBody_Heavy_Vampiric_Zombie::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Heavy_Vampiric_Zombie::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CBody_Heavy_Vampiric_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Heavy_Vampiric_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Heavy_Vampiric_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Heavy_Vampiric_Zombie::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Heavy_Vampiric_Zombie::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

HRESULT CBody_Heavy_Vampiric_Zombie::Render_CSM(_uint i)
{
//	FAILED_CHECK(__super::Render_CSM(i));

	return S_OK;
}

HRESULT CBody_Heavy_Vampiric_Zombie::Ready_Components()
{
	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_Monster"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.vExtents = _float3(1.0f, 1.0f, 1.0f);
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);


	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));

	return S_OK;
}

HRESULT CBody_Heavy_Vampiric_Zombie::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));
	return S_OK;
}

void CBody_Heavy_Vampiric_Zombie::Check_Frustum()
{
	m_bIsInFrustum = true;
}

CBody_Heavy_Vampiric_Zombie* CBody_Heavy_Vampiric_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Heavy_Vampiric_Zombie* pInstance = new CBody_Heavy_Vampiric_Zombie(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Heavy_Vampiric_Zombie");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Heavy_Vampiric_Zombie::Clone(void* pArg)
{
	CBody_Heavy_Vampiric_Zombie* pInstance = new CBody_Heavy_Vampiric_Zombie(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Heavy_Vampiric_Zombie");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Heavy_Vampiric_Zombie::Pool()
{
	return new CBody_Heavy_Vampiric_Zombie(*this);
}

void CBody_Heavy_Vampiric_Zombie::Free()
{
	__super::Free();
}
