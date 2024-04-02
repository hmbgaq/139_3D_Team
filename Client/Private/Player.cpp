#include "Player.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon_Player.h"
#include "Player_IdleLoop.h"
#include "Data_Manager.h"
// Add_UIManager
#include "UI_Manager.h"

#include "Player_HitNormal_B.h"
#include "Player_HitNormal_F.h"
#include "Player_HitNormal_L.h"
#include "Player_HitNormal_R.h"
#include "Player_HitPeriodic_01.h"
#include "Player_HitPeriodic_03.h"
#include "Player_HitPeriodic_04.h"
#include "Player_KnockFrontLight_F_02.h"
#include "Player_HitHeavy_F_5m.h"

#include "Player_DeathLight_F_01.h"
#include "Player_DeathLight_F_02.h"
#include "Player_DeathNormal_F_01.h"
#include "Player_DeathNormal_F_02.h"


#include "Player_InteractionJumpDown100.h"
#include "Player_InteractionJumpDown200.h"
#include "Player_InteractionJumpDown300.h"

#include "Player_InteractionVault100.h"
#include "Player_InteractionVault200.h"

#include "Player_CartRide_Start.h"
#include "Player_L06_Wagon_Jump.h"
#include "Player_CartRide_Loop.h"

#include "Player_InteractionPull_Rock_Idle.h"
#include "Player_InteractionPush_Rock_Idle.h"



#include "Player_InteractionClimb100.h"
#include "Player_InteractionClimb200.h"
#include "Player_InteractionClimb300.h"
#include "Player_InteractionClimb450.h"

#include "Player_Interaction_Slide_Sequence.h"
#include "Player_Interaction_SmallLever.h"

#include "Player_InteractionPlank_Up_Start.h"
#include "Player_InteractionLadder_Up_Start.h"

#include "Player_InteractionWhipSwing.h"
#include "Player_InteractionWhipPull.h"

#include "Player_InteractionRotateValve_01.h"

#include "Player_InteractionDoor_Open.h"
#include "Player_InteractionDoorPush.h"

#include "Player_InteractionClimbRope_Start.h"
#include "Player_InteractionRope_Down_Start.h"


#include "PhysXCharacterController.h"
#include "PhysXCollider.h"
#include "Preset_PhysXColliderDesc.h"




CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter_Client(pDevice, pContext, strPrototypeTag)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter_Client(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 7.f;
	//GameObjectDesc.fSpeedPerSec = 22.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));

	m_fHp = 100;

// 	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
// 	{
		m_pActor = new CActor<CPlayer>(this);
		m_pActor->Set_State(new CPlayer_IdleLoop());
		//m_pActor->Set_State(new CPlayer_CartRide_Loop());

// 	}


	m_pRigidBody->Set_UseGravity(true);

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();


	CData_Manager::GetInstance()->Set_Player(this);
	m_pGameInstance->Set_Player(this);


	///* For.Com_PhysXController */
	//FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_PhysXController"), TEXT("Com_PhysXController"), reinterpret_cast<CComponent**>(&m_pPhysXControllerCom)));
	//m_pPhysXControllerCom->Init_Controller(Preset::PhysXControllerDesc::PlayerSetting(m_pTransformCom), (_uint)PHYSX_COLLISION_LAYER::PLAYER);
	////m_pPhysXControllerCom->

	///* For.Com_PhysXCollider */
	//FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhysXCollider"), reinterpret_cast<CComponent**>(&m_pPhysXCollider)));
	//

	//CPhysXCollider::PhysXColliderDesc tPhysXColliderDesc;
	//Preset::PhysXColliderDesc::GroundSetting(tPhysXColliderDesc, m_pTransformCom);
	//m_pPhysXCollider->CreatePhysXActor(tPhysXColliderDesc);
	//m_pPhysXCollider->Add_PhysXActorAtScene();

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	//m_pGameInstance->Set_Player(this);

	__super::Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/* 성희임시추가 : UI창 껐다,켰다 하는 Key (옵션창, 스킬창 등등) => GamePlay상태든 UI상태든 입력이 가능해서 밖에 뺐음. => 알맞은 곳에 넣어주세요 */
	KeyInput(fTimeDelta);
	
	if (GAME_STATE::GAMEPLAY == m_pDataManager->Get_GameState())
	{
		if (m_pActor)
		{
			m_pActor->Update_State(fTimeDelta);
		}
		
		Update_ChargingTime(fTimeDelta);

	CData_Manager::GetInstance()->Set_CurHP(m_fHp);

		//if (m_pGameInstance->Key_Down(DIK_C))
		//	m_fHp = 100;
	}


	_bool bIsNotIdle = m_pBody->Get_CurrentAnimIndex() != ECast(Player_State::Player_IdleLoop);
	m_pDataManager->Set_ShowInterface(bIsNotIdle);
	
	

	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->Update(XMMatrixIdentity());

	




	//_float3 vPos = Get_Position();

	//PxControllerFilters Filters;
	//
	//m_pPhysXControllerCom->Synchronize_Controller(m_pTransformCom, fTimeDelta, Filters);

	//m_LastCollisionFlags = m_pPhysXControllerCom->MoveGravity(fTimeDelta, Filters);

	//_vector vPxPos = m_pPhysXControllerCom->Get_Position();
	//_float3 vResult;
	//XMStoreFloat3(&vResult, vPxPos);

	//if (m_LastCollisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
	//{
	//	_int a = 0;
	//	//m_pPhysXControllerCom->Reset_Gravity();
	//}

	//else if (vPos.y < 0)
	//{
	//	_int a = 0;
	//}

	//else 
	//{
	//	Set_Position(vResult);
	//}
	


}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	#ifdef _DEBUG
	
		//if(m_pNavigationCom != nullptr)
        //m_pGameInstance->Add_DebugRender(m_pNavigationCom);
    #endif // _DEBUG
}
	
HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Set_Navigation(CNavigation* pNavigation)
{
	if(m_pNavigationCom != nullptr)
		Safe_Release(m_pNavigationCom);

	m_pNavigationCom = pNavigation;
	m_pNavigationCom->Set_CurrentIndex(m_pNavigationCom->Get_SelectRangeCellIndex(this));
	Safe_AddRef(pNavigation);

	
}

void CPlayer::Aim_Walk(_float fTimeDelta)
{
	_uint AnimIndex;
	//_uint ReversedAnimIndex;


	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		//if (m_pGameInstance->Key_Pressing(DIK_A))
		//{
		//	AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FL45);
		//	if (Get_CurrentAnimIndex() != AnimIndex)
		//	{
		//		m_pBody->Set_RotateUpperX(MoveDirection::FrontLeft);
		//		Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		//	}
		//	Go_Straight_L45(fTimeDelta * 0.5f);
		//	
		//}
		//else if (m_pGameInstance->Key_Pressing(DIK_D))
		//{
		//	AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FR45);
		//	if (Get_CurrentAnimIndex() != AnimIndex)
		//	{
		//		m_pBody->Set_RotateUpperX(MoveDirection::FrontRight);
		//		Set_Animation(AnimwwwddwIndex, CModel::ANIM_STATE_LOOP, true, false);
		//	}
		//	Go_Straight_R45(fTimeDelta * 0.5f);
		//	
		//}
		//else
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_F);
			//AnimIndex = ECast(CPlayer::Player_State::Player_Bandit_WalkAim_F);

			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				m_pBody->Set_RotateUpperX(MoveDirection::Front);
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Straight(fTimeDelta * 0.5f);
			
		}
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		//if (m_pGameInstance->Key_Pressing(DIK_A))
		//{
		//	AnimIndex = ECast(CPlayer::Player_State::Player_Walk_BL135);
		//	if (Get_CurrentAnimIndex() != AnimIndex)
		//	{
		//		m_pBody->Set_RotateUpperX(MoveDirection::BackLeft);
		//		Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		//	}
		//	Go_Backward_L45(fTimeDelta * 0.5f);
		//	
		//}
		//else if (m_pGameInstance->Key_Pressing(DIK_D))
		//{
		//	AnimIndex = ECast(CPlayer::Player_State::Player_Walk_BR135);
		//	if (Get_CurrentAnimIndex() != AnimIndex)
		//	{
		//		m_pBody->Set_RotateUpperX(MoveDirection::BackRight);
		//		Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		//	}
		//	Go_Backward_R45(fTimeDelta * 0.5f);
		//	
		//}
		//else
		{
			AnimIndex = ECast(CPlayer::Player_State::Player_Walk_B);
			//AnimIndex = ECast(CPlayer::Player_State::Player_Bandit_WalkAim_B);
			if (Get_CurrentAnimIndex() != AnimIndex)
			{
				m_pBody->Set_RotateUpperX(MoveDirection::Back);
				Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			}
			Go_Backward(fTimeDelta * 0.5f);
			
		}
	}
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FL);
		//AnimIndex = ECast(CPlayer::Player_State::Player_Bandit_WalkAim_FL);
		//ReversedAnimIndex = ECast(CPlayer::Player_State::Player_Bandit_WalkAim_FR);
		//if (Get_CurrentAnimIndex() != AnimIndex || ReversedAnimIndex == AnimIndex)
		//{
		//	Set_Animation(ReversedAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		//}
		if (Get_CurrentAnimIndex() != AnimIndex)
		{
			m_pBody->Set_RotateUpperX(MoveDirection::Left);
			Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		}
		Go_Left(fTimeDelta * 0.5f);
		
	}
	else if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		AnimIndex = ECast(CPlayer::Player_State::Player_Walk_FR);
		//AnimIndex = ECast(CPlayer::Player_State::Player_Bandit_WalkAim_FR);
		//ReversedAnimIndex = ECast(CPlayer::Player_State::Player_Bandit_WalkAim_FL);
		//if (Get_CurrentAnimIndex() != AnimIndex || ReversedAnimIndex == AnimIndex)
		//{
		//	Set_Animation(ReversedAnimIndex, CModel::ANIM_STATE_LOOP_REVERSE, true, false);
		//}
		if (Get_CurrentAnimIndex() != AnimIndex)
		{
			m_pBody->Set_RotateUpperX(MoveDirection::Right);
			Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
		}
		Go_Right(fTimeDelta * 0.5f);
		
	}
	else
	{
		AnimIndex = ECast(CPlayer::Player_State::Player_IdleLoop);
		if (Get_CurrentAnimIndex() != AnimIndex)
		{
			m_pBody->Set_RotateUpperX(MoveDirection::Front);
			Set_Animation(AnimIndex, CModel::ANIM_STATE_LOOP, true, false);
			
		}
	}
}

