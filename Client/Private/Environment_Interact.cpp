#include "stdafx.h"
#include "..\Public\Environment_Interact.h"

#include "GameInstance.h"
#include "Character.h"
#include "Player.h"
#include "Level_Loading.h"
#include "SMath.h"
#include "Navigation.h"
#include "Cell.h"
#include "Bounding_AABB.h"

// !Add UI
#include "UI_Manager.h"
#include "UI_Interaction.h"
#include "UI_Weakness.h"
#include "UI.h"

#include "Data_Manager.h"
#include "Bone.h"

CEnvironment_Interact::CEnvironment_Interact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
	, m_pUIManager(CUI_Manager::GetInstance())
{
		Safe_AddRef(m_pUIManager);
}

CEnvironment_Interact::CEnvironment_Interact(const CEnvironment_Interact & rhs)
	: CGameObject(rhs)
	, m_pUIManager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CEnvironment_Interact::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CEnvironment_Interact::Initialize(void* pArg)
{	

	m_tEnvironmentDesc = *(ENVIRONMENT_INTERACTOBJECT_DESC*)pArg;

	m_iCurrentLevelIndex = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONPUSH)
	{
		m_pRigidBodyCom = CRigidBody::Create(m_pDevice, m_pContext);
		NULL_CHECK_RETURN(m_pRigidBodyCom, E_FAIL);
		if (nullptr != Find_Component(g_pRigidBodyTag))
			return E_FAIL;

		m_Components.emplace(g_pRigidBodyTag, m_pRigidBodyCom);
		Safe_AddRef(m_pRigidBodyCom);
		m_pRigidBodyCom->Set_Owner(this);
		m_pRigidBodyCom->Set_Transform(m_pTransformCom);


		m_pRigidBodyCom->Set_UseGravity(false);

		
		//m_tEnvironmentDesc.vBodyColliderSize = { 2.f, 2.f, 2.f};
		//m_tEnvironmentDesc.vBodyColliderCenter = { 0.f, 1.f, 0.f};
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (false == m_tEnvironmentDesc.bPreview)
		m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);

	if(m_iCurrentLevelIndex != (_uint)LEVEL_TOOL)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());
		Safe_AddRef(m_pPlayer);
		m_bFindPlayer = true;
	}

	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONEVENT)
	{
		m_pTransformCom->Set_Speed(m_fSplineSpeed);
		m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(m_tEnvironmentDesc.fRotationSpeed));
	}
	
	XMStoreFloat4x4(&m_InitMatrix, XMMatrixIdentity());

	if (m_iCurrentLevelIndex == (_uint)LEVEL_SNOWMOUNTAIN && m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONEVENT)
	{
		if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONEVENT)
		{
			Init_WagonEvent();
		}

		if (m_tEnvironmentDesc.bOwner == true)
		{
			if (FAILED(Find_InteractGroupObject()))
				return E_FAIL;
		}
	}

	
	if (m_tEnvironmentDesc.bRotate == true)
	{
		UnEnable_UpdateCells();
		m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(m_tEnvironmentDesc.fRotationSpeed));
	}
	//if (m_tEnvironmentDesc.bOffset == true && m_tEnvironmentDesc.bOwner == false)
	//{
	//	m_bInteractEnable = false;
	//}

	if (m_tEnvironmentDesc.bEnable == true)
	{
		if(FAILED(Load_EnableJson()))
			return E_FAIL;
	}

	FAILED_CHECK(Classification_Model());

	// !UI Add UI_Interaction
	if(m_iCurrnetLevel != (_uint)LEVEL_TOOL)
	Find_UI_For_InteractType();


	if (m_tEnvironmentDesc.bInteractMoveMode == true)
	{
		m_bExit = true;
	}

	//if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_JUMP300)
	//{
	//	f = TRUE;
	//}


	if (m_tEnvironmentDesc.bAnimModel == true)
	{
		if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_ROPEDOWN || m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_ROPECLIMB)
		{
			m_pWeaknessUI = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
			m_pWeaknessUI->Set_Active(true);
			m_pWeaknessUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldFloat4x4(), _float3(0.f, 1.f, 0.f));
			dynamic_cast<CUI_Weakness*>(m_pWeaknessUI)->Set_ColliderRadius(1.f);
		}
	}

	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_LEVER)
	{
		
		
		
		vector<pair<_int, _bool>>* pvecLevelSwitch = CData_Manager::GetInstance()->Get_LevelSwitchContainer();
		
		pvecLevelSwitch->push_back(make_pair(m_tEnvironmentDesc.iSwitchIndex, false));
		
	}
	
	return S_OK;
}

void CEnvironment_Interact::Priority_Tick(_float fTimeDelta)
{
	if(m_pInteractColliderCom != nullptr)
		m_pInteractColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CEnvironment_Interact::Tick(_float fTimeDelta)
{
	if (m_iCurrentLevelIndex == (_uint)LEVEL_SNOWMOUNTAIN)
	{
		if (m_pGameInstance->Key_Down(DIK_F7))
		{
			Start_SplineEvent();
			m_pPlayer->SetState_InteractCartRideLoop();
		}
		if(m_pGameInstance->Key_Down(DIK_NUMPAD1))
			m_vecPointChecks[0] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
			m_vecPointChecks[1] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
			m_vecPointChecks[2] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
			m_vecPointChecks[3] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
			Reset_TestEvent();
	}

	

	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL && m_bFindPlayer == false)
	{	
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());

		if(m_pPlayer != nullptr)
		{
			Safe_AddRef(m_pPlayer);
			m_bFindPlayer = true;
		}
	}


	if (true == m_tEnvironmentDesc.bAnimModel)// && true == m_bPlay)
	{
		//if(m_tEnvironmentDesc.eInteractType != CEnvironment_Interact::INTERACT_ZIPLINE)
			m_pModelCom->Play_Animation(fTimeDelta, _float3());
	}

	


	if(m_bInteractEnable == true && m_tEnvironmentDesc.eInteractType != CEnvironment_Interact::INTERACT_NONE)
		Interact();

	if(m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_ONCE && m_bInteract == true && m_bFindPlayer == true)
	{
		if (m_pPlayer->Get_CurrentAnimIndex() == (_uint)CPlayer::Player_State::Player_InteractionJumpDown300 && m_pPlayer->Is_Animation_End() == true)
		{
			UnEnable_UpdateCells();
		}

		if (m_pPlayer->Get_CurrentAnimIndex() == (_uint)CPlayer::Player_State::Player_ZipLine_Stop && m_pPlayer->Is_Animation_End() == true)
		{
			UnEnable_UpdateCells();
			m_pPlayer->Get_Navigation()->Set_CurrentIndex(m_tEnvironmentDesc.iArrivalCellIndex);
		}

		if (m_pPlayer->Get_CurrentAnimIndex() == (_uint)CPlayer::Player_State::Player_InteractionClimb200 && m_pPlayer->Is_Animation_End() == true && m_tEnvironmentDesc.bInteractMoveMode == true)
			m_bInteractMoveMode = true;
		
	}
	else if(m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_LOOP && m_bInteractMoveMode == false && m_bFindPlayer == true)
	{
		if(m_pPlayer->Get_CurrentAnimIndex() == (_uint)CPlayer::Player_State::Player_InteractionClimb200 && m_pPlayer->Is_Animation_End() == true && m_tEnvironmentDesc.bInteractMoveMode == true)
			m_bInteractMoveMode = true;

		if (m_pPlayer->Get_CurrentAnimIndex() == (_uint)CPlayer::Player_State::Player_InteractionJumpDown300 && m_pPlayer->Is_Animation_End() == true)
		{
			UnEnable_UpdateCells();
		}
		//else if (m_pPlayer->Is_Interection() == false && m_tEnvironmentDesc.bInteractMoveMode == true)
		//	m_bInteractMoveMode = true;
	}

	if(m_bInteractMoveMode == true)
		Check_InteractMoveCollider();

	// !UI Add UI_Interaction
	if (m_bEnable == true)
	{
		if (m_pUI_Interaction != nullptr)
		{
			// 각 상호작용 객체에 맞게 vOffset 조절해줘야함.
			m_pUI_Interaction->SetUp_WorldToScreen(m_pTransformCom->Get_WorldMatrix(), m_tEnvironmentDesc.vInteractColliderCenter/*, vOffset*/); // 위치 갱신
			m_pUI_Interaction->Set_OnInteraction(m_bInteract);	// 상호작용을 했는지
		}
	}
		
	


	if (m_bSpline == true)
	{
		Spline_Loop(fTimeDelta);
		//Spline_LoopDoublePoint(fTimeDelta);
	}

	if (m_tEnvironmentDesc.bRotate == true && m_bInteractEnable == true)
	{
		if (true == RotationCheck(fTimeDelta))
		{
			StartGroupInteract();
			m_bInteractEnable = false;
			m_bInteract = true;
			Enable_UpdateCells();
		}
	}

	if (m_tEnvironmentDesc.bOffset == true)
	{
		Move_For_Offset();
	}

	if (m_tEnvironmentDesc.bRootTranslate == true)
	{
		//Move_For_PlayerOffset();
		Move_For_PlayerRootMotion();
	}

	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_ROPECLIMB || m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_ROPEDOWN)
	{
		Rope_ChainFunction(fTimeDelta);
	}
	//if (m_tEnvironmentDesc.bOwner == false && m_pOwnerInteract != nullptr)
	//{
	//	if (true == Check_OwnerEnablePosition())
	//		m_bInteractEnable = true;
	//	else
	//		m_bInteractEnable = false;
	//}

	// 소영 - 렌더 필요사항
	if (m_bRenderOutLine)
	{
		m_fTimeAcc += (m_bIncrease ? fTimeDelta : -fTimeDelta);
		m_bIncrease = (m_fTimeAcc >= 0.7f) ? false : (m_fTimeAcc <= 0.f) ? true : m_bIncrease;
	}
	
	if (m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if(m_pInteractColliderCom != nullptr)
		m_pInteractColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if(m_pInteractMoveColliderCom != nullptr)
		m_pInteractMoveColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if(m_pMoveRangeColliderCom != nullptr)
		m_pMoveRangeColliderCom->Update(XMMatrixIdentity());

	
	if (m_pPlayer != nullptr && m_tEnvironmentDesc.bLevelChange == true && m_bInteract == true && true == m_pPlayer->Is_Animation_End())
	{
		if(m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_InteractionJumpDown300)
		{
			m_pGameInstance->Request_Level_Opening(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
			m_bInteract = false;
		}

	}
}
	



void CEnvironment_Interact::Late_Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );

	

	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		if (m_pColliderCom != nullptr && m_bColliderRender == true)
			m_pGameInstance->Add_DebugRender(m_pColliderCom);

		if (m_pInteractColliderCom != nullptr && m_bInteractColliderRender == true)
			m_pGameInstance->Add_DebugRender(m_pInteractColliderCom);

		if (m_pMoveRangeColliderCom != nullptr && m_bMoveColliderRender == true)
			m_pGameInstance->Add_DebugRender(m_pMoveRangeColliderCom);
		
		if (m_pInteractMoveColliderCom != nullptr && m_bInteractMoveColliderRender == true)
			m_pGameInstance->Add_DebugRender(m_pInteractMoveColliderCom);
	}


	if(m_pRigidBodyCom != nullptr)
		m_pRigidBodyCom->Late_Tick(fTimeDelta);
	/* 소영 보류 */
	if (true == m_bRenderOutLine)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this), );
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this), );
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_CascadeObject(0, this), );
		//FAILED_CHECK_RETURN(m_pGameInstance->Add_CascadeObject(1, this), );
	}
}

