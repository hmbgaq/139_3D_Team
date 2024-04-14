#include "stdafx.h"
#include "Tank.h"
#include "GameInstance.h"
#include "Body_Tank.h"
#include "Weapon_Tank.h"
#include "Tank_Idle.h"
#include "Data_Manager.h"
#include "Player.h"

#include "Tank_HitHeavyShield_FL_01.h"
#include "Tank_HitHeavyShield_FR_01.h"
#include "Tank_HitHeavyShield_F_01.h"
#include "Tank_HitNormalShield_FL_01.h"
#include "Tank_HitNormalShield_FR_01.h"
#include "Tank_HitNormalShield_F_01.h"
#include "Tank_Taunt_03.h"

#include "Tank_Stun_Start.h"
#include "Tank_DeathNormal_F_01.h"
#include "Finisher_Tank.h"

#include "Player.h"
#include "Player_Finisher_Tank.h"

#include "Effect_Manager.h"
#include "SMath.h"


CTank::CTank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CTank::CTank(const CTank& rhs)
	: CMonster_Character(rhs)
{
}

void CTank::Set_ShieldBroken()
{
	Reset_AttackCount();

	CWeapon_Tank* pWeapon_Tank = dynamic_cast<CWeapon_Tank*>(Get_Weapon(L"Weapon_Shield"));
	if (pWeapon_Tank)
	{
		pWeapon_Tank->Set_Follow(false);
		if (CModel::ANIM_STATE_NORMAL != pWeapon_Tank->Get_AnimState()) 
		{
			pWeapon_Tank->Set_Animation(0, CModel::ANIM_STATE_NORMAL, 0);

		}
		
	}
		
	m_fShieldBrokenTime = 20.f;

	Play_Sound_Shield_Crash();
}

HRESULT CTank::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CTank::Initialize(void* pArg)
{
	m_fMaxHp = 100;
	m_fHp = m_fMaxHp;

	Ready_EnemyHUD_Shard(m_pGameInstance->Get_NextLevel(), this);

	if (pArg == nullptr)
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

		GameObjectDesc.fSpeedPerSec = 10.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(&GameObjectDesc));
	}
	else
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = *(CGameObject::GAMEOBJECT_DESC*)pArg;

		if (GameObjectDesc.eDescType == CGameObject::MONSTER_DESC)
		{
			CMonster_Character::MONSTER_DESC MonsterDesc = *(CMonster_Character::MONSTER_DESC*)pArg;

			MonsterDesc.fSpeedPerSec = 10.f;
			MonsterDesc.fRotationPerSec = XMConvertToRadians(90.0f);

			FAILED_CHECK(__super::Initialize(&MonsterDesc));
		}
		else
		{
			GameObjectDesc.fSpeedPerSec = 10.f;
			GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
			FAILED_CHECK(__super::Initialize(&GameObjectDesc));
		}

	}

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CTank>(this);
		m_pActor->Set_State(new CTank_Idle());
	}


	m_bIsFixed = true;

	//m_pTarget = CData_Manager::GetInstance()->Get_Player();
	//m_pNavigationCom = nullptr;

	return S_OK;
}

void CTank::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CTank::Tick(_float fTimeDelta)
{
	/* !성희 추가 : 몬스터 HUD 위치 갱신 */
	Check_EnemyHUD_World(m_pTransformCom->Get_WorldMatrix()/*, vOffsetPos*/);

	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

	m_bIsFixed = !Is_ShieldBroken();

	if (Is_ShieldBroken())
	{
		Update_ShieldBrokenTime(fTimeDelta);
	}

	if (nullptr == m_pTarget)
	{
		Search_Target(10.f);
	}

	//if (nullptr == m_pTarget && m_pGameInstance->Key_Pressing(DIK_V))
	//{
	//	m_pTarget = CData_Manager::GetInstance()->Get_Player();
	//}
	if (m_bRushStart)
	{
		m_pEffect = EFFECT_MANAGER->Play_Effect("Parasiter/", "Tank_Rush.json", this);
		m_bRushStart = false;
	}
	if (m_bRushStop == true && m_pEffect != nullptr)
	{
		EFFECT_MANAGER->Return_ToPool(m_pEffect);
		m_pEffect = nullptr;
		m_bRushStop = false;
	}

}

void CTank::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CTank::Render()
{
	return S_OK;
}

void CTank::Hitted_Left(Power ePower)
{
	if (true == Is_ShieldBroken())
	{
		switch (ePower)
		{
		case Engine::Medium:
			m_pActor->Set_State(new CTank_HitNormalShield_FL_01());
		case Engine::Heavy:
			m_pActor->Set_State(new CTank_HitHeavyShield_FL_01());
		}
	}

}