void CPlayer::Activate_ShootingReaction(_float fHeight)
{
	m_pBody->Activate_ShootingReaction(fHeight);
}


#pragma region 상호작용

void CPlayer::SetState_InteractJumpDown100()
{
	m_pActor->Set_State(new CPlayer_InteractionJumpDown100());
}

void CPlayer::SetState_InteractJumpDown200()
{
	m_pActor->Set_State(new CPlayer_InteractionJumpDown200());
}

void CPlayer::SetState_InteractJumpDown300()
{
	m_pActor->Set_State(new CPlayer_InteractionJumpDown300());
}

void CPlayer::SetState_InteractVault100()
{
	m_pActor->Set_State(new CPlayer_InteractionVault100());
}

void CPlayer::SetState_InteractVault200()
{
	m_pActor->Set_State(new CPlayer_InteractionVault200());
}

void CPlayer::SetState_InteractCartRideStart()
{
	m_pActor->Set_State(new CPlayer_CartRide_Start());
}

void CPlayer::SetState_InteractCartRideLoop()
{
	m_pActor->Set_State(new CPlayer_CartRide_Loop());
}

void CPlayer::SetState_InteractCartRideWagonJump()
{
	m_pActor->Set_State(new CPlayer_L06_Wagon_Jump());
}


void CPlayer::SetState_InteractionPush_Rock_Idle()
{
	m_pActor->Set_State(new CPlayer_InteractionPush_Rock_Idle());
}

void CPlayer::SetState_InteractionPull_Rock_Idle()
{
	m_pActor->Set_State(new CPlayer_InteractionPull_Rock_Idle());
}

void CPlayer::SetState_InteractClimb100()
{
	m_pActor->Set_State(new CPlayer_InteractionClimb100());
}

void CPlayer::SetState_InteractClimb200()
{
	m_pActor->Set_State(new CPlayer_InteractionClimb200());
}

void CPlayer::SetState_InteractClimb300()
{
	m_pActor->Set_State(new CPlayer_InteractionClimb300());
}

void CPlayer::SetState_InteractClimb450()
{
	m_pActor->Set_State(new CPlayer_InteractionClimb450());
}

void CPlayer::SetState_InteractSlide()
{
	m_pActor->Set_State(new CPlayer_Interaction_Slide_Sequence());
}

void CPlayer::SetState_InteractSmallLever()
{
	m_pActor->Set_State(new CPlayer_Interaction_SmallLever());
}