HRESULT CEnvironment_Interact::Render()
{
	FAILED_CHECK(Bind_ShaderResources());
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (true == m_bRenderOutLine)
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_tEnvironmentDesc.bAnimModel == true)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			}

			auto& iter = find(m_vChainMesh.begin(), m_vChainMesh.end(), i);
			if (iter != m_vChainMesh.end())
			{
				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE); // 1
				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS); // 6
				m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float));
				m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_WHITEBLINK));
			}
			else 
			{
				m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
				m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
				m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
				m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);
			}

			m_pModelCom->Render((_uint)i);
		}
	}
	else
	{
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_tEnvironmentDesc.bAnimModel == true)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			}

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

			if (true == m_bRenderOutLine)
				m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_WHITEBLINK));
			else
				m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);

			m_pModelCom->Render((_uint)i);
		}
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_SHADOW));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Render_OutLine()
{
	if (false == m_bRenderOutLine)
		return S_OK;

	_float Dist = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_Pos()));
	m_fLineThick_Ratio = m_fLineThick / Dist;

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_LineThick", &m_fLineThick_Ratio, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float)));

	FAILED_CHECK(Bind_ShaderResources());
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		auto& iter = find(m_vChainMesh.begin(), m_vChainMesh.end(), i);
		if (iter != m_vChainMesh.end())
		{
			m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_OUTLINE_BLINK));
			m_pModelCom->Render((_uint)i);
		}
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Render_CSM(_uint i)
{
	FAILED_CHECK(Bind_ShaderResources());	
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pShaderCom->Begin(3);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

_bool CEnvironment_Interact::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_Interact::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

void CEnvironment_Interact::OnCollisionEnter(CCollider* other)
{
	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONPUSH)
	{
		if (m_bFindPlayer == true)
			Collision_Push_ForPlayer(other);

		Collision_Push_ForOtherInteract(other);
	}

	
}

void CEnvironment_Interact::OnCollisionStay(CCollider* other)
{
	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONPUSH)
	{
		if (m_bFindPlayer == true)
			Collision_Push_ForPlayer(other);

		Collision_Push_ForOtherInteract(other);
	}
}

void CEnvironment_Interact::OnCollisionExit(CCollider* other)
{
	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONPUSH)
	{
		if(m_bFindPlayer == true)
			Collision_Push_ForPlayer(other);

		Collision_Push_ForOtherInteract(other);
	}
}

void CEnvironment_Interact::Collision_Push_ForPlayer(CCollider* other)
{
	
	CGameObject* pTarget = other->Get_Owner();


	if (nullptr == pTarget) return;

	if (typeid(*pTarget) == typeid(CEnvironment_Interact))
	{
		return;
	}

	CCharacter* pTargetCharacter = nullptr;

	CGameObject* pTargetCharcterOwner = pTarget->Get_Object_Owner();

	if (pTargetCharcterOwner != nullptr)
	{
		pTargetCharacter = dynamic_cast<CCharacter*>(pTargetCharcterOwner);
		_vector vTargetPos = pTargetCharcterOwner->Get_Position_Vector();
		pTargetCharacter->Add_Force(this->Calc_Look_Dir_XZ(vTargetPos) * -1, 7.f * m_pGameInstance->Get_TimeDelta());
	}
}

void CEnvironment_Interact::Collision_Push_ForOtherInteract(CCollider* other)
{
	CGameObject* pTarget = other->Get_Owner();
	if (nullptr == pTarget) return;

	CEnvironment_Interact* pTargetInteract = dynamic_cast<CEnvironment_Interact*>(pTarget);

	if (pTargetInteract != nullptr)
	{
		m_bMove = false;
		
		_vector vTargetPos = pTargetInteract->Get_Position_Vector();
		pTargetInteract->Add_Force(this->Calc_Look_Dir_XZ(vTargetPos) * -1, 7.f * m_pGameInstance->Get_TimeDelta());
	}
}

void CEnvironment_Interact::Set_AnimationIndex(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

void CEnvironment_Interact::Set_Animation(_uint iAnimIndex, CModel::ANIM_STATE _eAnimState, _uint iTargetKeyFrameIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex, _eAnimState, false, m_pModelCom->Get_TickPerSecond() / 10.f, iTargetKeyFrameIndex);
}

void CEnvironment_Interact::Set_ColliderSize(_float3 vColliderSize)
{
	{
		CBounding* pBounding = m_pColliderCom->Get_Bounding();

		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Extents = vColliderSize;
		m_tEnvironmentDesc.vBodyColliderSize = vColliderSize;
	}
}

void CEnvironment_Interact::Set_ColliderCenter(_float3 vColliderCenter)
{
	{
		CBounding* pBounding = m_pColliderCom->Get_Bounding();


		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Center = vColliderCenter;
		m_tEnvironmentDesc.vBodyColliderCenter = vColliderCenter;
	}
}

void CEnvironment_Interact::Set_MoveRangeColliderSize(_float3 vColliderSize)
{
	{
		CBounding* pBounding = m_pMoveRangeColliderCom->Get_Bounding();

		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Extents = vColliderSize;
		m_tEnvironmentDesc.vMoveRangeColliderSize = vColliderSize;
	}

	
	
}

void CEnvironment_Interact::Set_MoveRangeColliderCenter(_float3 vColliderCenter)
{
	{
		CBounding* pBounding = m_pMoveRangeColliderCom->Get_Bounding();
		

		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Center = vColliderCenter;
		m_tEnvironmentDesc.vMoveRangeColliderCenter = vColliderCenter;
	}

}

void CEnvironment_Interact::Set_InteractMoveColliderSize(_float3 vInteractColliderSize)
{
	{
		CBounding* pBounding = m_pInteractMoveColliderCom->Get_Bounding();

		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Extents = vInteractColliderSize;
		m_tEnvironmentDesc.vInteractMoveColliderSize = vInteractColliderSize;
	}

}

void CEnvironment_Interact::Set_InteractMoveColliderCenter(_float3 vInteractColliderCenter)
{
	{
		CBounding* pBounding = m_pInteractMoveColliderCom->Get_Bounding();


		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Center = vInteractColliderCenter;
		m_tEnvironmentDesc.vInteractMoveColliderCenter = vInteractColliderCenter;
	}
}

void CEnvironment_Interact::Add_Force(_vector In_vDir, _float In_fPower)
{
	m_pRigidBodyCom->Add_Force(In_vDir, In_fPower);
}

void CEnvironment_Interact::StartGroupInteract()
{
	_int iInteractGroupSize = (_int)m_vecInteractGroup.size();

	for (_uint i = 0; i < (_uint)iInteractGroupSize; ++i)
	{
		m_vecInteractGroup[i]->Set_Interact(false);
		//m_vecInteractGroup[i]->set
	}
}

void CEnvironment_Interact::Reset_Interact()
{
	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);
	
	m_bInteract = false;
	m_bInteractEnable = true;
}


#ifdef _DEBUG

