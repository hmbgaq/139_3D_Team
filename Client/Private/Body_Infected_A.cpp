#include "stdafx.h"
#include "Body_Infected_A.h"
#include "GameInstance.h"

CBody_Infected_A::CBody_Infected_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Infected(pDevice, pContext, strPrototypeTag)
{
}

CBody_Infected_A::CBody_Infected_A(const CBody_Infected_A& rhs)
	: CBody_Infected(rhs)
{
}

HRESULT CBody_Infected_A::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Infected_A::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(OptionSetting());

	return S_OK;
}

void CBody_Infected_A::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Infected_A::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CBody_Infected_A::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Infected_A::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Infected_A::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

HRESULT CBody_Infected_A::OptionSetting()
{
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ORIGIN] = {  }; // ÇÇ¶± 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ATTACK] = { 1, 11 }; // ¹«±â 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::NAKED] = {0, 1, 2, 3, 5, 6, 7, 8, 11 }; // °Ñ°¡Á× + ÀÇ»ó + ¹«±â + ±âÅ¸ 

	m_eRender_State = CBody_Infected::RENDER_STATE::ORIGIN;

	return S_OK;
}

HRESULT CBody_Infected_A::Ready_Components()
{	
	FAILED_CHECK(__super::Ready_Components());

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Model_Infected_A"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	return S_OK;
}

HRESULT CBody_Infected_A::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	m_gCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_gCamFar, sizeof(_float)));

	if (m_eRender_State == CBody_Infected::RENDER_STATE::ATTACK)
	{
		m_vCamPos = m_pGameInstance->Get_CamPosition();
		m_vRimColor = { 1.0f, 0.3f, 0.2f, 1.f };
		m_vBloomPower = _float3(1.0f, 1.0f, 1.0f);
		m_fRimPower = 5.f;

		m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_vBloomPower, sizeof(_float3));
		m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));
	}

	return S_OK;
}

CBody_Infected_A* CBody_Infected_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Infected_A* pInstance = new CBody_Infected_A(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Infected_A");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_A::Clone(void* pArg)
{
	CBody_Infected_A* pInstance = new CBody_Infected_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Infected_A");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_A::Pool()
{
	return new CBody_Infected_A(*this);
}

void CBody_Infected_A::Free()
{
	__super::Free();
}