void CTank::Hitted_Right(Power ePower)
{
	if (true == Is_ShieldBroken())
	{
		switch (ePower)
		{
		case Engine::Medium:
			m_pActor->Set_State(new CTank_HitNormalShield_FR_01());
		case Engine::Heavy:
			m_pActor->Set_State(new CTank_HitHeavyShield_FR_01());
		}
	}

}

void CTank::Hitted_Front(Power ePower)
{
	if (true == Is_ShieldBroken())
	{
		switch (ePower)
		{
		case Engine::Medium:
			m_pActor->Set_State(new CTank_HitNormalShield_F_01());
		case Engine::Heavy:
			m_pActor->Set_State(new CTank_HitHeavyShield_F_01());
		}
	}
}

//void CTank::Hitted_Stun(Power ePower)
//{
//	m_pActor->Set_State(new CTank_Stun_Start());
//}

void CTank::Hitted_Finish()
{
	m_pActor->Set_State(new CFinisher_Tank());
	Look_At_OnLand(CData_Manager::GetInstance()->Get_Player()->Get_Position_Vector());
		
	CPlayer* pPlayer = Set_Player_Finisher_Pos(_float3(0.f, 0.f, 2.5f));
	_vector vFront = XMLoadFloat3(&Calc_Front_Pos(_float3(-1.2f, 0.f, 0.5f)));
	pPlayer->Look_At_OnLand(vFront);
	pPlayer->Get_Actor()->Set_State(new CPlayer_Finisher_Tank());
}

void CTank::Hitted_Dead(Power ePower)
{
	dynamic_cast<CBody_Tank*>(m_pBody)->Set_Dissolve(true);

	EFFECT_MANAGER->Play_Effect("Hit/", "Dead_Monster_01.json", nullptr, Get_Position());

	m_pActor->Set_State(new CTank_DeathNormal_F_01());
}

void CTank::Set_Taunt()
{
	m_pActor->Set_State(new CTank_Taunt_03());
}

void CTank::Create_GroundWave()
{
	CGameObject* pGroundWave = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Projectile_GroundWave");
	pGroundWave->Set_Position(Get_Position());

	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	pGroundWave->Get_Transform()->Look_At(pPlayer->Get_Position_Vector());

	Play_Sound_Wave();
}

void CTank::Play_Sound_Attack_Melee()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_attack_melee_punch001.wav";
		break;
	case 1:
		strFileName = L"tank_attack_melee_punch002.wav";
		break;
	case 2:
		strFileName = L"tank_attack_melee_punch003.wav";
		break;
	default:
		strFileName = L"tank_attack_melee_punch001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_ATTACK_MELEE", strFileName, CHANNELID::SOUND_ENEMY_WHOOSH, 10.f);

}

void CTank::Play_Sound_Attack_Shield()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_attack_shield_whoosh001.wav";
		break;
	case 1:
		strFileName = L"tank_attack_shield_whoosh002.wav";
		break;
	case 2:
		strFileName = L"tank_attack_shield_whoosh003.wav";
		break;
	default:
		strFileName = L"tank_attack_shield_whoosh001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_ATTACK_SHIELD", strFileName, CHANNELID::SOUND_ENEMY_WHOOSH, 10.f);

}

void CTank::Play_Sound_Wave()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_attack_wave_start001.wav";
		break;
	case 1:
		strFileName = L"tank_attack_wave_start002.wav";
		break;
	case 2:
		strFileName = L"tank_attack_wave_start003.wav";
		break;
	default:
		strFileName = L"tank_attack_wave_start001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_WAVE", strFileName, CHANNELID::SOUND_ENEMY_WHOOSH, 10.f);

}

void CTank::Play_Sound_Bodyfall()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_bodyfall_generic001.wav";
		break;
	case 1:
		strFileName = L"tank_bodyfall_generic002.wav";
		break;
	case 2:
		strFileName = L"tank_bodyfall_generic003.wav";
		break;
	case 3:
		strFileName = L"tank_bodyfall_generic004.wav";
		break;
	case 4:
		strFileName = L"tank_bodyfall_generic005.wav";
		break;
	case 5:
		strFileName = L"tank_bodyfall_generic006.wav";
		break;
	default:
		strFileName = L"tank_bodyfall_generic001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_BODYFALL", strFileName, CHANNELID::SOUND_ENEMY_DEAD, 10.f);

}

void CTank::Play_Sound_Footstep()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_footsteps_generic001.wav";
		break;
	case 1:
		strFileName = L"tank_footsteps_generic002.wav";
		break;
	case 2:
		strFileName = L"tank_footsteps_generic003.wav";
		break;
	case 3:
		strFileName = L"tank_footsteps_generic004.wav";
		break;
	case 4:
		strFileName = L"tank_footsteps_generic005.wav";
		break;
	case 5:
		strFileName = L"tank_footsteps_generic006.wav";
		break;
	default:
		strFileName = L"tank_footsteps_generic001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_FOOTSTEP", strFileName, CHANNELID::SOUND_ENEMY_FOOTSTEP, 10.f);

}