void CEnvironment_Interact::Set_InteractColliderSize(_float3 vColliderSize)
{
	CBounding* pBounding = m_pInteractColliderCom->Get_Bounding();

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);
	
	if(pAABB == nullptr)
		return;

	BoundingBox* pBox = pAABB->Get_OriginBounding();

	pBox->Extents = vColliderSize;
	m_tEnvironmentDesc.vInteractColliderSize = vColliderSize;

	//m_pInteractColliderCom->Set_Bounding()
}

void CEnvironment_Interact::Set_InteractColliderCenter(_float3 vColliderCenter)
{
	CBounding* pBounding = m_pInteractColliderCom->Get_Bounding();

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

	if (pAABB == nullptr)
		return;

	BoundingBox* pBox = pAABB->Get_OriginBounding();

	pBox->Center = vColliderCenter;
	m_tEnvironmentDesc.vInteractColliderCenter = vColliderCenter;
}

void CEnvironment_Interact::Set_LevelChangeType(_bool bLevelChange, LEVEL eLevel)
{
	m_tEnvironmentDesc.bLevelChange = bLevelChange;
	m_tEnvironmentDesc.eChangeLevel = eLevel;
}

#endif



#ifdef _DEBUG

_bool CEnvironment_Interact::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}


#endif

void CEnvironment_Interact::Interact()
{
	
	if(m_bFindPlayer == false)
		return;
	
	if (true == m_pInteractColliderCom->Is_Collision(m_pPlayer->Get_Collider()))
	{

		// !UI Add
		if (m_pUI_Interaction != nullptr)
			m_pUI_Interaction->Set_OnCollision(true);	// 상호작용을 할 수 있는 범위에 들어왔는지 (Collision)

		if (m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_LOOP)
		{
			switch (m_tEnvironmentDesc.eInteractType)
			{
				case CEnvironment_Interact::INTERACT_JUMP100:
				{
					//CPlayer::Player_State::Player_InteractionJumpDown100;

					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractJumpDown100();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractJumpDown100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractClimb100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
						
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP200:
				{

					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractJumpDown200();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractJumpDown200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractClimb200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP300:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractJumpDown300();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (true == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractJumpDown300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractClimb300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_VAULT100:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractVault100();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_VAULT200:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractVault200();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_WAGONPUSH:
				{
					_int iCurrentAnimIndex = m_pPlayer->Get_CurrentAnimIndex();

					if (iCurrentAnimIndex != (_int)CPlayer::Player_State::Player_InteractionPush_Rock_Idle)
					{
						if (iCurrentAnimIndex == (_int)CPlayer::Player_State::Player_Run_F || iCurrentAnimIndex == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractionPush_Rock_Idle();
						}

					}


					if (m_bMove == true)
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					else
						m_pPlayer->Set_RootMoveRate(_float3(0.f, 0.f, 0.f));

					break;
				}

				case CEnvironment_Interact::INTERACT_WAGONJUMP:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractCartRideStart();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTEARCT_WAGONROPEJUMP:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractCartRideWagonJump();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB100:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb100();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB200:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb200();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB300:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb300();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB450:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb450();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb450();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{F
						//	m_pPlayer->SetState_InteractJumpDown300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_SLIDE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						m_pPlayer->SetState_InteractSlide();

					break;
				}

				case CEnvironment_Interact::INTERACT_LEVER:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						m_pPlayer->SetState_InteractSmallLever();
						Switch_Funtion();
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_PLANK:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						m_pPlayer->SetState_InteractPlankStart();
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_ROPECLIMB:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_InteractClimbRope();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}
						
						
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	m_pPlayer->SetState_InteractClimbRope();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iReverseLadderCount);
						//	m_pPlayer->SetState_InteractRopeDown();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}

					break;
				}

				case CEnvironment_Interact::INTERACT_ROPEDOWN:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_InteractRopeDown();
					}
					break;
				}

				case CEnvironment_Interact::INTERACT_DOOROPEN:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractDoorOpen();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}
					break;
				}

				case CEnvironment_Interact::INTERACT_LADDERUP:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder1")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);

						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder2")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder3")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder4")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder5")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}

						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						m_pPlayer->SetState_InteractLadderUpStart();

						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder1")
						//	{
						//		m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder2")
						//	{
						//		m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder3")
						//	{
						//		m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder4")
						//	{
						//		m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder5")
						//	{
						//		m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	}
						//
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//	m_pPlayer->SetState_InteractLadderUpStart();
						//}
						//else
						//{
						//	if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder1")
						//	{
						//		m_pPlayer->SetState_InteractJumpDown300();
						//
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder2")
						//	{
						//		m_pPlayer->SetState_InteractJumpDown300();
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder3")
						//	{
						//		m_pPlayer->SetState_InteractJumpDown200();
						//	}
						//	else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder4")
						//	{
						//		m_pPlayer->SetState_InteractJumpDown200();
						//	}
						//
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
						
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_WHIPSWING:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractWhipSwing();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_WHIPPULL:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractWhipPull();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_ROTATIONVALVE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractRotationValve();

						StartGroupInteract();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}

					break;

				}
				case CEnvironment_Interact::INTERACT_ZIPLINE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						ZipLine_Function();
						m_pPlayer->SetState_InteractZipLine();
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}

					break;

				}

				case CEnvironment_Interact::INTERACT_CROUCHUNDER:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_CrouchUnder();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}

					break;

				}

				case CEnvironment_Interact::INTERACT_CROUCHUNDERGATE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_CrouchUnderGate();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}

					break;

				}
				
			}


			Enable_UpdateCells();

			if (m_tEnvironmentDesc.bUseGravity == false)
				m_pPlayer->Set_UseGravity(false);

			m_bInteract = true;

			if (true == m_tEnvironmentDesc.bLevelChange && m_bInteract == true)
			{
				if (m_pPlayer->Is_Inputable_Front(32) && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
				{
					m_pGameInstance->Request_Level_Opening(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
					m_bInteract = false;
				}
			}

		}
		else if (m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_ONCE && m_bInteract == false)
		{
			switch (m_tEnvironmentDesc.eInteractType)
			{
				case CEnvironment_Interact::INTERACT_JUMP100:
				{


					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractJumpDown100();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractJumpDown100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractClimb100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP200:
				{

					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractJumpDown200();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractJumpDown200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractClimb200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP300:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractJumpDown300();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractJumpDown300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractClimb300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//
						//}
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_VAULT100:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractVault100();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);

					}

					break;
				}



				case CEnvironment_Interact::INTERACT_VAULT200:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractVault200();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);

						
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_WAGONPUSH:
				{
					_int iCurrentAnimIndex = m_pPlayer->Get_CurrentAnimIndex();

					if (iCurrentAnimIndex != (_int)CPlayer::Player_State::Player_InteractionPush_Rock_Idle)
					{
						if (iCurrentAnimIndex == (_int)CPlayer::Player_State::Player_Run_F || iCurrentAnimIndex == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractionPush_Rock_Idle();
							m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						}

					}

				}

				case CEnvironment_Interact::INTERACT_WAGONJUMP:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractCartRideStart();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTEARCT_WAGONROPEJUMP:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractCartRideWagonJump();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB100:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb100();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown100();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}

					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB200:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb200();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown200();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB300:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb300();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB450:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractClimb450();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->SetState_InteractClimb450();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//}
						//else
						//{
						//	m_pPlayer->SetState_InteractJumpDown300();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_SLIDE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractSlide();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_LEVER:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractSmallLever();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						Switch_Funtion();
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_PLANK:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_InteractPlankStart();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);

					}
					break;
				}

				case CEnvironment_Interact::INTERACT_ROPECLIMB:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_InteractClimbRope();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);

						//if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						//{
						//	m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	m_pPlayer->SetState_InteractClimbRope();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
						//
						//}
						//else
						//{
						//	m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						//	m_pPlayer->SetState_InteractRopeDown();
						//	m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
						//
						//}
					}

					break;
				}

				case CEnvironment_Interact::INTERACT_ROPEDOWN:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractRopeDown();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_DOOROPEN:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractDoorOpen();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_LADDERUP:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder1")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder2")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder3")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder4")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}
						else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder5")
						{
							m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						}

						m_pPlayer->SetState_InteractLadderUpStart();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);

