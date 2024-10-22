#include "stdafx.h"
#include "GameInstance.h"
#include "Bandit_Sniper.h"
#include "Data_Manager.h"
#include "Body_Bandit_Sniper.h"
#include "Weapon_Bandit_Sniper.h"

/* State */
#include "Sniper_CoverLow_Idle.h" 
#include "Sniper_KnockFrontLight_F_01.h"
#include "Sniper_KnockFrontLight_F_02.h"
#include "Sniper_KnockUp_Low_Fixed.h"

#include "Sniper_HitNormal_FL_01.h"
#include "Sniper_HitNormal_FR_01.h"
#include "Sniper_HitHeavy_F_01.h"
#include "Sniper_HitHeavy_FR_01.h"
#include "Sniper_HitHeavy_FL.h"

#include "Sniper_OpenStatePull_F_01.h"
#include "Sniper_HitLightOpened_L01.h"
#include "Sniper_HitLightOpened_R_01.h"

#include "Sniper_Electrocute_Loop_TEMP.h" 

#include "Sniper_DeathLight_F_01.h"
#include "Sniper_Weakspot_Death_01.h"
#include "Sniper_DeathNormal_F_01.h"
#include "Sniper_DeathNormal_F_03.h"
#include "Sniper_DeathHeavy_F_03.h"

CBandit_Sniper::CBandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CBandit_Sniper::CBandit_Sniper(const CBandit_Sniper& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CBandit_Sniper::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBandit_Sniper::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

		GameObjectDesc.fSpeedPerSec = 10.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(&GameObjectDesc));
	}
	else
	{
		FAILED_CHECK(__super::Initialize(pArg));
	}


	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CBandit_Sniper>(this);
		m_pActor->Set_State(new CSniper_CoverLow_Idle());
	}

	FAILED_CHECK(Ready_Option());
	
	m_fHp = 20.f;
	m_pTransformCom->Set_Look(0.f, 0.f, -1.f);

	m_pRigidBody->Set_UseGravity(false);
	///* !성희 추가 : 몬스터 HUD 생성 */
	//Ready_EnemyHUD_Shard(m_pGameInstance->Get_NextLevel(), this);
	return S_OK;
}

void CBandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBandit_Sniper::Tick(_float fTimeDelta)
{
	///* !성희 추가 : 몬스터 HUD 위치 갱신 */
	Check_EnemyHUD_World(m_pTransformCom->Get_WorldMatrix()/*, vOffsetPos*/);

	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

	if (true == m_bDeadState)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc >= 3.f)
		{
			Set_Dead(true);
		}
	}
}

void CBandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBandit_Sniper::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CBandit_Sniper::Set_ColliderSize(_float fSizeX, _float fSizeY, _float fSizeZ)
{
	CBody_Bandit_Sniper* pBody = dynamic_cast<CBody_Bandit_Sniper*>(m_pBody);
	NULL_CHECK_RETURN(pBody, );
	
	pBody->Get_Collider()->Set_ColliderSize(fSizeX, fSizeY, fSizeZ);
}

void CBandit_Sniper::Sniping_Target(_float4 TargetPos)
{
	CWeapon_Bandit_Sniper* pWeapon = dynamic_cast<CWeapon_Bandit_Sniper*>(m_pWeapon);
	NULL_CHECK_RETURN(pWeapon, );

	_float4x4 matTemp = pWeapon->Get_WeaponWorldMatrix();

	m_iBulletCnt += 1;

	_float4 NewPos = TargetPos;
	NewPos.y += 1.f;
	pWeapon->Sniping(NewPos, _float3(matTemp.m[3][0], matTemp.m[3][1], matTemp.m[3][2])); // (_float4 vTargetPos, _float3 StartfPos)
}

HRESULT CBandit_Sniper::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Sniper::Ready_PartObjects()
{
	/* For. Body */
	{
		CBody::BODY_DESC		BodyDesc = {};
		BodyDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Sniper"), BodyDesc));
	}

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("RightHandProp");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Bandit_Sniper"), "RightHandProp", WeaponDesc, TEXT("Weapon_Sniper")));
	
		m_pWeapon = Get_Weapon(TEXT("Weapon_Sniper"));
		m_pWeapon->Set_Enable(true);
	}

	return S_OK;
}