void CPlayer::SetState_InteractPlankStart()
{
	m_pActor->Set_State(new CPlayer_InteractionPlank_Up_Start());
}

void CPlayer::SetState_InteractRopeDown()
{
	m_pActor->Set_State(new CPlayer_InteractionRope_Down_Start());
}

void CPlayer::SetState_InteractClimbRope()
{
	m_pActor->Set_State(new CPlayer_InteractionClimbRope_Start());
}


void CPlayer::SetState_InteractDoorOpen()
{
	m_pActor->Set_State(new CPlayer_InteractionDoor_Open());
}

void CPlayer::SetState_InteractDoorPush()
{
	m_pActor->Set_State(new CPlayer_InteractionDoorPush());
}

void CPlayer::SetState_InteractLadderUpStart()
{
	m_pActor->Set_State(new CPlayer_InteractionLadder_Up_Start());
}

void CPlayer::SetState_InteractWhipSwing()
{
	m_pActor->Set_State(new CPlayer_InteractionWhipSwing());
}

void CPlayer::SetState_InteractWhipPull()
{
	m_pActor->Set_State(new CPlayer_InteractionWhipPull());
}

void CPlayer::SetState_InteractRotationValve()
{
	m_pActor->Set_State(new CPlayer_InteractionRotateValve_01());
}


#pragma endregion 상호작용

void CPlayer::Search_Target(_float fMaxDistance)
{
	__super::Search_Target(LAYER_MONSTER, fMaxDistance);
	__super::Search_Target(LAYER_BOSS, fMaxDistance);
}

void CPlayer::Chasing_Attack(_float fTimeDelta, _float fMaxDistance, _uint iCount)
{
	if (nullptr == m_pTarget || 0 >= m_pTarget->Get_Hp() || true == m_pTarget->Is_Dead() || false == m_pTarget->Get_Enable())
	{
		Search_Target(fMaxDistance);
	}

	//Search_Target(fMaxDistance);

	if (m_pTarget)
	{
		Look_At_Target();
		for (_uint i = 0; i < iCount; ++i) 
		{
			Move_In_Proportion_To_Enemy(fTimeDelta);
		}
	}
}

void CPlayer::KeyInput(_float fTimeDelta)
{
	/* ! UI : ShaderOption Window / Key : Esc */
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		m_bShowOption = !m_bShowOption;
	
		if (m_bShowOption == true)
		{
			m_pUIManager->Active_Option();
			m_pUIManager->Active_MouseCursor();
			m_pDataManager->Set_GameState(GAME_STATE::UI);
		}
		else
		{
			m_pUIManager->NonActive_Option();
			m_pUIManager->Active_MouseCursor();
			m_pDataManager->Set_GameState(GAME_STATE::GAMEPLAY);
		}
	}

	/* ! UI : DiedScreen / Key : I */
	if (m_pGameInstance->Key_Down(DIK_I))
	{
		m_bShowDiedScreen = !m_bShowDiedScreen;
	
		if (m_bShowDiedScreen == true)
		{
			m_pUIManager->Active_DiedScreen();
			m_pDataManager->Set_GameState(GAME_STATE::UI);
		}
		else
		{
			m_pUIManager->NonActive_DiedScreen();
			m_pDataManager->Set_GameState(GAME_STATE::GAMEPLAY);
		}
	}

	/* ! UI : DiedScreen / Key : I */
	if (m_pGameInstance->Key_Down(DIK_B))
	{
		m_bShowDiedScreen = !m_bShowDiedScreen;

		if (m_bShowDiedScreen == true)
		{
			m_pUIManager->Active_DiedScreen();
			m_pDataManager->Set_GameState(GAME_STATE::UI);
		}
		else
		{
			m_pUIManager->NonActive_DiedScreen();
			m_pDataManager->Set_GameState(GAME_STATE::GAMEPLAY);
		}
	}

	/* ! UI : SkillWindow / Key : K (!아직 UI 안넣음) */
	if (m_pGameInstance->Key_Down(DIK_K))
	{
		//m_bShowSkillWindow = !m_bShowSkillWindow;

		//if (m_bShowSkillWindow == true)
		//	m_pUIManager->Active_SkillWindow();
		//else
		//	m_pUIManager->NonActive_SkillWindwo();
	}
}

