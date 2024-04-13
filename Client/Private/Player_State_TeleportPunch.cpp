#include "..\Public\Player_State_TeleportPunch.h"
#include "Player_IdleLoop.h"
#include "Transform.h"
#include "Data_Manager.h"
#include "SMath.h"
#include "GameInstance.h"

void CPlayer_State_TeleportPunch::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	Sound_Whoosh_Up();
}

CState<CPlayer>* CPlayer_State_TeleportPunch::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	CCharacter* pTarget = pActor->Get_Target();
	if (nullptr == pTarget) 
	{
		return new CPlayer_IdleLoop();
	}

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(2);

		if (true == m_bFlags[0])
		{
			Sound_Whoosh_Down();
		}

	}
	else if (false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta, 30.f, 40);
		m_bFlags[1] = pActor->Is_Inputable_Front(4) || 1.5f >= pActor->Calc_Distance();
	}
	else if (false == m_bFlags[2] && 1.5f >= pActor->Calc_Distance())
	{
		

		//_vector vPlayerPos = pActor->Get_Position_Vector();
		//_vector vDir = pTarget->Calc_Look_Dir_XZ(vPlayerPos);
		
		_float fDamage = 30.f + CData_Manager::GetInstance()->Get_Additional_ElectricDamage();
		_float fForce = 0.3f;
		Direction eDir = Direction::Front;
		Power ePower = Power::Heavy;
		_bool bIsMelee = true;

		//pTarget->Set_Hitted(fDamage, vDir, fForce, 1.f, eDir, ePower, bIsMelee);
		//pActor->Create_Hitting_Effect(pTarget->Get_Position(), ePower);

		pActor->Hit_Direct(pTarget, fDamage, fForce, eDir, ePower, bIsMelee);


		Sound_Impact_Layer1();
		Sound_Impact_Layer2();


		//pTarget->Look_At_And_Knockback(pActor->Get_Position(), 1.0f);
		//pTarget->Hitted_Dead(Power::Heavy);
		//pTarget->Set_Invincible(true);
		m_bFlags[2] = true;
	}




	return nullptr;
}

void CPlayer_State_TeleportPunch::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Target(nullptr);
}

CState<CPlayer>* CPlayer_State_TeleportPunch::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return TeleportPunch_State(pActor, fTimeDelta, _iAnimIndex);
}

void CPlayer_State_TeleportPunch::Sound_Whoosh_Up()
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

void CPlayer_State_TeleportPunch::Sound_Whoosh_Down()
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

void CPlayer_State_TeleportPunch::Sound_Impact_Layer1()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"player_supercharged_punch_impact_layer1_001.wav";
		break;
	case 1:
		strFileName = L"player_supercharged_punch_impact_layer1_002.wav";
		break;
	case 2:
		strFileName = L"player_supercharged_punch_impact_layer1_003.wav";
		break;
	case 3:
		strFileName = L"player_supercharged_punch_impact_layer1_004.wav";
		break;
	case 4:
		strFileName = L"player_supercharged_punch_impact_layer1_005.wav";
		break;
	default:
		strFileName = L"player_supercharged_punch_impact_layer1_001.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_IMPACT", strFileName, CHANNELID::SOUND_PLAYER_IMPACT, 10.f);
}

void CPlayer_State_TeleportPunch::Sound_Impact_Layer2()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"player_supercharged_punch_impact_layer2_001.wav";
		break;
	case 1:
		strFileName = L"player_supercharged_punch_impact_layer2_002.wav";
		break;
	case 2:
		strFileName = L"player_supercharged_punch_impact_layer2_003.wav";
		break;
	case 3:
		strFileName = L"player_supercharged_punch_impact_layer2_004.wav";
		break;
	case 4:
		strFileName = L"player_supercharged_punch_impact_layer2_005.wav";
		break;
	default:
		strFileName = L"player_supercharged_punch_impact_layer2_001.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_IMPACT", strFileName, CHANNELID::SOUND_PLAYER_IMPACT2, 10.f);
}
