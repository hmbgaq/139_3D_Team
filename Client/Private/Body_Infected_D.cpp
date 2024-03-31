#include "stdafx.h"
#include "Body_Infected_D.h"
#include "GameInstance.h"

CBody_Infected_D::CBody_Infected_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Infected(pDevice, pContext, strPrototypeTag)
{
}

CBody_Infected_D::CBody_Infected_D(const CBody_Infected_D& rhs)
	: CBody_Infected(rhs)
{
}

HRESULT CBody_Infected_D::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Infected_D::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(OptionSetting());

	m_eRender_State = CBody_Infected::RENDER_STATE::ORIGIN;

	return S_OK;
}

void CBody_Infected_D::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Infected_D::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Infected_D::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Infected_D::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Infected_D::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CBody_Infected_D::Update_DiscardMesh()
{
}

HRESULT CBody_Infected_D::OptionSetting()
{
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ORIGIN] = {  }; // ÇÇ¶± 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ATTACK] = { 1, 11 }; // ¹«±â 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::NAKED] = { 0, 1, 2, 3, 5, 6, 7, 8, 11 }; // °Ñ°¡Á× + ÀÇ»ó + ¹«±â + ±âÅ¸ 

	return S_OK;
}

HRESULT CBody_Infected_D::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components());

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Infected_D"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	return S_OK;
}

HRESULT CBody_Infected_D::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

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

CBody_Infected_D* CBody_Infected_D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Infected_D* pInstance = new CBody_Infected_D(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Infected_D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_D::Clone(void* pArg)
{
	CBody_Infected_D* pInstance = new CBody_Infected_D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Infected_D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_D::Pool()
{
	return new CBody_Infected_D(*this);
}

void CBody_Infected_D::Free()
{
	__super::Free();
}