HRESULT CPlayer::Ready_Components()
{
	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Player"), BodyDesc));

	//if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL_TOOL))
	//{
		
	CWeapon::WEAPON_DESC		WeaponDesc = {};
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Punch"), "LeftHandIK", WeaponDesc,				PLAYER_WEAPON_PUNCH_L));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Punch"), "RightHandIK", WeaponDesc,			PLAYER_WEAPON_PUNCH_R));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_ELWinchester"), "RightHandIK", WeaponDesc,		PLAYER_WEAPON_WINCHESTER));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Revolver"), "RightInHandIndex", WeaponDesc,	PLAYER_WEAPON_REVOLVER));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_ELShotgun"), "RightInHandIndex", WeaponDesc,	PLAYER_WEAPON_SHOTGUN));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Dynamite"), "RightInHandIndex", WeaponDesc,	PLAYER_WEAPON_DYNAMITE));


	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Kick"), "RightFoot", WeaponDesc,			PLAYER_WEAPON_KICK));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Zapper"), "LeftHandIK", WeaponDesc,		PLAYER_WEAPON_ZAPPER));

	
	Set_Weapons_Enable_False();

	//CWeapon* m_pWeapon_Punch_L = Get_Weapon(PLAYER_WEAPON_PUNCH_L);
	//m_pWeapon_Punch_L->Set_Enable(false);
	//
	//CWeapon* m_pWeapon_Punch_R = Get_Weapon(PLAYER_WEAPON_PUNCH_R);
	//m_pWeapon_Punch_R->Set_Enable(false);
	//
	//CWeapon* m_pWeapon_Winchester = Get_Weapon(PLAYER_WEAPON_WINCHESTER);
	//m_pWeapon_Winchester->Set_Enable(false);

	//CWeapon* m_pWeapon_Revolver = Get_Weapon(PLAYER_WEAPON_REVOLVER);
	//m_pWeapon_Revolver->Set_Enable(false);

	//CWeapon* m_pWeapon_Shotgun = Get_Weapon(PLAYER_WEAPON_SHOTGUN);
	//m_pWeapon_Shotgun->Set_Enable(false);

	//CWeapon* m_pWeapon_Kick = Get_Weapon(PLAYER_WEAPON_KICK);
	//m_pWeapon_Kick->Set_Enable(false);

	//CWeapon* m_pWeapon_Zapper = Get_Weapon(PLAYER_WEAPON_ZAPPER);
	//m_pWeapon_Zapper->Set_Enable(false);

	
	
	return S_OK;
}

void CPlayer::Check_Frustum()
{
	m_bIsInFrustum = true;
}

void CPlayer::Update_ChargingTime(_float fTimeDelta)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_LB)) 
	{
		m_fChargingTime += fTimeDelta;
	}
	else 
	{
		m_fChargingTime = 0.f;
	}
}

CGameObject* CPlayer::Slam()
{
	CGameObject* pSlam = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Impact_Slam");
	pSlam->Set_Position(Get_Position());
	
	
	return nullptr;
}

void CPlayer::Hitted_Left(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CPlayer_HitNormal_L());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CPlayer_HitNormal_L());
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CPlayer_HitHeavy_F_5m());
		break;
	default:
		m_pActor->Set_State(new CPlayer_HitNormal_L());
		break;
	}
}

void CPlayer::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CPlayer_HitNormal_R());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CPlayer_HitNormal_R());
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CPlayer_HitHeavy_F_5m());
		break;
	default:
		m_pActor->Set_State(new CPlayer_HitNormal_R());
		break;
	}
}

void CPlayer::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CPlayer_HitNormal_F());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CPlayer_HitNormal_F());
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CPlayer_HitHeavy_F_5m());
		break;
	default:
		m_pActor->Set_State(new CPlayer_HitNormal_F());
		break;
	}
}

void CPlayer::Hitted_Knock(_bool bIsCannonball)
{
	//if (bIsCannonball)
	//{
	//	m_pActor->Set_State(new CInfected_KnockFrontCannonball_F_01_TEMP());
	//}
	//else
	//{
	//	m_pActor->Set_State(new CInfected_KnockFrontLight_F_01_NEW());
	//}

	m_pActor->Set_State(new CPlayer_KnockFrontLight_F_02());
}

void CPlayer::Hitted_Dead(Power ePower)
{

	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CPlayer_DeathLight_F_01());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CPlayer_DeathNormal_F_01());
		break;

	default:
		break;
	}
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Pool()
{
	return new CPlayer(*this);
}

void CPlayer::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}


	
}
