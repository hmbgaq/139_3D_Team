#include "stdafx.h"
#include "Body_Infected_B.h"
#include "GameInstance.h"

CBody_Infected_B::CBody_Infected_B(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Infected(pDevice, pContext, strPrototypeTag)
{
}

CBody_Infected_B::CBody_Infected_B(const CBody_Infected_B& rhs)
	: CBody_Infected(rhs)
{
}

HRESULT CBody_Infected_B::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Infected_B::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(OptionSetting());

	return S_OK;
}

void CBody_Infected_B::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Infected_B::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Infected_B::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Infected_B::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Infected_B::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

HRESULT CBody_Infected_B::Render_CSM(_uint i)
{
	//FAILED_CHECK(__super::Render_CSM(i));

	return S_OK;
}

HRESULT CBody_Infected_B::OptionSetting()
{
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ORIGIN] = { 9, 10, 11, 12, 13, 14, 15 }; // ÇÇ¶± 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::ATTACK] = { 4 }; // ¹«±â 
	m_vDiscardMesh[CBody_Infected::RENDER_STATE::NAKED] = { 0, 1, 2, 5, 7 }; // °Ñ°¡Á× + ÀÇ»ó + ¹«±â + ±âÅ¸ 

	m_eRender_State = CBody_Infected::RENDER_STATE::ORIGIN;

	return S_OK;
}

HRESULT CBody_Infected_B::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components());

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Model_Infected_B"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	return S_OK;
}

HRESULT CBody_Infected_B::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	m_gCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_gCamFar, sizeof(_float)));
	return S_OK;
}

CBody_Infected_B* CBody_Infected_B::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Infected_B* pInstance = new CBody_Infected_B(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Infected_B");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_B::Clone(void* pArg)
{
	CBody_Infected_B* pInstance = new CBody_Infected_B(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Infected_B");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_B::Pool()
{
	return new CBody_Infected_B(*this);
}

void CBody_Infected_B::Free()
{
	__super::Free();
}