void CTank::Play_Sound_Hit()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_vo_hit001.wav";
		break;
	case 1:
		strFileName = L"tank_vo_hit002.wav";
		break;
	case 2:
		strFileName = L"tank_vo_hit003.wav";
		break;
	case 3:
		strFileName = L"tank_vo_hit004.wav";
		break;
	case 4:
		strFileName = L"tank_vo_hit005.wav";
		break;
	default:
		strFileName = L"tank_vo_hit001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_HIT", strFileName, CHANNELID::SOUND_ENEMY_HIT, 10.f);

}

void CTank::Play_Sound_Shield_Crash()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_stun_shield_crash001.wav";
		break;
	case 1:
		strFileName = L"tank_stun_shield_crash002.wav";
		break;
	case 2:
		strFileName = L"tank_stun_shield_crash003.wav";
		break;
	case 3:
		strFileName = L"tank_stun_shield_crash004.wav";
		break;
	case 4:
		strFileName = L"tank_stun_shield_crash005.wav";
		break;
	default:
		strFileName = L"tank_stun_shield_crash001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_SHIELD", strFileName, CHANNELID::SOUND_ENEMY_HIT2, 10.f);
}

void CTank::Play_Sound_Shield_PickUp()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_stun_shield_pickup001.wav";
		break;
	case 1:
		strFileName = L"tank_stun_shield_pickup002.wav";
		break;
	case 2:
		strFileName = L"tank_stun_shield_pickup003.wav";
		break;
	default:
		strFileName = L"tank_stun_shield_pickup001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_SHIELD", strFileName, CHANNELID::SOUND_ENEMY_HIT2, 10.f);
}



void CTank::Play_Sound_Voice_Attack()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_vo_attack001.wav";
		break;
	case 1:
		strFileName = L"tank_vo_attack002.wav";
		break;
	case 2:
		strFileName = L"tank_vo_attack003.wav";
		break;
	case 3:
		strFileName = L"tank_vo_attack004.wav";
		break;
	case 4:
		strFileName = L"tank_vo_attack005.wav";
		break;
	default:
		strFileName = L"tank_vo_attack001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_VOICE_ATTACK", strFileName, CHANNELID::SOUND_ENEMY_EFFORT, 10.f);
}

void CTank::Play_Sound_Voice_Charge()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_vo_taunt_charge001.wav";
		break;
	case 1:
		strFileName = L"tank_vo_taunt_charge002.wav";
		break;
	case 2:
		strFileName = L"tank_vo_taunt_charge003.wav";
		break;
	case 3:
		strFileName = L"tank_vo_taunt_charge004.wav";
		break;
	case 4:
		strFileName = L"tank_vo_taunt_charge005.wav";
		break;
	default:
		strFileName = L"tank_vo_taunt_charge001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_VOICE_CHARGE", strFileName, CHANNELID::SOUND_ENEMY_EFFORT, 10.f);
}

void CTank::Play_Sound_Shield_Impact()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"tank_attack_shield_impactA001.wav";
		break;
	case 1:
		strFileName = L"tank_attack_shield_impactA002.wav";
		break;
	case 2:
		strFileName = L"tank_attack_shield_impactA003.wav";
		break;
	case 3:
		strFileName = L"tank_attack_shield_impactA004.wav";
		break;
	case 4:
		strFileName = L"tank_attack_shield_impactA005.wav";
		break;
	default:
		strFileName = L"tank_attack_shield_impactA001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"TANK_ATTACK_SHIELD_IMPACT", strFileName, CHANNELID::SOUND_ENEMY_ATTACK, 10.f);
}

HRESULT CTank::Ready_Components()
{
	return S_OK;
}

HRESULT CTank::Ready_PartObjects()
{
	/* For. Body */
	{
		CBody::BODY_DESC		BodyDesc = {};
		FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Tank"), BodyDesc));
	}

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("LeftHandIK");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Tank"), "LeftHandIK", WeaponDesc, TEXT("Weapon_Shield")));
	}

	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Punch_Tank"), "LeftHand", WeaponDesc, TANK_WEAPON_PUNCH_L));
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Punch_Tank"), "RightHand", WeaponDesc, TANK_WEAPON_PUNCH_R));

		Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
		Set_Weapon_Enable(TANK_WEAPON_PUNCH_R, false);

		Set_Weapon_Collisions_Enable(TEXT("Weapon_Shield"), true);

	}

	return S_OK;
}

CTank* CTank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CTank* pInstance = new CTank(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTank::Clone(void* pArg)
{
	CTank* pInstance = new CTank(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTank::Pool()
{
	return new CTank(*this);
}

void CTank::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
