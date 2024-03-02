#include "stdafx.h"
#include "..\Public\Body_Infected_A.h"
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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Infected_A::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

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
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Infected_A::Render_Shadow()
{
	if (FAILED(__super::Render_Shadow()))
		return E_FAIL;

	return S_OK;
}


HRESULT CBody_Infected_A::Ready_Components()
{	
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Infected_A"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
}

HRESULT CBody_Infected_A::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;
}

CBody_Infected_A* CBody_Infected_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Infected_A* pInstance = new CBody_Infected_A(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
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

	/* 원형객체를 초기화한다.  */
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
