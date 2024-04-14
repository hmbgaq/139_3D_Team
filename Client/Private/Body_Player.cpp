#include "stdafx.h"
#include "Body_Player.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"

#include "PhysXCollider.h"
#include "Preset_PhysXColliderDesc.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Player::CBody_Player(const CBody_Player& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
			
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	FAILED_CHECK(Ready_ShaderOption());

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	Set_MouseMove(fTimeDelta, m_bIsUseMouseMove);
}

void CBody_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (Is_Splitted())
	{
		Update_ShootingReaction(fTimeDelta);
	}

	//if()
}

void CBody_Player::Late_Tick(_float fTimeDelta)
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

	if (m_ePlayerRenderPass == RENDER_PASS::RENDER_HEAL)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this), );
	}
	else if (m_ePlayerRenderPass == RENDER_PASS::RENDER_SUPERCHARGE)
	{
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this), );
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE_BLUR, this), );
	}
}

HRESULT CBody_Player::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_ePlayerRenderPass == RENDER_PASS::RENDER_ORIGIN)
	{
		cout << "render Origin " << endl;
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_ORIGIN));

			m_pModelCom->Render((_uint)i);
		}
	}
	else if (m_ePlayerRenderPass == RENDER_PASS::RENDER_HEAL)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_vRimColor = { 0.7f, 0.f, 0.f, 1.f };   /* RimLight */
			m_fRimPower = 2.f;                      /* RimLight */

			/* 기본 */
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			/* RimLight*/
			m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
			m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));

			m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_PLAYER_HEAL));

			m_pModelCom->Render((_uint)i);
		}
	}
	else if (m_ePlayerRenderPass == RENDER_PASS::RENDER_SNOWMOUNTAIN)
	{
			cout << "render SnowMountain" << endl;
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_vRimColor = { 1.f, 1.f, 1.f, 1.f };   /* RimLight */
			m_fRimPower = 4.f;                         /* RimLight */

			/* Base */
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			/* RimLight */
			m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
			m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));

			m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_PLAYER_SNOWMOUNTAIN));

			m_pModelCom->Render((_uint)i);
		}
	}
	else if (m_ePlayerRenderPass == RENDER_PASS::RENDER_SUPERCHARGE)
	{
		cout << "render Super Charge" << endl;

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_vRimColor = { 0.f, 0.0f, 0.7f, 1.f };   /* RimLight */
			m_fRimPower = 2.f;                         /* RimLight */
			m_vBloomPower = { 0.f, 0.f, 0.8f};

			/* Base */
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			/* RimLight */
			m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
			m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));
			m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_vBloomPower, sizeof(_float3));

			m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_SUPER_CHARGE));

			m_pModelCom->Render((_uint)i);
		}
	}
	return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));

	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_iCurrnetLevel)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_iCurrnetLevel)));

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pShaderCom->Begin(2);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_OutLine()
{
	cout << "render outline " << endl;

	FAILED_CHECK(Bind_ShaderResources());

	if (m_ePlayerRenderPass == RENDER_PASS::RENDER_HEAL)
	{
		m_vLineColor = { 0.5f, 0.f, 0.f, 1.f };
		m_fLineThick = { 0.3f };
		m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));
	}
	else if (m_ePlayerRenderPass == RENDER_PASS::RENDER_SUPERCHARGE)
	{
		m_vLineColor = { 0.7f, 0.7f, 0.7f, 1.f };
		m_fLineThick = { 0.3f };
		m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_OUTLINE));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_OutLine_Blur()
{
	cout << "render outline blur " << endl;

	FAILED_CHECK(Bind_ShaderResources());

	if (m_ePlayerRenderPass == RENDER_PASS::RENDER_HEAL)
	{
		m_vLineColor = { 0.5f, 0.f, 0.f, 1.f }; // Red
		m_fLineThick = { 0.3f };
		m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));
	}
	else if (m_ePlayerRenderPass == RENDER_PASS::RENDER_SUPERCHARGE)
	{
		m_vLineColor = { 0.7f, 0.7f, 0.7f, 1.f }; // White 
		m_fLineThick = { 0.3f };
		m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick, sizeof(_float));
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pShaderCom->Begin(ECast(ANIM_SHADER::ANIM_OUTLINE));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_CSM(_uint i)
{
	return S_OK;
}

void CBody_Player::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_Player::OnCollisionStay(CCollider* other)
{
	//__super::OnCollisionStay(other);
}

void CBody_Player::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}

HRESULT CBody_Player::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Rentier"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER);
	BoundingDesc.vExtents = _float3(0.2f, 1.f, 0.2f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));
	
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);
	//m_pTrail->Set_Play(true); // 시작 때 트레일 켜기
	//if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_PhysXCollider"),
	//	TEXT("Com_PhysXCollider"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
	//	return E_FAIL;

	////m_pPhysXCollider->ini

	//CPhysXCollider::PhysXColliderDesc tPhysXColliderDesc;
	//Preset::PhysXColliderDesc::DynamicPieceSetting(tPhysXColliderDesc, m_pTransformCom);
	//m_pPhysXCollider->CreatePhysXActor(tPhysXColliderDesc);
	//m_pPhysXCollider->Add_PhysXActorAtScene();
	
	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	_float4 fCamPos = m_pGameInstance->Get_CamPosition();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &fCamPos, sizeof(_float4)));

	return S_OK;
}

void CBody_Player::Check_Frustum()
{
	m_bIsInFrustum = true;
}

HRESULT CBody_Player::Ready_ShaderOption()
{
	/* 레벨에 따른 셰이더 옵션 조절 */
	if (m_iCurrnetLevel == ECast(LEVEL::LEVEL_SNOWMOUNTAIN))
		m_ePlayerRenderPass = RENDER_PASS::RENDER_SNOWMOUNTAIN;
	else
		m_ePlayerRenderPass = RENDER_PASS::RENDER_ORIGIN;

	/* 셰이더에 올릴 값들에 대한 옵션값 조절 */
	// 1. OutLine
	m_vLineColor		= { 0.5f, 0.f, 0.f, 1.f };
	m_fLineThick		= { 0.3f };

	// 2. RimLight 
	//m_vHealRimColor = { 0.7f, 0.f, 0.f, 1.f };
	//m_fHealRimPower = 2.f;

	//m_vSnowRimColor = { 0.7f, 0.7f, 0.7f, 1.f };
	//m_fSnowRimPower = 2.f;

	return S_OK;
}

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Player::Pool()
{
	return new CBody_Player(*this);
}

void CBody_Player::Free()
{
	__super::Free();

	/* 이펙트 무한루프라 제거 */
}
