#include "stdafx.h"
#include "Bandit_Heavy.h"
#include "GameInstance.h"
#include "Body_Bandit_Heavy.h"
#include "BanditHeavy_Idle.h"
#include "Data_Manager.h"
#include "Player.h"
#include "SMath.h"

#include "BanditHeavy_HitNormal_F_01.h"
#include "BanditHeavy_HitNormal_F_02.h"
#include "BanditHeavy_HitNormal_L_01.h"
#include "BanditHeavy_HitNormal_R_01.h"
#include "BanditHeavy_HitHeavy_F_01.h"
#include "BanditHeavy_DeathHeavy_F_01.h"
#include "BanditHeavy_Taunt_01.h"


CBandit_Heavy::CBandit_Heavy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CBandit_Heavy::CBandit_Heavy(const CBandit_Heavy& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CBandit_Heavy::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBandit_Heavy::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

		GameObjectDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(&GameObjectDesc));
	}
	else
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = *(CGameObject::GAMEOBJECT_DESC*)pArg;

		if (GameObjectDesc.eDescType == CGameObject::MONSTER_DESC)
		{
			CMonster_Character::MONSTER_DESC MonsterDesc = *(CMonster_Character::MONSTER_DESC*)pArg;

			MonsterDesc.fSpeedPerSec = 7.f;
			MonsterDesc.fRotationPerSec = XMConvertToRadians(90.0f);

			FAILED_CHECK(__super::Initialize(&MonsterDesc));
		}
		else
		{
			GameObjectDesc.fSpeedPerSec = 7.f;
			GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
			FAILED_CHECK(__super::Initialize(&GameObjectDesc));
		}
		
	}

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CBandit_Heavy>(this);
		m_pActor->Set_State(new CBanditHeavy_Idle());
	}

	return S_OK;
}

void CBandit_Heavy::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBandit_Heavy::Tick(_float fTimeDelta)
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


	if (nullptr == m_pTarget)
	{
		Search_Target(10.f);
	}


	//if (nullptr == m_pTarget && m_pGameInstance->Key_Pressing(DIK_V))
	//{
	//	m_pTarget = CData_Manager::GetInstance()->Get_Player();
	//}

}

void CBandit_Heavy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBandit_Heavy::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CBandit_Heavy::Hitted_Left(Power ePower)
{
	switch (ePower)
	{
	//case Engine::Medium:
	//	m_pActor->Set_State(new CBanditHeavy_HitNormal_L_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CBanditHeavy_HitHeavy_F_01());
	}
}

void CBandit_Heavy::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	//case Engine::Medium:
	//	m_pActor->Set_State(new CBanditHeavy_HitNormal_R_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CBanditHeavy_HitHeavy_F_01());
	}
}

void CBandit_Heavy::Hitted_Front(Power ePower)
{
	_uint iRand = SMath::Random(0, 1);

	switch (ePower)
	{
	//case Engine::Medium:
	//	if (1 == iRand)
	//		m_pActor->Set_State(new CBanditHeavy_HitNormal_F_02());
	//	else 
	//		m_pActor->Set_State(new CBanditHeavy_HitNormal_F_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CBanditHeavy_HitHeavy_F_01());
	}
}

void CBandit_Heavy::Hitted_Dead(Power ePower)
{
	m_pActor->Set_State(new CBanditHeavy_DeathHeavy_F_01());
}

void CBandit_Heavy::Set_Taunt()
{
	m_pActor->Set_State(new CBanditHeavy_Taunt_01());
}

void CBandit_Heavy::Play_Sound_Attack()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"bandit_heavy_attack_strong_slam_impact001.wav";
		break;
	case 1:
		strFileName = L"bandit_heavy_attack_strong_slam_impact002.wav";
		break;
	case 2:
		strFileName = L"bandit_heavy_attack_strong_slam_impact003.wav";
		break;
	case 3:
		strFileName = L"bandit_heavy_attack_strong_slam_impact004.wav";
		break;
	case 4:
		strFileName = L"bandit_heavy_attack_strong_slam_impact005.wav";
		break;
	default:
		strFileName = L"bandit_heavy_attack_strong_slam_impact001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_ATTACK", strFileName, CHANNELID::SOUND_ENEMY_ATTACK, 10.f);

}

void CBandit_Heavy::Play_Sound_Whoosh()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh001.wav";
		break;
	case 1:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh002.wav";
		break;
	case 2:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh003.wav";
		break;
	case 3:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh004.wav";
		break;
	case 4:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh005.wav";
		break;
	case 5:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh006.wav";
		break;
	default:
		strFileName = L"bandit_vampiric_heavy_attack_hammer_whoosh001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_WHOOSH", strFileName, CHANNELID::SOUND_ENEMY_WHOOSH, 10.f);

}