// 						if (false == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
// 						{
// 							if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder1")
// 							{
// 								m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder2")
// 							{
// 								m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder3")
// 							{
// 								m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder4")
// 							{
// 								m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder5")
// 							{
// 								m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
// 							}
// 
// 							m_pPlayer->SetState_InteractLadderUpStart();
// 							m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);
// 						}
// 						else
// 						{
// 							if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder1")
// 							{
// 								m_pPlayer->SetState_InteractJumpDown300();
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder2")
// 							{
// 								m_pPlayer->SetState_InteractJumpDown300();
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder3")
// 							{
// 								m_pPlayer->SetState_InteractJumpDown200();
// 							}
// 							else if (m_tEnvironmentDesc.strModelTag == L"Prototype_Component_Model_ChainClimbLadder4")
// 							{
// 								m_pPlayer->SetState_InteractJumpDown200();
// 							}
// 							m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
// 
// 						}
// 
 					}

					break;
				}

				case CEnvironment_Interact::INTERACT_WHIPSWING:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractWhipSwing();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_WHIPPULL:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractWhipPull();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_ROTATIONVALVE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->SetState_InteractRotationValve();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
					}


					break;
				}

				case CEnvironment_Interact::INTERACT_ZIPLINE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						ZipLine_Function();
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_InteractZipLine();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
					}

					break;

				}

				case CEnvironment_Interact::INTERACT_CROUCHUNDER:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_CrouchUnder();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
					}

					break;

				}

				case CEnvironment_Interact::INTERACT_CROUCHUNDERGATE:
				{
					if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
					{
						m_pPlayer->Set_Ladder_Count(m_tEnvironmentDesc.iLadderCount);
						m_pPlayer->SetState_CrouchUnderGate();
						m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerReverseRootMoveRate);
					}

					break;

				}


			}
		}

			if(m_tEnvironmentDesc.bUseGravity == false)
				m_pPlayer->Set_UseGravity(false);
			
			m_bInteract = true;
		

		if (true == m_tEnvironmentDesc.bLevelChange && m_bInteract == true)
		{
			if (m_pPlayer->Is_Inputable_Front(32) && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL && m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_JUMP200)
			{
				m_pGameInstance->Request_Level_Opening(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
				//m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
				m_bInteract = false;
			}

		
		}
	}
	else
	{
		if (m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_LOOP)
		{
			if(m_tEnvironmentDesc.eInteractType != CEnvironment_Interact::INTERACT_JUMP300)
			UnEnable_UpdateCells();
			m_bInteract = false;


			// !UI Add
			if (m_pUI_Interaction != nullptr)
			{
				m_pUI_Interaction->Set_OnCollision(false);	// 상호작용을 할 수 있는 범위에서 나갔는지 (Collision)
				m_pUI_Interaction->Reset_Interaction_UI();	// 나가면서 UI 리셋
			}
		}
		else if (m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_ONCE && m_bInteract == false) 
		{
			//Enable_UpdateCells();

		}
	}
		
}

void CEnvironment_Interact::Rope_ChainFunction(const _float fTimeDelta)
{
	if (m_pWeaknessUI != nullptr)
	{
		if (m_pWeaknessUI->Get_Enable() == false && m_bChainEnable == false)
		{
			m_pModelCom->Set_Animation(5, CModel::ANIM_STATE::ANIM_STATE_NORMAL);
			//m_pModelCom->Set_Animation(3, CModel::ANIM_STATE::ANIM_STATE_NORMAL);
			m_bChainEnable = true;
		}
		else
		{
			m_pWeaknessUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldFloat4x4(), _float3(0.f, 1.f, 0.f));
		}

		//!if (m_pPlayer != nullptr) //! 로프짧아져서 안됨^
		//!{
		//!	if (m_pPlayer->Get_CurrentAnimIndex() == (_uint)CPlayer::Player_State::Player_InteractionClimbRope_Stop)
		//!	{
		//!		m_pModelCom->Set_Animation(7, CModel::ANIM_STATE::ANIM_STATE_NORMAL);
		//!	}
		//!}

	}
}

void CEnvironment_Interact::Switch_Funtion()
{
	m_bLeverSwitch = !m_bLeverSwitch;
	CData_Manager::GetInstance()->Set_LevelSwitchForIndex(m_tEnvironmentDesc.iSwitchIndex, m_bLeverSwitch);

	//!CData_Manager::GetInstance()->Set_LevelSwitchForIndex(_int iIndex, _bool bSwitch) { m_vecLevelSwitch[iIndex] = bSwitch; }
}

void CEnvironment_Interact::ZipLine_Function()
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_float4 ZipLineRootBonePosition = m_pModelCom->Get_BonePtr("Root")->Get_CombinedPosition(WorldMatrix);
	_float4 ZipLineFinalBonePosition = m_pModelCom->Get_BonePtr("Rope_030")->Get_CombinedPosition(WorldMatrix);


	_float4 vPlayerHandPos = m_pPlayer->Get_HandPos();
	_float	fOffsetY = vPlayerHandPos.y - m_pPlayer->Get_Position().y;

	ZipLineRootBonePosition.y -= fOffsetY;
	//_vector vDir = ZipLineFinalBonePosition - m_pPlayer->Get_HandPos();

	_vector vZipLineDir = XMVector4Normalize(XMLoadFloat4(&ZipLineFinalBonePosition) - XMLoadFloat4(&ZipLineRootBonePosition));
	
	
	//! 먼저 시작 점을 구해야해. 시작점은 로프의 첫번째 뼈위치야. 즉 루트 뼈 인거지.
	//! 루트뼈의 위치로 플레이어를 이동시키면 안돼.
	//! 플레이어의 위치로부터 손 위치까지의 오프셋 y만큼 벌어져야해.
	
	m_pPlayer->Set_InteractObject(this);
	m_pPlayer->Set_InteractDir(XMVector3Normalize(vZipLineDir));

}
_float4x4 CEnvironment_Interact::Get_ZipLineBoneMatrix()
{
	return m_pModelCom->Get_BonePtr("L_12_LostShipment_ZipLine")->Get_CombinedTransformationMatrix();
}

HRESULT CEnvironment_Interact::Find_UI_For_InteractType()
{
	// !Add UI
	switch (m_tEnvironmentDesc.eInteractType)
	{
	case Client::CEnvironment_Interact::INTERACT_VAULT100:
	case Client::CEnvironment_Interact::INTERACT_VAULT200:
	case Client::CEnvironment_Interact::INTERACT_WAGONJUMP:
		// Interaction_Icon_vault
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Vault", "Interaction_Icon_vault");
		break;
	case Client::CEnvironment_Interact::INTERACT_WAGONEVENT: // 수레 타고가는거
		break; // NO
	case Client::CEnvironment_Interact::INTEARCT_WAGONROPEJUMP: // 수레 액션
		break; // No
	case Client::CEnvironment_Interact::INTERACT_CLIMB100:
	case Client::CEnvironment_Interact::INTERACT_CLIMB200:
	case Client::CEnvironment_Interact::INTERACT_CLIMB300:
	case Client::CEnvironment_Interact::INTERACT_CLIMB450:
		// Interaction_Icon_sqeese
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Sqeese", "Interaction_Icon_sqeese");
		break;
	case Client::CEnvironment_Interact::INTERACT_WAGONPUSH:
	case Client::CEnvironment_Interact::INTERACT_LEVER:
		// Interaction_Icon_Generic
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Generic", "Interaction_Icon_Generic");
		break;
	case Client::CEnvironment_Interact::INTERACT_JUMP100:
	case Client::CEnvironment_Interact::INTERACT_JUMP200:
	case Client::CEnvironment_Interact::INTERACT_JUMP300:
	case Client::CEnvironment_Interact::INTERACT_PLANK: // 사다리 : 올라가기만함 점프 UI로 통일
	case Client::CEnvironment_Interact::INTERACT_ROPECLIMB:
		// Interaction_Icon_jupm_up
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Jump_Up", "Interaction_Icon_jupm_up");
		break;
	case Client::CEnvironment_Interact::INTERACT_SLIDE:
	case Client::CEnvironment_Interact::INTERACT_ROPEDOWN:
		// Interaction_Icon_jupm_down
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Jump_Down", "Interaction_Icon_jupm_down");
		break;
	case Client::CEnvironment_Interact::INTERACT_DOOROPEN:
		// interaction_icon_end-level
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "End_Level", "interaction_icon_end-level");
		break;
	case Client::CEnvironment_Interact::INTERACT_LADDERUP:
		// Interaction_Icon_ladder
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Ladder", "Interaction_Icon_ladder");
		break;
	case Client::CEnvironment_Interact::INTERACT_WHIPSWING:
	case Client::CEnvironment_Interact::INTERACT_WHIPPULL:
		// Interaction_Icon_Whip
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Whip", "Interaction_Icon_Whip");
		break;
	case Client::CEnvironment_Interact::INTERACT_ROTATIONVALVE:
		// Interaction_Icon_parts
		m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Parts", "Interaction_Icon_parts");
		break;

	case CEnvironment_Interact::INTERACT_ZIPLINE:
	{
		//m_pUI_Interaction = m_pUIManager->Add_Interaction(m_pGameInstance->Get_NextLevel(), "Parts", "interaction_icon_zipline");
		break;

	}
	case Client::CEnvironment_Interact::INTERACT_END:
		break;
	default:
		break;
	}

	return S_OK;
}

void CEnvironment_Interact::Move_For_PlayerRootMotion()
{
	if(m_pPlayer == nullptr || m_bInteract == false || m_pPlayer->Is_Interection() == false)
		return;

	

	_bool bMove = Check_MoveCollider();

	if (bMove == true)
	{
		_float3 vPlayerRootMotion = m_pPlayer->Get_AddRootMotion();

		vPlayerRootMotion.y = 0.f;
		vPlayerRootMotion.x = 0.f;

		m_pTransformCom->Add_RootBone_ForTarget(vPlayerRootMotion, m_pNavigationCom, m_pPlayer->Get_Transform());
	}
	else
	{
		
		switch (m_pPlayer->Get_CurrentAnimIndex())
		{
			case CPlayer::Player_State::Player_InteractionPush_Rock_Loop:
			{
				m_pPlayer->SetState_InteractionPush_End();
				m_bInteract = false;
				break;
			}
			
			case CPlayer::Player_State::Player_InteractionPull_Rock_Loop:
			{
				m_pPlayer->SetState_InteractionPull_End();
				m_bInteract = false;
				break;
			}

		}
		//m_pPlayer->Set_RootMoveRate(_float3(0.f, 0.f, 0.f));
	}
	
}

