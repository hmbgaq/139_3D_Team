#include "stdafx.h"
#include "Body_Mother.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Player.h"

CBody_Mother::CBody_Mother(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Mother::CBody_Mother(const CBody_Mother& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Mother::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Mother::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//디졸브 값
	m_fDissolveWeight = 0.f;
	m_fDissolve_feather = 0.1f;
	m_vDissolve_Color = { 1.f, 0.f, 0.f };
	m_fDissolve_Discard = 0.2f;
	//m_vDiscardMesh[CBody_Mother::RENDER_STATE::ATTACK] = { 1 }; // 무기 

	return S_OK;
}

void CBody_Mother::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Mother::Tick(_float fTimeDelta)
{
	if (m_iRenderPass == ECast(MONSTER_SHADER::COMMON_DISSOLVE))
	{
		m_bDissolve = true;

		if (m_fDissolveWeight <= 3.f)
			m_fDissolveWeight += fTimeDelta * 0.5f;

	}

	__super::Tick(fTimeDelta);

	if (true == m_bDeadState)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc >= 2.f)
		{
			//Set_EnemyHUD_Dead();
			Set_Dead(true);
		}
	}

}

void CBody_Mother::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());
	//
	//if (m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
	//	m_pModelCom->Play_Animation(fTimeDelta, m_vMovePos, CData_Manager::GetInstance()->Get_Player()->Get_Position());
	//else if (m_pGameInstance->Get_NextLevel() == (_uint)LEVEL_TOOL)
	//	m_pModelCom->Play_Animation(fTimeDelta, m_vMovePos);
	//if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
	//	return;
	//
	//if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
	//	return;

// 	if (true == m_pGameInstance->isIn_WorldPlanes(m_pParentTransform->Get_State(CTransform::STATE_POSITION), 2.f))
// 	{
// 		
// 	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
}

HRESULT CBody_Mother::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPass = m_iShaderPass;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
		//m_pShaderCom->Begin(0);
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

HRESULT CBody_Mother::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CBody_Mother::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_Mother::OnCollisionStay(CCollider* other)
{
	__super::OnCollisionStay(other);
}

void CBody_Mother::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}

void CBody_Mother::SetUp_Animation(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

HRESULT CBody_Mother::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Mother"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_Dissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTexture)));


	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.fRadius = 1.0f;
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);


	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Mother::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	if (m_eRender_State == CBody_Mother::RENDER_STATE::ATTACK)
	{
		m_vCamPos = m_pGameInstance->Get_CamPosition();
		m_vRimColor = { 0.73f, 0.0f, 0.0f, 1.0f };
		m_vBloomPower = _float3(0.5f, 0.5f, 0.5f);
		m_fRimPower = 5.f;

		m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_vBloomPower, sizeof(_float3));
		m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));
	}

	return S_OK;
}

void CBody_Mother::Check_Frustum()
{
	m_bIsInFrustum = true;
}

CBody_Mother* CBody_Mother::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Mother* pInstance = new CBody_Mother(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Mother");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Mother::Clone(void* pArg)
{
	CBody_Mother* pInstance = new CBody_Mother(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Mother");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Mother::Pool()
{
	return new CBody_Mother(*this);
}

void CBody_Mother::Free()
{
	__super::Free();
}

