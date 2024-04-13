#include "Player.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon_Player.h"
#include "Player_IdleLoop.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"

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

#include "Player_InteractionPush_Rock_End.h"
#include "Player_InteractionPull_Rock_End.h"



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
#include "Player_ZipLine_Start.h"
#include "Player_CrouchUnder_Start.h"
#include "Player_CrouchUnder_Gate.h"

#include "Environment_Interact.h"
#include "Player_ZipLine_Loop.h"

#include "PhysXCharacterController.h"
#include "PhysXCollider.h"
#include "Preset_PhysXColliderDesc.h"


#include "Effect.h"
#include "Effect_Manager.h"


//!For Debug
#include "Level_Loading.h"

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

	//GameObjectDesc.fSpeedPerSec = 7.f;
	GameObjectDesc.fSpeedPerSec = 22.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));

	m_fHp = 100;
	m_fMaxHp = m_fHp;



// 	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
// 	{
		m_pActor = new CActor<CPlayer>(this);
		m_pActor->Set_State(new CPlayer_IdleLoop());
		//m_pActor->Set_State(new CPlayer_CartRide_Loop());

// 	}


	m_pRigidBody->Set_UseGravity(true);

	//_uint iNextLevel = m_pGameInstance->Get_NextLevel();


	CData_Manager::GetInstance()->Set_Player(this);
	m_pGameInstance->Set_Player(this);

	Set_HUD_MaxCooltime(HUD::LEFT_TOP,		SUPER_CHARGE_MAXCOOLTIME);		//슈퍼차지
	Set_HUD_MaxCooltime(HUD::LEFT_RIGHT,	HEAL_MAXCOOLTIME);		//힐
	Set_HUD_MaxCooltime(HUD::LEFT_BOTTOM,	REVOLVER_MAXCOOLTIME);		//리볼버
	Set_HUD_MaxCooltime(HUD::LEFT_LEFT,		SHOTGUN_MAXCOOLTIME);		//샷건

	Set_HUD_MaxCooltime(HUD::RIGHT_TOP,		RIFLE_MAXCOOLTIME);	//라이플
	Set_HUD_MaxCooltime(HUD::RIGHT_RIGHT,	SLAM_MAXCOOLTIME);	//내려찍기
	Set_HUD_MaxCooltime(HUD::RIGHT_BOTTOM,	KICK_MAXCOOLTIME);	//발차기
	Set_HUD_MaxCooltime(HUD::RIGHT_LEFT,	WHIP_MAXCOOLTIME);	//전기 줄

	//m_pUIManager->Change_LeftHUD_MaxCoolTime("LeftHUD_Top", 5.f);
	//m_pUIManager->Change_LeftHUD_MaxCoolTime("LeftHUD_Right", 5.f);
	//m_pUIManager->Change_LeftHUD_MaxCoolTime("LeftHUD_Bottom", 5.f);
	//m_pUIManager->Change_LeftHUD_MaxCoolTime("LeftHUD_Left", 5.f);

	//m_pUIManager->Change_RightHUD_MaxCoolTime("RightHUD_Top", 5.f);
	//m_pUIManager->Change_RightHUD_MaxCoolTime("RightHUD_Right", 5.f);
	//m_pUIManager->Change_RightHUD_MaxCoolTime("RightHUD_Bottom", 5.f);
	//m_pUIManager->Change_RightHUD_MaxCoolTime("RightHUD_Left", 5.f);
	

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	//m_pGameInstance->Set_Player(this);

	__super::Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	//!if (m_pGameInstance->Key_Down(DIK_H))
	//!{
	//!	if (GAME_STATE::UI == m_pDataManager->Get_GameState())
	//!	{
	//!		m_pDataManager->Set_GameState(GAME_STATE::GAMEPLAY);
	//!	}
	//!	else if (GAME_STATE::GAMEPLAY == m_pDataManager->Get_GameState())
	//!	{
	//!		m_pDataManager->Set_GameState(GAME_STATE::UI);
	//!	}
	//!}
	
	//! 유정 테스트 공간 시작
	if (m_bfirstcheck)
	{
		//EFFECT_MANAGER->Play_Effect("Parasiter/MotherShakeTree/", "Circle_Floor_05.json", this, _float3(m_pTransformCom->Get_Position().x, 0.1f, m_pTransformCom->Get_Position().z));
		//EFFECT_MANAGER->Play_Effect("Parasiter/MotherShakeTree/", "Circle_Floor_05.json", nullptr, _float3(m_pTransformCom->Get_Position().x + 2.f, 0.1f, m_pTransformCom->Get_Position().z + 2.f));
		
		//EFFECT_MANAGER->Play_Effect("Player/TeleportPunch/", "TeleportPunch_01.json", this, true, "Head");
		m_bfirstcheck = false;
	}
	//! 유정 테스트 공간 끝

	if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		m_pActor->Set_State(new CPlayer_IdleLoop());
	}

	/* 성희추가 : UI창 껐다,켰다 하는 Key (옵션창, 스킬창 등등) => GamePlay상태든 UI상태든 입력이 가능해야해서 밖에 뺐음. */
	if (m_pGameInstance->Get_NextLevel() != LEVEL::LEVEL_TOOL)
		KeyInput(fTimeDelta);

	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);


	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}
		
	Update_ChargingTime(fTimeDelta);

	Update_SuperCharge(fTimeDelta);

	m_pDataManager->Set_CurHP(m_fHp);



	if (m_pGameInstance->Key_Down(DIK_T))
	{
		Teleport();
	}


	//if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	//{
	//	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_INTRO_BOSS));
	//}


	_bool bIsNotIdle = (m_pBody->Get_CurrentAnimIndex() != ECast(Player_State::Player_IdleLoop) || (false == Is_Splitted()));
	
	if(m_pDataManager->Get_GameState() == GAME_STATE::GAMEPLAY)
		m_pDataManager->Set_ShowInterface(bIsNotIdle);
	

	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->Update(XMMatrixIdentity());

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
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
	m_pNavigationCom->Set_CurrentIndex(m_pNavigationCom->Find_CurrentCellIndex(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION)));
	//Set_InitPosition(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));
	
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

