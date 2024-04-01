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
	if (m_bAlive == false)
		return S_OK;

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
		
		m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_ORIGIN));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody_Infected_D::Render_Shadow()
{
	if (m_bAlive == false)
		return S_OK;

	//FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

HRESULT CBody_Infected_D::OptionSetting()
{
	m_eRender_State = CBody_Infected::RENDER_STATE::ORIGIN;

	return S_OK;
}

HRESULT CBody_Infected_D::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components());

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Model_Infected_D"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	return S_OK;
}

HRESULT CBody_Infected_D::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	m_gCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_gCamFar, sizeof(_float)));

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
