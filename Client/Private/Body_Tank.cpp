#include "stdafx.h"
#include "Body_Tank.h"
#include "GameInstance.h"
#include "Data_Manager.h"

CBody_Tank::CBody_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Monster(pDevice, pContext, strPrototypeTag)
{
}

CBody_Tank::CBody_Tank(const CBody_Tank& rhs)
	: CBody_Monster(rhs)
{
}

HRESULT CBody_Tank::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Tank::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	m_bDissolve = false;
	m_fDissolveWeight = 0.f;
	m_fDissolve_feather = 0.1f;
	m_vDissolve_Color = { 1.f, 0.f, 0.f };
	m_fDissolve_Discard = 0.8f;

	return S_OK;
}

void CBody_Tank::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Tank::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDissolve)
	{
		if (m_fDissolveWeight <= 3.f)
			m_fDissolveWeight += fTimeDelta * 0.6f;
		else
			m_bDissolve = false;
	}
}

void CBody_Tank::Late_Tick(_float fTimeDelta)
{
	if (GAME_STATE::GAMEPLAY != CData_Manager::GetInstance()->Get_GameState())
	{
		m_bIsPaused = true;
	}
	else
	{
		m_bIsPaused = false;
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Tank::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

		if (m_bDissolve)
		{
			m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture");
			m_pShaderCom->Bind_RawValue("g_Dissolve_Weight", &m_fDissolveWeight, sizeof(_float));
			m_pShaderCom->Bind_RawValue("g_Dissolve_feather", &m_fDissolve_feather, sizeof(_float));
			m_pShaderCom->Bind_RawValue("g_Dissolve_Color", &m_vDissolve_Color, sizeof(_float));
			m_pShaderCom->Bind_RawValue("g_Dissolve_ColorRange", &m_fDissolve_Discard, sizeof(_float));

			m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_DISSOLVE));
		}
		else
		{
			m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_ORIGIN));
		}
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody_Tank::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_SHADOW));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CBody_Tank::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_Tank::OnCollisionStay(CCollider* other)
{
	__super::OnCollisionStay(other);
}

void CBody_Tank::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}


HRESULT CBody_Tank::Ready_Components()
{
	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_Monster"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Model_Tank"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For. Texture */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_Dissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTexture)));

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.vExtents = _float3(0.6f, 0.6f, 0.6f);
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);

	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));

	return S_OK;
}

HRESULT CBody_Tank::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	return S_OK;
}

void CBody_Tank::Check_Frustum()
{
	m_bIsInFrustum = true;
}

CBody_Tank* CBody_Tank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Tank* pInstance = new CBody_Tank(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Tank::Clone(void* pArg)
{
	CBody_Tank* pInstance = new CBody_Tank(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Tank::Pool()
{
	return new CBody_Tank(*this);
}

void CBody_Tank::Free()
{
	__super::Free();
}
