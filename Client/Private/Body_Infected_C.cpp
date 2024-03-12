#include "stdafx.h"
#include "Body_Infected_C.h"
#include "GameInstance.h"

CBody_Infected_C::CBody_Infected_C(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Infected(pDevice, pContext, strPrototypeTag)
{
}

CBody_Infected_C::CBody_Infected_C(const CBody_Infected_C& rhs)
	: CBody_Infected(rhs)
{
}

HRESULT CBody_Infected_C::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Infected_C::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CBody_Infected_C::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Infected_C::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Infected_C::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Infected_C::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Infected_C::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}


HRESULT CBody_Infected_C::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components());

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Infected_C"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	return S_OK;
}

HRESULT CBody_Infected_C::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	return S_OK;
}

CBody_Infected_C* CBody_Infected_C::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Infected_C* pInstance = new CBody_Infected_C(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Infected_C");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_C::Clone(void* pArg)
{
	CBody_Infected_C* pInstance = new CBody_Infected_C(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Infected_C");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Infected_C::Pool()
{
	return new CBody_Infected_C(*this);
}

void CBody_Infected_C::Free()
{
	__super::Free();
}