HRESULT CBandit_Sniper::Ready_Option()
{
	m_bProtectExist = true; /* 현재 방어막 있는상태 */
	m_pTarget = m_pGameInstance->Get_Player(); /* 타겟은 플레이어 고정 */ 
	
	return S_OK;
}

void CBandit_Sniper::Hitted_Left(Power ePower)
{
	/* 무기 강도 */
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CSniper_HitNormal_FL_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_1.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CSniper_HitNormal_FL_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CSniper_HitHeavy_FL());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_3.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	default:
		m_pActor->Set_State(new CSniper_HitNormal_FL_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_1.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	}
}

void CBandit_Sniper::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CSniper_HitNormal_FR_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_1.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CSniper_HitNormal_FR_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CSniper_HitHeavy_FR_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_3.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	default:
		m_pActor->Set_State(new CSniper_HitNormal_FR_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_1.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	}
}

void CBandit_Sniper::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CSniper_KnockFrontLight_F_02());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_1.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CSniper_KnockFrontLight_F_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CSniper_HitHeavy_F_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_3.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	default:
		m_pActor->Set_State(new CSniper_KnockFrontLight_F_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_1.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
		break;
	}
}

void CBandit_Sniper::Hitted_Knock(_bool bIsCannonball)
{
	/* 발차기 */
	m_pActor->Set_State(new CSniper_KnockFrontLight_F_01());
	m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_7.wav", SOUND_ENEMY_HIT2, 3.f);

}

void CBandit_Sniper::Hitted_Dead(Power ePower)
{
	m_pBody->Collider_Off(); // 바디콜라이더 비활성화 
	m_bDeadState = true;

	wstring strSoundKey = L"";

	_int iRandomSound = m_pGameInstance->Random_Int(1, 5);
	switch (iRandomSound)
	{
		case 1:
			strSoundKey = L"bandit01_death_1.wav";
			break;
		case 2:
			strSoundKey = L"bandit01_death_2.wav";
			break;
		case 3:
			strSoundKey = L"bandit01_death_3.wav";
			break;
		case 4:
			strSoundKey = L"bandit01_death_4.wav";
			break;
		case 5:
			strSoundKey = L"bandit01_death_5.wav";
			break;
	}
	
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CSniper_DeathLight_F_01());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CSniper_DeathNormal_F_03());
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CSniper_DeathHeavy_F_03());
		break;
	default:
		m_pActor->Set_State(new CSniper_DeathNormal_F_01());
		
		break;
	}

	m_pGameInstance->Play_Sound(L"SNIPER_DEATH", strSoundKey, SOUND_ENEMY_DEAD, 3.f);
	//CSniper_Weakspot_Death_01는 약점공격으로 죽을때 
}

void CBandit_Sniper::Hitted_Electrocute() //! 돌진감전
{
	m_pActor->Set_State(new CSniper_Electrocute_Loop_TEMP());
	m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hitlong_5.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
}

void CBandit_Sniper::Hitted_OpenState_Pull() //! 끌어 당김
{
	m_pActor->Set_State(new CSniper_OpenStatePull_F_01());
	m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 3.f);
}

void CBandit_Sniper::Hitted_Opened(Direction eDirection) //! 끌고 마구공격
{
	switch (eDirection)
	{
	case Engine::Left:
		m_pActor->Set_State(new CSniper_HitLightOpened_L01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 2.f);
		break;
	case Engine::Right:
		m_pActor->Set_State(new CSniper_HitLightOpened_R_01());
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 2.f);
		break;
	case Engine::Front:
		m_pActor->Set_State(new CSniper_HitLightOpened_R_01()); // ? Front도 있음?
		m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_2.wav", SOUND_ENEMY_HIT_GROUND, 2.f);
		break;
	}
}

void CBandit_Sniper::Hitted_KnockUp() //! 공중 띄워짐
{
	m_pActor->Set_State(new CSniper_KnockUp_Low_Fixed());
	m_pGameInstance->Play_Sound(L"SNIPER_HIT", L"bandit01_hit_7.wav", SOUND_ENEMY_HIT2, 3.f);
}

CBandit_Sniper* CBandit_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBandit_Sniper* pInstance = new CBandit_Sniper(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBandit_Sniper::Clone(void* pArg)
{
	CBandit_Sniper* pInstance = new CBandit_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBandit_Sniper::Pool()
{
	return new CBandit_Sniper(*this);
}

void CBandit_Sniper::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