void CEnvironment_Interact::Move_For_Offset()
{
	if(m_pOwnerInteract == nullptr)
		return;

	_float4 vOwnerPosition = m_pOwnerInteract->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vCalcPosition = XMLoadFloat4(&vOwnerPosition) + XMLoadFloat4(&m_tEnvironmentDesc.vOffset);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);
}

void CEnvironment_Interact::Move_For_PlayerOffset()
{

	if (m_pPlayer == nullptr || m_bInteract == false || m_pPlayer->Is_Interection() == false)
		return;

	_bool bMove = Check_MoveCollider();

	if (bMove == true)
	{
		_float4 vOwnerPosition = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vCalcPosition = XMLoadFloat4(&vOwnerPosition) + XMLoadFloat4(&m_tEnvironmentDesc.vOffset);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);
	}
}


HRESULT CEnvironment_Interact::Find_InteractGroupObject()
{
	

	list<CGameObject*> BackGroundObjects = *m_pGameInstance->Get_GameObjects(m_iCurrentLevelIndex, L"Layer_BackGround");

	if (true == BackGroundObjects.empty())
		return E_FAIL;

	for (auto& iter : BackGroundObjects)
	{
		if (typeid(*iter) == typeid(CEnvironment_Interact))
		{
			CEnvironment_Interact* pInteractObject = dynamic_cast<CEnvironment_Interact*>(iter);

			if (pInteractObject == nullptr)
				return E_FAIL;
			else
			{
				_int iFindObjectGroupIndex = pInteractObject->Get_InteractGroupIndex();

				if (iFindObjectGroupIndex != -1 && iFindObjectGroupIndex == m_tEnvironmentDesc.iInteractGroupIndex)
				{
					m_vecInteractGroup.push_back(pInteractObject);
					//pInteractObject->Set_Interact(false); //! 처음에 충돌 되더라도 활성화시키지 않기위함
					pInteractObject->Set_OwnerObject(this); //! 자신이 활성화의 주체가 됨을 알림
				}
				else
					continue;


			}
		}
	}

	return S_OK;
}


_bool CEnvironment_Interact::ArrivalCheck()
{
	if(m_tEnvironmentDesc.bArrival == false)
		return true;

	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector		vDir = m_tEnvironmentDesc.vArrivalPosition - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance <= 0.25f)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

_bool CEnvironment_Interact::RotationCheck(const _float fTimeDelta)
{
	if (m_pOwnerInteract != nullptr)
	{
		if (m_pOwnerInteract->Is_OwnerInteract() == true)
		{
			return m_pTransformCom->Rotation_LerpAxis(XMConvertToRadians(m_tEnvironmentDesc.fRotationAngle), fTimeDelta, m_tEnvironmentDesc.eRotationState);
		}
		
		return false;
	}
	else if(m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		return m_pTransformCom->Rotation_LerpAxis(XMConvertToRadians(m_tEnvironmentDesc.fRotationAngle), fTimeDelta, m_tEnvironmentDesc.eRotationState);
	}

	return false;
}

_bool CEnvironment_Interact::Check_MoveCollider()
{
	if(m_pMoveRangeColliderCom == nullptr)
		return false;

	//!m_pMoveRangeColliderCom->Get_Bounding()
	//m_pMoveRangeColliderCom->

	_float3 vMinCorner = dynamic_cast<CBounding_AABB*>(m_pMoveRangeColliderCom->Get_Bounding())->Get_MinCorner();
	_float3 vMaxCorner = dynamic_cast<CBounding_AABB*>(m_pMoveRangeColliderCom->Get_Bounding())->Get_MaxCorner();

	_float3 vCaclPos = Get_Position() + m_pTransformCom->Get_RootBone_ForTarget(m_pPlayer->Get_AddRootMotion(), m_pPlayer->Get_Transform());
	_float3 vResult = {};

	if (vCaclPos.x < vMinCorner.x)
	{
		m_bMove = false;
		return false;
	}
	else if (vCaclPos.x > vMaxCorner.x)
	{
		m_bMove = false;
		return false;
	}
	

	if (vCaclPos.z < vMinCorner.z)
	{
		m_bMove = false;
		return false;
	}
	else if (vCaclPos.z > vMaxCorner.z)
	{
		m_bMove = false;
		return false;
	}


	m_bMove = true;
	return true;
	
}

