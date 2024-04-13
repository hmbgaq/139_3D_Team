#include "Player_EnergyWhip_Leap.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"
#include "Data_Manager.h"
#include "SMath.h"

void CPlayer_EnergyWhip_Leap::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 15);

	// 이펙트 생성 테스트
	EFFECT_MANAGER->Play_Effect("Player/Zapper_Dash/", "Zapper_Dash_31.json", pActor);
	//EFFECT_MANAGER->Create_Effect("Player/Zapper_Dash/", "Zapper_Dash_29.json", pActor);

	Sound_Dash();

	pActor->Play_Voice_Zapper_Dash();

}

CState<CPlayer>* CPlayer_EnergyWhip_Leap::Update(CPlayer* pActor, _float fTimeDelta)
{	
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		pActor->Chasing_Attack(fTimeDelta, 30.f, 8);
		m_bFlags[0] = pActor->Is_Inputable_Front(22);
	}

	if (false == m_bFlags[1] && pActor->Is_Inputable_Front(15))
	{
		pActor->Apply_Shake_And_Blur(Power::Heavy);

		CCharacter* pTarget = pActor->Get_Target();
		pTarget->Look_At_OnLand(pActor->Get_Position_Vector());

		if (CData_Manager::GetInstance()->Is_AdditionalSkill_Learned(Additional_Skill::ELECTROCUTE))
		{
			pTarget->Hitted_Electrocute();
			Sound_Hit();
		}
		else 
		{
			pTarget->Hitted_Front(Power::Medium);
			Sound_Hit();
		}
		
		m_bFlags[1] = true;
	}


	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_Leap::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Target(nullptr);
}

void CPlayer_EnergyWhip_Leap::Sound_Dash()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"player_whip_electrical_dash001.wav";
		break;
	case 1:
		strFileName = L"player_whip_electrical_dash002.wav";
		break;
	case 2:
		strFileName = L"player_whip_electrical_dash001.wav";
		break;
	case 3:
		strFileName = L"player_whip_electrical_dash004.wav";
		break;
	case 4:
		strFileName = L"player_whip_electrical_dash005.wav";
		break;
	default:
		strFileName = L"player_whip_electrical_dash001.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WHOOSH", strFileName, CHANNELID::SOUND_PLAYER_WHOOSH, 10.f);
}

void CPlayer_EnergyWhip_Leap::Sound_Hit()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"Player_EnergyMode_ElectroHit_01.wav";
		break;
	case 1:
		strFileName = L"Player_EnergyMode_ElectroHit_02.wav";
		break;
	case 2:
		strFileName = L"Player_EnergyMode_ElectroHit_03.wav";
		break;
	default:
		strFileName = L"Player_EnergyMode_ElectroHit_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_ATTACK", strFileName, CHANNELID::SOUND_PLAYER_ATTACK, 5.f);
}