string CPlayer::Get_HUD_Tag(HUD eHUD)
{
	string strHUDTag = "";

	switch (eHUD)
	{
	case Client::CPlayer::HUD::LEFT_TOP:
		strHUDTag = "LeftHUD_Top";
		break;
	case Client::CPlayer::HUD::LEFT_RIGHT:
		strHUDTag = "LeftHUD_Right";
		break;
	case Client::CPlayer::HUD::LEFT_BOTTOM:
		strHUDTag = "LeftHUD_Bottom";
		break;
	case Client::CPlayer::HUD::LEFT_LEFT:
		strHUDTag = "LeftHUD_Left";
		break;
	case Client::CPlayer::HUD::RIGHT_TOP:
		strHUDTag = "RightHUD_Top";
		break;
	case Client::CPlayer::HUD::RIGHT_RIGHT:
		strHUDTag = "RightHUD_Right";
		break;
	case Client::CPlayer::HUD::RIGHT_BOTTOM:
		strHUDTag = "RightHUD_Bottom";
		break;
	case Client::CPlayer::HUD::RIGHT_LEFT:
		strHUDTag = "RightHUD_Left";
		break;
	}

	return strHUDTag;
}

void CPlayer::Set_HUD_MaxCooltime(HUD eHUD, _float fCurrnetCooltime)
{
	string strHUDTag = Get_HUD_Tag(eHUD);
	_uint iIndex = ECast(eHUD);
	m_MaxCooltimes[iIndex] = fCurrnetCooltime;

	switch (eHUD)
	{
	case Client::CPlayer::HUD::LEFT_TOP:
	case Client::CPlayer::HUD::LEFT_RIGHT:
	case Client::CPlayer::HUD::LEFT_BOTTOM:
	case Client::CPlayer::HUD::LEFT_LEFT:
		m_pUIManager->Change_LeftHUD_MaxCoolTime(strHUDTag, fCurrnetCooltime);
		break;

	case Client::CPlayer::HUD::RIGHT_TOP:
	case Client::CPlayer::HUD::RIGHT_RIGHT:
	case Client::CPlayer::HUD::RIGHT_BOTTOM:
	case Client::CPlayer::HUD::RIGHT_LEFT:
		m_pUIManager->Change_RightHUD_MaxCoolTime(strHUDTag, fCurrnetCooltime);
		break;
	}
}

