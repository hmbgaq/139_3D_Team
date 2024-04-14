#include "Player_EnergyWhip_CloseRange_01.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"
#include "Data_Manager.h"
#include "SMath.h"

#include "Player_EnergyWhip_Leap.h"
#include "Player_EnergyWhip_LongRange.h"
#include "Player_EnergyWhip_Pull.h"


void CPlayer_EnergyWhip_CloseRange_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_ZAPPER);
	pWeapon
		->Set_Damage(0)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.2f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);

	

}

CState<CPlayer>* CPlayer_EnergyWhip_CloseRange_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(4);
		if (true == m_bFlags[0])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_ZAPPER, true);

			// 이펙트 생성 테스트
			//EFFECT_MANAGER->Create_Effect("Player/Zapper_Shield/", "Zapper_Shield_20_distortionTest.json", pActor);
			EFFECT_MANAGER->Play_Effect("Player/Zapper_Shield/", "Zapper_Shield_22_distortionTest.json", pActor);

			Sound_Whoosh_Up();

			//pActor->Get_Body()->Collider_Off();
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(18);
		if (true == m_bFlags[1])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_ZAPPER, false);
			//pActor->Get_Body()->Get_Collider()->Set_Enable(true);

			Sound_Whoosh_Down();
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = true;
	}

	CData_Manager* pDataManager = CData_Manager::GetInstance();
	_bool bIsLearned = pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::ELECTRIC_DASH);

	if (m_pGameInstance->Key_Down(DIK_W) && true == bIsLearned)
	{
		pActor->Search_Target(20.f);
		if (pActor->Get_Target()) 
		{
			return new CPlayer_EnergyWhip_Leap();
			//return new CPlayer_EnergyWhip_LongRange();
		}
	}

	bIsLearned = pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::ELECTRIC_WHIP);
	if (m_pGameInstance->Key_Down(DIK_S) && true == bIsLearned)
	{
		pActor->Search_Target(20.f);
		if (pActor->Get_Target())
		{
			CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::ELECTRIC_WHIP);
			_bool bIsCooltimeEnd = pActor->Activate_HUD_Skill(eSelectedHUD);
			if (true == bIsCooltimeEnd)
			{
				return new CPlayer_EnergyWhip_Pull();
			}
			else 
			{
				pActor->Set_Target(nullptr);
			}
			
		}
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_CloseRange_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_ZAPPER, false);

	
}

void CPlayer_EnergyWhip_CloseRange_01::Sound_Whoosh_Up()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"player_supercharged_energy_whoosh_up_001.wav";
		break;
	case 1:
		strFileName = L"player_supercharged_energy_whoosh_up_002.wav";
		break;
	case 2:
		strFileName = L"player_supercharged_energy_whoosh_up_003.wav";
		break;
	case 3:
		strFileName = L"player_supercharged_energy_whoosh_up_004.wav";
		break;
	case 4:
		strFileName = L"player_supercharged_energy_whoosh_up_005.wav";
		break;
	default:
		strFileName = L"player_supercharged_energy_whoosh_up_001.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WHOOSH", strFileName, CHANNELID::SOUND_PLAYER_WHOOSH, 10.f);
}

void CPlayer_EnergyWhip_CloseRange_01::Sound_Whoosh_Down()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"player_supercharged_energy_whoosh_down_001.wav";
		break;
	case 1:
		strFileName = L"player_supercharged_energy_whoosh_down_002.wav";
		break;
	case 2:
		strFileName = L"player_supercharged_energy_whoosh_down_003.wav";
		break;
	case 3:
		strFileName = L"player_supercharged_energy_whoosh_down_004.wav";
		break;
	case 4:
		strFileName = L"player_supercharged_energy_whoosh_down_005.wav";
		break;
	default:
		strFileName = L"player_supercharged_energy_whoosh_down_001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"PLAYER_WHOOSH", strFileName, CHANNELID::SOUND_PLAYER_WHOOSH2, 10.f);
}
