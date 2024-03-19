#include "Player.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon_Player.h"
#include "Player_IdleLoop.h"
#include "Data_Manager.h"

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
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));

	m_iHp = 100;

// 	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
// 	{
		m_pActor = new CActor<CPlayer>(this);
		m_pActor->Set_State(new CPlayer_IdleLoop());
// 	}


	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

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

	CData_Manager::GetInstance()->Set_Player(this);
	m_pGameInstance->Set_Player(this);
	/* Temp - 맵에 맞게 위치 조정한값*/
	//m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, XMVectorSet(-26.f, 0.f, -6.f, 1.f));

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_Player(this);

	__super::Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor/* && m_pGameInstance->Get_NextLevel() != ECast(LEVEL_TOOL)*/)
	{
		m_pActor->Update_State(fTimeDelta);
	}

	Update_ChargingTime(fTimeDelta);



	CData_Manager::GetInstance()->Set_CurHP(m_iHp);

	if (m_pGameInstance->Key_Down(DIK_C))
		m_iHp = 100;

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

void CPlayer::Activate_ShootingReaction()
{
	m_pBody->Activate_ShootingReaction();
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

#pragma endregion 상호작용

void CPlayer::Search_Target(_float fMaxDistance)
{
	__super::Search_Target(LAYER_BOSS, fMaxDistance);
	__super::Search_Target(LAYER_MONSTER, fMaxDistance);
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
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Punch"), "LeftHandIK", WeaponDesc, WEAPON_PUNCH_L));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_Punch"), "RightHandIK", WeaponDesc, WEAPON_PUNCH_R));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Player_Weapon_ELWinchester"), "RightHandIK", WeaponDesc, WEAPON_WINCHESTER));

	//}

	CWeapon* m_pWeapon_Punch_L = Get_Weapon(WEAPON_PUNCH_L);
	m_pWeapon_Punch_L->Set_Enable(false);
	
	CWeapon* m_pWeapon_Punch_R = Get_Weapon(WEAPON_PUNCH_R);
	m_pWeapon_Punch_R->Set_Enable(false);
	
	CWeapon* m_pWeapon_Winchester = Get_Weapon(WEAPON_WINCHESTER);
	m_pWeapon_Winchester->Set_Enable(false);

	
	
	return S_OK;
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