void CPlayer::Set_HUD_Cooltime(HUD eHUD, _float fCurrnetCooltime)
{
	string strHUDTag = Get_HUD_Tag(eHUD);

	switch (eHUD)
	{
	case Client::CPlayer::HUD::LEFT_TOP:
	case Client::CPlayer::HUD::LEFT_RIGHT:
	case Client::CPlayer::HUD::LEFT_BOTTOM:
	case Client::CPlayer::HUD::LEFT_LEFT:
		m_pUIManager->Change_LeftHUD_CurrentCoolTime(strHUDTag, fCurrnetCooltime);
		break;

	case Client::CPlayer::HUD::RIGHT_TOP:
	case Client::CPlayer::HUD::RIGHT_RIGHT:
	case Client::CPlayer::HUD::RIGHT_BOTTOM:
	case Client::CPlayer::HUD::RIGHT_LEFT:
		m_pUIManager->Change_RightHUD_CurrentCoolTime(strHUDTag, fCurrnetCooltime);
		break;
	}
}

_float CPlayer::Get_HUD_Cooltime(HUD eHUD)
{
	_float fCooltime = 10000.f;
	string strHUDTag = Get_HUD_Tag(eHUD);

	switch (eHUD)
	{
	case Client::CPlayer::HUD::LEFT_TOP:
	case Client::CPlayer::HUD::LEFT_RIGHT:
	case Client::CPlayer::HUD::LEFT_BOTTOM:
	case Client::CPlayer::HUD::LEFT_LEFT:
		fCooltime = m_pUIManager->Get_LeftHUD_CurrentCoolTime(strHUDTag);
		break;
	case Client::CPlayer::HUD::RIGHT_TOP:
	case Client::CPlayer::HUD::RIGHT_RIGHT:
	case Client::CPlayer::HUD::RIGHT_BOTTOM:
	case Client::CPlayer::HUD::RIGHT_LEFT:
		fCooltime = m_pUIManager->Get_RightHUD_CurrentCoolTime(strHUDTag);
		break;
	}

	return fCooltime;
}

//_bool CPlayer::Activate_HUD_Skill(HUD eHUD)
//{
//	_uint iIndex = ECast(eHUD);
//	_float fCooltime = m_MaxCooltimes[iIndex];
//
//	Set_HUD_Cooltime(eHUD, fCooltime);
//
//	return true;
//}

_bool CPlayer::Activate_HUD_Skill(HUD eHUD, _float fCost)
{
	_bool bIsCooltimeEnd = Is_HUD_Cooltime_End(eHUD, fCost);
	if (false == bIsCooltimeEnd)
	{

		return false;
	}

	_float fCurrentCooltime = Get_HUD_Cooltime(eHUD);
	_float fResultCooltime = fCurrentCooltime;
	if (0 > fCost)
	{
		_uint iIndex = ECast(eHUD);
		fResultCooltime = m_MaxCooltimes[iIndex];
	}
	else 
	{
		fResultCooltime += fCost;
	}

	Set_HUD_Cooltime(eHUD, fResultCooltime);

	return true;
}

_bool CPlayer::Is_HUD_Cooltime_End(HUD eHUD, _float fCost)
{
	_float fCooltime = 10000.f;

	_uint iIndex = ECast(eHUD);
	
	_float fMaxCooltime = m_MaxCooltimes[iIndex];
	string strHUDTag = Get_HUD_Tag(eHUD);

	switch (eHUD)
	{
	case Client::CPlayer::HUD::LEFT_TOP:
	case Client::CPlayer::HUD::LEFT_RIGHT:
	case Client::CPlayer::HUD::LEFT_BOTTOM:
	case Client::CPlayer::HUD::LEFT_LEFT:
		fCooltime = m_pUIManager->Get_LeftHUD_CurrentCoolTime(strHUDTag);
		//fMaxCooltime = m_pUIManager->Get_LeftHUD_MaxCoolTime(strHUDTag);
		break;
	case Client::CPlayer::HUD::RIGHT_TOP:
	case Client::CPlayer::HUD::RIGHT_RIGHT:
	case Client::CPlayer::HUD::RIGHT_BOTTOM:
	case Client::CPlayer::HUD::RIGHT_LEFT:
		fCooltime = m_pUIManager->Get_RightHUD_CurrentCoolTime(strHUDTag);
		//fMaxCooltime = m_pUIManager->Get_RightHUD_MaxCoolTime(strHUDTag);
		break;
	}

	_bool bResult = { false };
	if (0 > fCost) 
	{	
		bResult = 0.1f >= fCooltime;
	}
	else 
	{
		_float fDiff = 0.f;

		if (fCooltime >= fMaxCooltime)
		{
			fDiff = fCost + 1.f;
		}
		else 
		{
			fDiff = fMaxCooltime - fCooltime;
		}

		bResult = fDiff >= fCost;
	}

	return bResult;
}