void CBandit_Heavy::Play_Sound_Effort()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 7);
	switch (iRand)
	{
	case 0:
		strFileName = L"banditzombieheavy02_effort_1_01.wav";
		break;
	case 1:
		strFileName = L"banditzombieheavy02_effort_1_02.wav";
		break;
	case 2:
		strFileName = L"banditzombieheavy02_effort_1_03.wav";
		break;
	case 3:
		strFileName = L"banditzombieheavy02_effort_1_04.wav";
		break;
	case 4:
		strFileName = L"banditzombieheavy02_effort_1_05.wav";
		break;
	case 5:
		strFileName = L"banditzombieheavy02_effort_1_06.wav";
		break;
	case 6:
		strFileName = L"banditzombieheavy02_effort_1_07.wav";
		break;
	default:
		strFileName = L"banditzombieheavy02_effort_1_01.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_EFFORT", strFileName, CHANNELID::SOUND_ENEMY_VOICE, 10.f);

}

void CBandit_Heavy::Play_Sound_BodyMove()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"bandit_heavy_mvm_bodymoves_medium001.wav";
		break;
	case 1:
		strFileName = L"bandit_heavy_mvm_bodymoves_medium002.wav";
		break;
	case 2:
		strFileName = L"bandit_heavy_mvm_bodymoves_medium003.wav";
		break;
	case 3:
		strFileName = L"bandit_heavy_mvm_bodymoves_medium004.wav";
		break;
	case 4:
		strFileName = L"bandit_heavy_mvm_bodymoves_medium005.wav";
		break;
	default:
		strFileName = L"bandit_heavy_mvm_bodymoves_medium001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_BODYMOVE", strFileName, CHANNELID::SOUND_ENEMY_BODYMOVE, 10.f);

}

void CBandit_Heavy::Play_Sound_FootStep()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 8);
	switch (iRand)
	{
	case 0:
		strFileName = L"bandit_heavy_mvm_footsteps001.wav";
		break;
	case 1:
		strFileName = L"bandit_heavy_mvm_footsteps002.wav";
		break;
	case 2:
		strFileName = L"bandit_heavy_mvm_footsteps003.wav";
		break;
	case 3:
		strFileName = L"bandit_heavy_mvm_footsteps004.wav";
		break;
	case 4:
		strFileName = L"bandit_heavy_mvm_footsteps005.wav";
		break;
	case 5:
		strFileName = L"bandit_heavy_mvm_footsteps006.wav";
		break;
	case 6:
		strFileName = L"bandit_heavy_mvm_footsteps007.wav";
		break;
	case 7:
		strFileName = L"bandit_heavy_mvm_footsteps008.wav";
		break;
	default:
		strFileName = L"bandit_heavy_mvm_footsteps001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_FOOTSTEP", strFileName, CHANNELID::SOUND_ENEMY_FOOTSTEP, 10.f);

}

void CBandit_Heavy::Play_Sound_Hit()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 4);
	switch (iRand)
	{
	case 0:
		strFileName = L"banditvampiricheavy01_hitheavy_1_01.wav";
		break;
	case 1:
		strFileName = L"banditvampiricheavy01_hitheavy_1_02.wav";
		break;
	case 2:
		strFileName = L"banditvampiricheavy01_hitheavy_1_03.wav";
		break;
	case 3:
		strFileName = L"banditvampiricheavy01_hitheavy_1_04.wav";
		break;
	default:
		strFileName = L"banditvampiricheavy01_hitheavy_1_01.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_HIT", strFileName, CHANNELID::SOUND_ENEMY_HIT, 10.f);
}

void CBandit_Heavy::Play_Sound_Death()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 4);
	switch (iRand)
	{
	case 0:
		strFileName = L"banditvampiricheavy01_death_1_01.wav";
		break;
	case 1:
		strFileName = L"banditvampiricheavy01_death_1_02.wav";
		break;
	case 2:
		strFileName = L"banditvampiricheavy01_death_1_03.wav";
		break;
	case 3:
		strFileName = L"banditvampiricheavy01_death_1_04.wav";
		break;
	default:
		strFileName = L"banditvampiricheavy01_death_1_01.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_DEATH", strFileName, CHANNELID::SOUND_ENEMY_DEAD, 10.f);
}

HRESULT CBandit_Heavy::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Heavy::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Heavy"), BodyDesc));

	return S_OK;
}

//CBandit_Heavy* CBandit_Heavy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
//{
//	CBandit_Heavy* pInstance = new CBandit_Heavy(pDevice, pContext, strPrototypeTag);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX("Failed to Created : CBandit_Heavy");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}

//CGameObject* CBandit_Heavy::Clone(void* pArg)
//{
//	CBandit_Heavy* pInstance = new CBandit_Heavy(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Cloned : CBandit_Heavy");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CGameObject* CBandit_Heavy::Pool()
//{
//	return new CBandit_Heavy(*this);
//}

void CBandit_Heavy::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