_bool CEnvironment_Interact::Check_InteractMoveCollider()
{
	if (m_pInteractMoveColliderCom == nullptr || m_pPlayer == nullptr)
		return false;

	_float3 vPlayerPos = m_pPlayer->Get_Position();

	if (m_bTest == true)
	{
		if (true == m_pInteractMoveColliderCom->Is_Collision(m_pPlayer->Get_Collider()))
		{
			m_pPlayer->Get_Navigation()->Set_InteractMoveMode(true);
		}
		else
			m_pPlayer->Get_Navigation()->Set_InteractMoveMode(false);
	}
	else
	{
		if (true == m_pInteractMoveColliderCom->Is_Collision(m_pPlayer->Get_Collider())) //! 충돌 중이라면 플레이어  y값 고정
		{
			_float3 vMinCorner = dynamic_cast<CBounding_AABB*>(m_pInteractMoveColliderCom->Get_Bounding())->Get_MinCorner();
			_float3 vMaxCorner = dynamic_cast<CBounding_AABB*>(m_pInteractMoveColliderCom->Get_Bounding())->Get_MaxCorner();

			m_pPlayer->Get_Navigation()->Set_InteractMoveMode(true);

			if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_CLIMB200)
				vPlayerPos.y = vMinCorner.y;

			if (m_bExit == false)
			{
				if (vPlayerPos.x < vMinCorner.x)
				{
					vPlayerPos.x = vMinCorner.x;
				}
				else if (vPlayerPos.x > vMaxCorner.x)
				{
					vPlayerPos.x = vMaxCorner.x;
				}


				if (vPlayerPos.z < vMinCorner.z)
				{
					vPlayerPos.z = vMinCorner.z;
				}
				else if (vPlayerPos.z > vMaxCorner.z)
				{
					vPlayerPos.z = vMaxCorner.z;
				}
			}



			m_pPlayer->Set_Position(vPlayerPos);

		}
		else //! 충돌중이 아니라면
		{
			CNavigation* pPlayerNavigation = m_pPlayer->Get_Navigation();

			pPlayerNavigation->Set_CurrentIndex(pPlayerNavigation->Find_CurrentCellIndex(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION)));


			_float fHeight = pPlayerNavigation->Get_CellForIndex(pPlayerNavigation->Get_CurrentCellIndex())->Get_Height(m_pPlayer->Get_Position());

			_float fDiffY = vPlayerPos.y - fHeight;

			if (fDiffY > 1.f)
			{


				switch (m_tEnvironmentDesc.eInteractType)
				{
				case CEnvironment_Interact::INTERACT_VAULT200:
				{
					m_pPlayer->SetState_InteractJumpDown200();
					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP100:
				{
					m_pPlayer->SetState_InteractJumpDown100();
					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP200:
				{
					m_pPlayer->SetState_InteractJumpDown200();
					break;
				}

				case CEnvironment_Interact::INTERACT_JUMP300:
				{
					m_pPlayer->SetState_InteractJumpDown300();
					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB100:
				{
					m_pPlayer->SetState_InteractJumpDown100();
					m_pPlayer->Set_UseGravity(false);
					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB200:
				{
					m_pPlayer->SetState_InteractJumpDown200();
					m_pPlayer->Set_UseGravity(false);
					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB300:
				{
					m_pPlayer->SetState_InteractJumpDown300();
					m_pPlayer->Set_UseGravity(false);
					break;
				}

				case CEnvironment_Interact::INTERACT_CLIMB450:
				{
					m_pPlayer->SetState_InteractJumpDown300();
					m_pPlayer->Set_UseGravity(false);
					break;
				}

				}
			}

			pPlayerNavigation->Set_InteractMoveMode(false);
			m_bInteractMoveMode = false;
			m_bInteract = false;

			return true;
		}
	}

}

_bool CEnvironment_Interact::EnableCheck()
{
	_int iEnableCheckCount = (_int)m_vecEnablePosition.size();

	for (_int i = 0; i < iEnableCheckCount; ++i)
	{

		if(true == SMath::Is_InRange(m_pTransformCom->Get_Position_Float4(), m_vecEnablePosition[i], 0.5f))
			return true;
	}
	
	return false;
}



_bool CEnvironment_Interact::Check_OwnerEnablePosition()
{
	return m_pOwnerInteract->EnableCheck();
}

void CEnvironment_Interact::Stop_PlayerForArrival()
{
	if (true == ArrivalCheck())
	{

	}
	else
	{

	}
	//m_pPlayer->
}

HRESULT CEnvironment_Interact::Add_InteractGroupObject(CEnvironment_Interact* pInteractObject)
{
	if (pInteractObject == nullptr)
		return E_FAIL;

	m_vecInteractGroup.push_back(pInteractObject);

	string strModelTag;

	m_pGameInstance->WString_To_String(pInteractObject->Get_ModelTag(), strModelTag);

	m_vecInteractGroupTag.push_back(strModelTag);

	return S_OK;
}




void CEnvironment_Interact::Delete_InteractGroupObject(_int iIndex, _int* iSelectIndex)
{
	m_vecInteractGroup.erase(m_vecInteractGroup.begin() + iIndex);
	m_vecInteractGroupTag.erase(m_vecInteractGroupTag.begin() + iIndex);
	*iSelectIndex--;
}

void CEnvironment_Interact::Set_EnablePosition(vector<_float4>* EnablePositions)
{
	_int iPointSize = _int(EnablePositions->size());

	for (_int i = 0; i < iPointSize; ++i)
	{
		m_vecEnablePosition.push_back((*EnablePositions)[i]);
		m_vecPointChecks.push_back(false);
	}

}

void CEnvironment_Interact::Reset_EnablePosition()
{
	m_vecPointChecks.clear();
	m_vecEnablePosition.clear();
}

void CEnvironment_Interact::Add_UpdateCellIndex(_int iCellIndex)
{
	m_tEnvironmentDesc.vecUpdateCellIndex.push_back(iCellIndex);
}

void CEnvironment_Interact::Enable_UpdateCells()
{
	if(m_pPlayer == nullptr)
		return;
	
	CNavigation* pNavigation = m_pPlayer->Get_Navigation();

	if (pNavigation == nullptr)
		return;
	

	_int iUpdateCellCount = (_int)m_tEnvironmentDesc.vecUpdateCellIndex.size();

	for (_int i = 0; i < iUpdateCellCount; ++i)
	{
		pNavigation->Set_MoveEnableForCellIndex(m_tEnvironmentDesc.vecUpdateCellIndex[i]);
	}
}

void CEnvironment_Interact::UnEnable_UpdateCells()
{
	if (m_pPlayer == nullptr)
		return;

	CNavigation* pNavigation = m_pPlayer->Get_Navigation();

	if (pNavigation == nullptr)
		return;

	_int iUpdateCellCount = (_int)m_tEnvironmentDesc.vecUpdateCellIndex.size();

	for (_int i = 0; i < iUpdateCellCount; ++i)
	{
		pNavigation->Set_MoveUnEnableForCellIndex(m_tEnvironmentDesc.vecUpdateCellIndex[i]);
	}
}

void CEnvironment_Interact::Reset_TestEvent()
{
	m_vecPointChecks[0] = false;
	m_vecPointChecks[1] = false;
	m_vecPointChecks[2] = false;
	m_vecPointChecks[3] = false;

	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);
	
	m_vecSplinePoints.clear();
	m_iCurrentSplineIndex = 0;

	m_strCurrentSplineTrack = L"분기1지점오답";

	auto& StartIter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

	m_vecSplinePoints = StartIter->second;

	m_iDivergingCount = 4;

	Set_SplineSpeed(30.f);

	for (_int i = 0; i < m_iDivergingCount; ++i)
	{
		m_vecPointChecks.push_back(false);
	}
}

void CEnvironment_Interact::Start_Spline(vector<_float4>* SplinePoints)
{
	if (true == SplinePoints->empty() || m_bSpline == true)
		return;

	if (SplinePoints->size() % 2 != 0)
	{
		MSG_BOX("스플라인 보간은 짝수여야됨~");
		return;
	}

	
	m_InitMatrix = m_pTransformCom->Get_WorldFloat4x4();

	_vector vFirstPos, vSecondPos, vResultPos;
	_int iPointSize = _int(SplinePoints->size());
	_int iRoopCount = _int(SplinePoints->size() / 2);

	for (_int i = 0; i < iPointSize; ++i)
	{
		

		m_vecSplinePoints.push_back((*SplinePoints)[i]);
	}

	// 포인트가 6개면 루프카운트는 3
	

	//for (_int i = 0; i < iRoopCount; i++) 
	//{
	//	//! 루프카운트는 3. i는 0 
	//	
	//	vFirstPos = XMLoadFloat4(&SplinePoints[i * 2]); //! 0 2 4
	//	vSecondPos = XMLoadFloat4(&SplinePoints[i * 2 + 1]); //! 1 3 5
	//	vResultPos = XMVectorCatmullRom(vFirstPos, vFirstPos, vSecondPos, vSecondPos, m_fSplineTimeAcc);
	//	m_vecSplinePoints.push_back(vResultPos);
	//}

	m_bSpline = true;



	//TODO XMVectorCatmullRom()
	//!XMVECTOR Result;
	//!
	//!float t2 = t * t;
	//!float t3 = t2 * t;
	//!
	//!float P0 = -t3 + 2.0f * t2 - t;
	//!float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	//!float P2 = -3.0f * t3 + 4.0f * t2 + t;
	//!float P3 = t3 - t2;
	//!
	//!Result.x = (P0 * Position0.x + P1 * Position1.x + P2 * Position2.x + P3 * Position3.x) * 0.5f;
	//!Result.y = (P0 * Position0.y + P1 * Position1.y + P2 * Position2.y + P3 * Position3.y) * 0.5f;
	//!Result.z = (P0 * Position0.z + P1 * Position1.z + P2 * Position2.z + P3 * Position3.z) * 0.5f;
	//!Result.w = (P0 * Position0.w + P1 * Position1.w + P2 * Position2.w + P3 * Position3.w) * 0.5f;
	//!
	//!return Result;
	
	//TODO XMVectorCatmullRomV() 
	
	//!이 함수는 T 에서 독립적인 가중치 인자가 제공될 수 있다는 점을 제외하면 XMVectorCatmullRom 과 동일합니다 . 
	//!예를 들어, 한 2D 위치 세트에 대한 위치 벡터의 x 및 y 구성 요소와 다른 2D 위치 세트에 대한 위치 벡터의 z 및 w 구성 요소를 사용하여 두 세트의 Catmull-Rom 보간법을 계산할 수 있습니다. 
	//!2D 위치. T 의 x 및 y 구성 요소는 첫 번째 Catmull-Rom 보간에 대한 보간 요소를 결정합니다. 
	//!마찬가지로 T 의 z 및 w 구성 요소는 두 번째 Catmull-Rom 보간에 대한 보간 요소를 결정합니다.
	
	
}

void CEnvironment_Interact::Start_SplineDouble(vector<_float4>* SplinePoints)
{
	if (true == SplinePoints->empty() || m_bSpline == true)
		return;

	if (SplinePoints->size() % 4 != 0)
	{
		MSG_BOX("스플라인 보간은 4의배수여야됨~");
		return;
	}


	m_InitMatrix = m_pTransformCom->Get_WorldFloat4x4();

	_vector vFirstPos, vSecondPos, vResultPos;
	_int iPointSize = (_int)SplinePoints->size();
	_int iRoopCount = _int(SplinePoints->size() / 4);


	for (_int i = 0; i < iPointSize; ++i)
	{


		m_vecSplinePoints.push_back((*SplinePoints)[i]);
	}

	// 포인트가 6개면 루프카운트는 3


	//for (_int i = 0; i < iRoopCount; i++) 
	//{
	//	//! 루프카운트는 3. i는 0 
	//	
	//	vFirstPos = XMLoadFloat4(&SplinePoints[i * 2]); //! 0 2 4
	//	vSecondPos = XMLoadFloat4(&SplinePoints[i * 2 + 1]); //! 1 3 5
	//	vResultPos = XMVectorCatmullRom(vFirstPos, vFirstPos, vSecondPos, vSecondPos, m_fSplineTimeAcc);
	//	m_vecSplinePoints.push_back(vResultPos);
	//}

	m_bSpline = true;



	//TODO XMVectorCatmullRom()
	//!XMVECTOR Result;
	//!
	//!float t2 = t * t;
	//!float t3 = t2 * t;
	//!
	//!float P0 = -t3 + 2.0f * t2 - t;
	//!float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	//!float P2 = -3.0f * t3 + 4.0f * t2 + t;
	//!float P3 = t3 - t2;
	//!
	//!Result.x = (P0 * Position0.x + P1 * Position1.x + P2 * Position2.x + P3 * Position3.x) * 0.5f;
	//!Result.y = (P0 * Position0.y + P1 * Position1.y + P2 * Position2.y + P3 * Position3.y) * 0.5f;
	//!Result.z = (P0 * Position0.z + P1 * Position1.z + P2 * Position2.z + P3 * Position3.z) * 0.5f;
	//!Result.w = (P0 * Position0.w + P1 * Position1.w + P2 * Position2.w + P3 * Position3.w) * 0.5f;
	//!
	//!return Result;
	
	

	//TODO XMVectorCatmullRomV() 

	//!이 함수는 T 에서 독립적인 가중치 인자가 제공될 수 있다는 점을 제외하면 XMVectorCatmullRom 과 동일합니다 . 
	//!예를 들어, 한 2D 위치 세트에 대한 위치 벡터의 x 및 y 구성 요소와 다른 2D 위치 세트에 대한 위치 벡터의 z 및 w 구성 요소를 사용하여 두 세트의 Catmull-Rom 보간법을 계산할 수 있습니다. 
	//!2D 위치. T 의 x 및 y 구성 요소는 첫 번째 Catmull-Rom 보간에 대한 보간 요소를 결정합니다. 
	//!마찬가지로 T 의 z 및 w 구성 요소는 두 번째 Catmull-Rom 보간에 대한 보간 요소를 결정합니다.

}

void CEnvironment_Interact::Reset_StartMatrix()
{
	m_pTransformCom->Set_WorldMatrix(m_InitMatrix); 
	m_iCurrentSplineIndex = 0; 
	m_fSplineTimeAcc = 0.f; 
	m_vecSplinePoints.clear();
	m_bSpline = false;
}

void CEnvironment_Interact::Spline_Loop(const _float fTimeDelta)
{
	
	


	//m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed;//m_pTransformCom->Get_Speed();

	
	//
	//	if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 1)
	//	{
	//		m_bSpline = false;
	//		return;
	//	}
	//	else
	//		++m_iCurrentSplineIndex;
	//}


	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		_vector vCalcPosition = {};

		m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed;

		if (m_fSplineTimeAcc >= 1.f)
		{
			m_fSplineTimeAcc = 0.0;
			++m_iCurrentSplineIndex;
			
		}

		_float t = m_fSplineTimeAcc / m_fArrivalTime;

		t = max(0.0f, min(1.0f, t));

		if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 1)
		{
			vCalcPosition = m_vecSplinePoints[m_iCurrentSplineIndex];
			m_bSpline = false;
		}
		else
		{
			_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


			_vector vMovePoint1, vMovePoint2;
			vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
			vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
			//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, 0.f);

			
			//vCalcPosition = XMVectorCatmullRomV(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, XMVectorSet(0.1f, 0.1f, 0.1f, 1.f));
			vCalcPosition = CatmullRomInterpolation(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
			//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, t);

			
			
		}

		m_pTransformCom->Look_At_Lerp(vCalcPosition, fTimeDelta, 0.1f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);

		//if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus))
		//{
		//	m_iCurrentSplineIndex++;
		//	//m_fSplineTimeAcc = 0.f;
		//}
	}
	else
	{
		Change_WagonTrack(fTimeDelta);
	}

}

void CEnvironment_Interact::Spline_LoopDoublePoint(const _float fTimeDelta)
{
	
	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed;

		_double t = m_fSplineTimeAcc / m_fArrivalTime;

		t = max(0.0f, min(1.0f, t));

		if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 3)
		{
			m_bSpline = false;
			return;
		}

		_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		_vector vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, vCalcPosition;
		vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
		vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
		vMovePoint3 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 2]);
		vMovePoint4 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 3]);

		//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
		vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, (_float)t);
		


		if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus))
		{
			m_iCurrentSplineIndex++;
			//m_fSplineTimeAcc = 0.f;
		}
	}
	else
	{
		Change_WagonTrack(fTimeDelta);



	}
}