CPlayer::HUD CPlayer::Get_Skill_HUD_Enum(Player_Skill ePlayer_Skill)
{
	switch (ePlayer_Skill)
	{
	case Client::CPlayer::Player_Skill::SUPER_CHARGE:
		return HUD::LEFT_TOP;
	case Client::CPlayer::Player_Skill::HEAL:
		return HUD::LEFT_RIGHT;
	case Client::CPlayer::Player_Skill::REVOLVER:
		return HUD::LEFT_BOTTOM;
	case Client::CPlayer::Player_Skill::SHOTGUN:
		return HUD::LEFT_LEFT;

	case Client::CPlayer::Player_Skill::RIFLE:
		return HUD::RIGHT_TOP;
	case Client::CPlayer::Player_Skill::SLAM_DOWM:
		return HUD::RIGHT_RIGHT;
	case Client::CPlayer::Player_Skill::KICK:
		return HUD::RIGHT_BOTTOM;
	case Client::CPlayer::Player_Skill::ELECTRIC_WHIP:
		return HUD::RIGHT_LEFT;
	}

	return HUD();
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

void CPlayer::SetState_InteractionPush_End()
{
	m_pActor->Set_State(new CPlayer_InteractionPush_Rock_End());
}

void CPlayer::SetState_InteractionPull_End()
{
	m_pActor->Set_State(new CPlayer_InteractionPull_Rock_End());
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

void CPlayer::SetState_InteractZipLine()
{
	m_pActor->Set_State(new CPlayer_ZipLine_Start());
}

void CPlayer::SetState_CrouchUnder()
{
	m_pActor->Set_State(new CPlayer_CrouchUnder_Start());
}

void CPlayer::SetState_CrouchUnderGate()
{
	m_pActor->Set_State(new CPlayer_CrouchUnder_Gate());
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

void CPlayer::LeftHUDCoolDown(const string& strUIName, _float fCoolTime)
{
	m_pUIManager->Change_LeftHUD_CurrentCoolTime(strUIName, fCoolTime); // 방법 : UI객체 찾아서 바로 수정하는 법 (안받고 수정가능)
}

_float CPlayer::Get_LeftHUDMaxCoolTime(const string& strUIName)
{
	return m_pUIManager->Get_LeftHUD_MaxCoolTime(strUIName); // 방법 : UI객체 찾아서 바로 수정하는 법 (안받고 수정가능)
}

void CPlayer::Set_DiedScreen(_bool _bShowDiedScreen)
{
	m_bShowDiedScreen = _bShowDiedScreen;

	if (m_bShowDiedScreen == true)
	{
		m_pUIManager->Active_DiedScreen();
		m_pDataManager->Set_GameState(GAME_STATE::UI);
	}
	else
	{
		m_fHp = m_fMaxHp;
		m_pUIManager->NonActive_DiedScreen();
		m_pDataManager->Set_GameState(GAME_STATE::GAMEPLAY);
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

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pUIManager->Active_LetterBox();
	}
	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_pUIManager->NonActive_LetterBox();
	}

	/* ! UI : SkillWindow / Key : K */
	if (m_pGameInstance->Key_Down(DIK_K))
	{
		m_bShowSkillWindow = !m_bShowSkillWindow;

		if (m_bShowSkillWindow == true)
		{
			m_pUIManager->Active_SkillWindowBackground();
			m_pUIManager->NonActive_PlayerHUD(); // PlayerHUD Off
			m_pUIManager->NonActive_MouseCursor(); // MouseCursor Off
			m_pUIManager->NonActive_Crosshair(); // Crosshair Off
			m_pDataManager->Set_GameState(GAME_STATE::UI);
		}
		else
		{
			m_pUIManager->NonActive_SkillWindowAll();
			m_pUIManager->Active_Crosshair(true); // Crosshair On
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
			m_fHp = m_fMaxHp;
			m_pUIManager->NonActive_DiedScreen();
			m_pDataManager->Set_GameState(GAME_STATE::GAMEPLAY);
		}
	}

	/* ! UI : TestText / Key : 6 */
	if (m_pGameInstance->Key_Down(DIK_5))
	{
		m_pUIManager->Set_SkillLevel("ElectricCord", CUI::UI_LEVEL::LEVEL1); // LEVEL1 == 언락 가능 상태
	}
	/* ! UI : TestText / Key : 7 */
	if (m_pGameInstance->Key_Down(DIK_6))
	{
		m_pUIManager->Set_SkillLevel("ElectricDash", CUI::UI_LEVEL::LEVEL1); // LEVEL1 == 언락 가능 상태
	}

	/* ! UI : TestText / Key : 7 */
	if (m_pGameInstance->Key_Down(DIK_7))
	{
		/* 함수 이름 직관적이게 바꿨습니다. */

		// !성희 추가 : =>특정 스킬의 쿨타임을 찾아서 CurrentCoolTime을 수정하는 방법 2가지<=
		//CUI* pUI = m_pUIManager->Get_LeftHUD("LeftHUD_Right"); // 1. 첫번째 방법 : UI객체 받아오는 법 (받아서 수정가능)
		m_pUIManager->Change_LeftHUD_CurrentCoolTime("LeftHUD_Right", 5.f); // 2. 두번째 방법 : UI객체 찾아서 바로 수정하는 법 (안받고 수정가능)
		
		// !성희 추가 : =>특정 스킬의 쿨타임을 찾아서 MaxCoolTime을 수정하는 방법<=
		m_pUIManager->Change_LeftHUD_MaxCoolTime("LeftHUD_Right", 5.f); // 2. 두번째 방법 : UI객체 찾아서 바로 수정하는 법 (안받고 수정가능)
	}
	/* ! UI : TestText / Key : 8 */
	if (m_pGameInstance->Key_Down(DIK_8))
	{
		m_pUIManager->Active_TutorialBox();
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::START);
	}
	/* ! UI : TestText / Key : 9 */
	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_pUIManager->Active_TutorialBox();
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::CRANE);
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
	/* 숨쉬는 이펙트 추가 */
	//m_pEffect = EFFECT_MANAGER->Play_Effect("Player/Breath/", "SY_Player_Breath02.json", this, TRUE, "lips_H_close_upnode");
	m_pEffect = EFFECT_MANAGER->Play_Effect("Player/Breath/", "SY_Player_Breath04.json", this, true, "lips_H_close_upnode");



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
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Whip"), "RightInHandIndex", WeaponDesc,		PLAYER_WEAPON_WHIP));


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

void CPlayer::Slam()
{
	CGameObject* pSlam = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Impact_Slam");
	pSlam->Set_Position(Get_Position());
}

void CPlayer::Throw_Dynamite()
{
	CGameObject* pDynamite = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Bullet_Dynamite");
	
	_float3 vSpawnPos = Get_Position();
	vSpawnPos.y += 1.f;
	
	_float3 vTargetPos = Calc_Front_Pos(_float3(0.f, 1.f, 1.f));
	
	pDynamite->Set_Position(vSpawnPos);
	pDynamite->Get_Transform()->Look_At_OnLand(vTargetPos);

}

void CPlayer::Teleport()
{
	CGameObject* pTeleport = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Bullet_Teleport");

	_float3 vSpawnPos = Get_Position();
	vSpawnPos.y += 1.f;

	_float3 vTargetPos = Calc_Front_Pos(_float3(0.f, 1.f, 1.f));

	pTeleport->Set_InitPosition(vSpawnPos);
	pTeleport->Get_Transform()->Look_At_OnLand(vTargetPos);
}

void CPlayer::Update_SuperCharge(_float fTimeDelta)
{
	_float fTime = m_fSuperChargeTime - fTimeDelta;

	if (fTime > 0) 
	{
		m_fSuperChargeTime = fTime;
		
		// 슈퍼차지 상시 이펙트 생성
		EFFECT_MANAGER->Generate_Effect(&m_fEffectTimeAcc, 0.5f, fTimeDelta, "Player/SuperCharge/", "SuperCharge_Always_Pos_03.json", Get_Position());
		//EFFECT_MANAGER->Generate_Effect_AttachBone(&m_fEffectTimeAcc, 0.5f, fTimeDelta, "Player/SuperCharge/", "SuperCharge_Always_Pos_03.json", this, true, "Head");
	}
	else 
	{
		m_fSuperChargeTime = 0.f;
		m_fEffectTimeAcc = 0.f;
	}

	//m_fSuperChargeTime = fTime > 0 ? fTime : 0.f;
}

void CPlayer::Search_LockOn_Target()
{
	m_pLockOnTarget = Select_The_Nearest_Enemy(LAYER_MONSTER);
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
		m_pActor->Set_State(new CPlayer_DeathNormal_F_01());
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

	if (nullptr != m_pEffect)
	{
		m_pEffect->Delete_Object_Owner();
		Safe_Release(m_pEffect);
	}


}