void CEnvironment_Interact::Spline_Clear()
{
	m_bSpline = false;
	m_vecSplinePoints.clear();
	m_fSplineTimeAcc = 0.f;
	m_fSplineEndRaidus = 1.f;
	m_iCurrentSplineIndex = 0;
}

HRESULT CEnvironment_Interact::Load_SplineJson()
{

	json SplineJson = {};
//C:\Users\PC\Desktop\3D_TeamPortpolio\Client\Bin\DataFiles\Data_Map\Spline
	string strHardPath = "../Bin/DataFiles/Data_Map/Spline/SplineData_202403280100.json";

	
	if (FAILED(CJson_Utility::Load_Json(strHardPath.c_str()/*m_tEnvironmentDesc.strSplineJsonPath.c_str()*/, SplineJson)))
	{
		MSG_BOX("스플라인 데이터 불러오기 실패");
		return E_FAIL;
	}

	_int iSplineJsonSize = _int(SplineJson.size());

	for (_int i = 0; i < iSplineJsonSize; ++i)
	{
		string strSplineKey = SplineJson[i]["SplineKey"];//(string)SplineJson[i]["SplineKey"];
		wstring strTest;

		m_pGameInstance->String_To_WString(strSplineKey, strTest);


		//m_pGameInstance->WString_To_String(strTest, strSplineKey);

		

		//strSplineKey = strSplineKey.c_str();
		
		json   SplineVectorJson = SplineJson[i]["SplineVectorJson"];
		_float fSplineSpeed = SplineJson[i]["SplineSpeed"]; //! 해당 구간 속도

		_int iSplineVectorJsonSize = _int(SplineVectorJson.size());

		vector<_float4> vecSplinePoint;

		for (_int j = 0; j < iSplineVectorJsonSize; ++j)
		{
			_float4 vSplinePoint = {};
			CJson_Utility::Load_Float4(SplineVectorJson[j]["SplinePoint"], vSplinePoint);
			vecSplinePoint.push_back(vSplinePoint);
		}

		m_mapSplineSpeeds.emplace(strTest, fSplineSpeed);
		m_mapSplineVectors.emplace(strTest,vecSplinePoint);
		
		
	}
	
	
//	m_iDivergingCount = SplineJson[0][""]




	//json

	//todo vector<_float4>						m_vecSplinePoints;
	//todo map<string, vector<_float4>>				m_mapSplineVectors;
	//todo map<string, vector<_bool>>				m_mapSplinePointChecks;
	//todo 
	//todo vector<_bool>							m_vecPointChecks;
	//todo _float								m_fSplineTimeAcc = 0.f;
	//todo _float								m_fSplineEndRaidus = 1.f;
	//todo _int								m_iCurrentSplineIndex = 0;
	//todo _float								m_fSplineSpeed = 1.f;
	//todo _float								m_fArrivalTime = 3.f; // ! 스플라인에 마지막 점까지 도착하는데 걸리는 시간.

	//m_tEnvironmentDesc.strSplineJsonPath;
	//
	//json SplineJson;
	//
	//if (FAILED(CJson_Utility::Load_Json(m_tEnvironmentDesc.strSplineJsonPath.c_str(), SplineJson)))
	//{
	//	MSG_BOX("스플라인 불러오기 실패");
	//	return E_FAIL;
	//}
	//
	//
	//_int iSplineJsonSize = SplineJson.size();
	//
	//for (_int i = 0; i < iSplineJsonSize; ++i)
	//{
	//	string strSplineKey = SplineJson[i]["SplineKey"];
	//
	//	
	//
	//
	//	_float4 vSplinePoint;
	//
	//	CJson_Utility::Load_Float4(SplineJson[i]["SplinePoint"], vSplinePoint);
	//
	//
	//	
	//
	//	//!Desc.iShaderPassIndex = BasicJson[i]["ShaderPassIndex"];
	//	//!Desc.iPlayAnimationIndex = BasicJson[i]["PlayAnimationIndex"];
	//}

	//m_pGameInstance->Load_FromJson(SplineJson);


	return S_OK;
}

HRESULT CEnvironment_Interact::Load_EnableJson()
{
	json EnablePointJson;

	if(CJson_Utility::Load_Json(m_tEnvironmentDesc.strEnableJsonPath.c_str(), EnablePointJson))
		return E_FAIL;

	_int iJsonSize = (_int)EnablePointJson.size();

	for (_int i = 0; i < iJsonSize; ++i)
	{
		_float4 vEnablePoint = {};
		CJson_Utility::Load_Float4(EnablePointJson, vEnablePoint);

		m_vecEnablePosition.push_back(vEnablePoint);
		m_vecPointChecks.push_back(false);
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Init_WagonEvent()
{
	Load_SplineJson();

	m_strCurrentSplineTrack = L"분기1지점오답";
	
	auto& StartIter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

	if (StartIter == m_mapSplineVectors.end())
		return E_FAIL;




	m_vecSplinePoints = StartIter->second;

	m_iDivergingCount = 4;

	Set_SplineSpeed(10.f);

	for (_int i = 0; i < m_iDivergingCount; ++i)
	{
		m_vecPointChecks.push_back(false);
	}

	return S_OK;
}

void CEnvironment_Interact::Change_WagonTrack(const _float fTimeDelta)
{

	m_fSplineTimeAcc += m_fSplineSpeed * fTimeDelta;

	

	_double t = m_fSplineTimeAcc / m_fArrivalTime;
	//!
	t = max(0.0f, min(1.0f, t));


	if (m_strCurrentSplineTrack == L"분기1지점오답" && m_vecPointChecks[0] == true)
	{
		_float4 vChangePoint = m_vecSplinePoints[m_iCurrentSplineIndex];

		m_vecSplinePoints.clear();

		m_vecSplinePoints.push_back(vChangePoint);
		
		m_strCurrentSplineTrack = L"분기1지점정답";

		auto& iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

		_int iNewSplineVectorSize = _int(iter->second.size());

		for (_int i = 0; i < iNewSplineVectorSize; ++i)
		{
			m_vecSplinePoints.push_back(iter->second[i]);
		}
		
		auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);
		
		Set_SplineSpeed(Speediter->second);
		
		m_iCurrentSplineIndex = 0;
		m_bSpline = true;
		m_fSplineTimeAcc = 0.f;
	}

	

	if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 3)
	{
		if (m_strCurrentSplineTrack == L"분기1지점정답")
		{
			if (m_vecPointChecks[1] == true)
			{	
				m_vecSplinePoints.clear();
				//m_strCurrentSplineTrack = L"분기2지점정답";
				m_strCurrentSplineTrack = L"분기2지점정답스피드존";
				
				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기2지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
		}
		//else if (m_strCurrentSplineTrack == L"분기2지점정답")
		else if (m_strCurrentSplineTrack == L"분기2지점정답스피드존")
		{
			
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기2지점정답스피드2";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = (_int)iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
		}
		else if (m_strCurrentSplineTrack == L"분기2지점정답스피드2")
		{
			if (m_vecPointChecks[3] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기3지점정답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기3지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(125.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기3지점정답")
		{
			if (m_vecPointChecks[3] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기4지점정답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기4지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(125.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기1지점오답" || m_strCurrentSplineTrack == L"분기2지점오답" || m_strCurrentSplineTrack == L"분기3지점오답" || m_strCurrentSplineTrack == L"분기4지점오답" || m_strCurrentSplineTrack == L"분기4지점정답")
		{
			m_bSpline = false;
			m_fSplineTimeAcc = 0.f;
		}
		return;
	}

	_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	_vector vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, vCalcPosition;
	vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
	vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
	vMovePoint3 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 2]);
	vMovePoint4 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 3]);
	
	vCalcPosition = CatmullRomInterpolation(vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, (_float)t);
	//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, t);

	if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus) )
	{
		
		m_fSplineTimeAcc = 0.0f;
		++m_iCurrentSplineIndex;
	}
	else if (m_fSplineTimeAcc >= 1.0f)
	{
		m_fSplineTimeAcc = 0.0f;
	}

	_float3 vOffsetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vOffsetPosition.y += 1.5f;

	m_pPlayer->Set_Position(vOffsetPosition);
	

}

_vector CEnvironment_Interact::CatmullRomInterpolation(_fvector p0, _fvector p1, _fvector p2, _fvector p3, _float t)
{
	 float t2 = t * t;
    float t3 = t * t * t;
    XMFLOAT3 result;

	_float3 ps0, ps1, ps2, ps3;

	XMStoreFloat3(&ps0, p0);
	XMStoreFloat3(&ps1, p1);
	XMStoreFloat3(&ps2, p2);
	XMStoreFloat3(&ps3, p3);

    result.x = 0.5f * ((2.0f * ps1.x) +
                        (-ps0.x + ps2.x) * t +
                        (2.0f * ps0.x - 5.0f * ps1.x + 4.0f * ps2.x - ps3.x) * t2 +
                        (-ps0.x + 3.0f * ps1.x - 3.0f * ps2.x + ps3.x) * t3);

    result.y = 0.5f * ((2.0f * ps1.y) +
                        (-ps0.y + ps2.y) * t +
                        (2.0f * ps0.y - 5.0f * ps1.y + 4.0f * ps2.y - ps3.y) * t2 +
                        (-ps0.y + 3.0f * ps1.y - 3.0f * ps2.y + ps3.y) * t3);

    result.z = 0.5f * ((2.0f * ps1.z) +
                        (-ps0.z + ps2.z) * t +
                        (2.0f * ps0.z - 5.0f * ps1.z + 4.0f * ps2.z - ps3.z) * t2 +
                        (-ps0.z + 3.0f * ps1.z - 3.0f * ps2.z + ps3.z) * t3);

    return XMVectorSet(result.x, result.y, result.z, 1.f);
}

vector<_float4> CEnvironment_Interact::CreateSmoothSpline(vector<_float4>& points, _int segments)
{
	vector<_float4> splinePoints;

	for (int i = 0; i < points.size() - 3; ++i) {
		for (int j = 0; j <= segments; ++j) {
			float t = static_cast<float>(j) / segments;
			_float4 p = CatmullRomInterpolation(points[i], points[i + 1], points[i + 2], points[i + 3], t);
			splinePoints.push_back(p);
		}
	}

	return splinePoints;
}

HRESULT CEnvironment_Interact::Ready_Components()
{
	
	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	else
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	

	//if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONPUSH)
	//{
	//	/* For.Com_Model */
	//	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Navigation"),
	//		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
	//		return E_FAIL;
	//
	//	
	//}


	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, m_tEnvironmentDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	if (m_tEnvironmentDesc.eInteractType != CEnvironment_Interact::INTERACT_NONE)
	{
		if (FAILED(Ready_InteractCollider(m_tEnvironmentDesc.eInteractType)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Ready_InteractCollider(INTERACT_TYPE eInteractType)
{
	

	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	/* For.Com_Collider */
	
	if (m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONPUSH)
	{
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
		//BoundingDesc.iLayer = ECast(COLLISION_LAYER::INTERACT);
		BoundingDesc.vExtents = m_tEnvironmentDesc.vBodyColliderSize;
		BoundingDesc.vCenter = m_tEnvironmentDesc.vBodyColliderCenter;
	
	
		if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
			return E_FAIL;
	}
	

	//CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	/* For.Com_Collider */
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
	BoundingDesc.vExtents = m_tEnvironmentDesc.vInteractColliderSize;
	BoundingDesc.vCenter = m_tEnvironmentDesc.vInteractColliderCenter;

	
	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_InteractCollider"), reinterpret_cast<CComponent**>(&m_pInteractColliderCom), &BoundingDesc)))
		return E_FAIL;

	
	/* For.Com_Collider */
	BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
	BoundingDesc.vExtents = m_tEnvironmentDesc.vMoveRangeColliderSize;
	BoundingDesc.vCenter = m_tEnvironmentDesc.vMoveRangeColliderCenter;


	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_MoveCollider"), reinterpret_cast<CComponent**>(&m_pMoveRangeColliderCom), &BoundingDesc)))
		return E_FAIL;


	/* For.Com_Collider */
	BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
	BoundingDesc.vExtents = m_tEnvironmentDesc.vInteractMoveColliderSize;
	BoundingDesc.vCenter = m_tEnvironmentDesc.vInteractMoveColliderCenter;


	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_InteractMoveCollider"), reinterpret_cast<CComponent**>(&m_pInteractMoveColliderCom), &BoundingDesc)))
		return E_FAIL;
	
	
	return S_OK;
}

HRESULT CEnvironment_Interact::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	m_gCamFar = m_pGameInstance->Get_CamFar();
	m_pShaderCom->Bind_RawValue("g_fCamFar", &m_gCamFar, sizeof(_float));

	return S_OK;
}

HRESULT CEnvironment_Interact::Classification_Model()
{
	/* 추후 승용 일어나면 그룹화로 수정할곳 지금당장은 모델태그로 만족 */
	wstring strTemp = Get_ModelTag();

	if (TEXT("Prototype_Component_Model_ChainBeam1") == strTemp ||
		TEXT("Prototype_Component_Model_ChainClimbLadder3") == strTemp)
	{
		m_bRenderOutLine = true;
		m_vChainMesh.push_back(1);
		m_vChainMesh.push_back(2);
		m_vLineColor = { 1.f, 1.f, 1.f, 1.f };
		m_fLineThick = 0.3f;
	}
	else if (TEXT("Prototype_Component_Model_ChainBeam2") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainBeam3") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainBeam4") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainBeam5") == strTemp||
			 TEXT("Prototype_Component_Model_ChainClimb1") == strTemp||
			 TEXT("Prototype_Component_Model_ChainHook1") == strTemp||
			 TEXT("Prototype_Component_Model_ChainHookRound1") == strTemp||
			 TEXT("Prototype_Component_Model_ChainInteraction1") == strTemp||
			 TEXT("Prototype_Component_Model_ChainInteraction2") == strTemp||
			 TEXT("Prototype_Component_Model_ChainJumpDown1") == strTemp||
			 TEXT("Prototype_Component_Model_ChainLadder1") == strTemp||
			 TEXT("Prototype_Component_Model_ChainLadder2") == strTemp||
			 TEXT("Prototype_Component_Model_ChainMod1") == strTemp )
	{
		m_bRenderOutLine = true;
		m_vChainMesh.push_back(0);
		m_vLineColor = { 1.f, 1.f, 1.f, 1.f };
		m_fLineThick = 0.3f;
	}
	else if (TEXT("Prototype_Component_Model_ChainClimbLadder1") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainClimbLadder2") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainClimbLadder4") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainClimbLadderTopPlank1") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainClimbLadderTopPlank2") == strTemp )
	{
		m_bRenderOutLine = true;
		m_vChainMesh.push_back(1);
		m_vLineColor = { 1.f, 1.f, 1.f, 1.f };
		m_fLineThick = 0.3f;
	}
	else if (TEXT("Prototype_Component_Model_ChainClimbLadder5") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainClimbLadder2") == strTemp ||
			 TEXT("Prototype_Component_Model_ChainClimbLadder4") == strTemp )
	{
		m_bRenderOutLine = true;
		m_vChainMesh.push_back(2);
		m_vLineColor = { 1.f, 1.f, 1.f, 1.f };
		m_fLineThick = 0.3f;
	}
	else
		m_bRenderOutLine = false;

	return S_OK;
}

CEnvironment_Interact * CEnvironment_Interact::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CEnvironment_Interact*		pInstance = new CEnvironment_Interact(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_Interact");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnvironment_Interact::Clone(void* pArg)
{
	CEnvironment_Interact*		pInstance = new CEnvironment_Interact(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_Interact");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_Interact::Pool()
{
	return new CEnvironment_Interact(*this);
}

void CEnvironment_Interact::Free()
{
	__super::Free();
	
	if(m_pPlayer != nullptr)
		Safe_Release(m_pPlayer);

	if(m_pUI_Interaction != nullptr)
		Safe_Release(m_pUI_Interaction);

	if (m_pUIManager != nullptr)
		Safe_Release(m_pUIManager);



	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
}



