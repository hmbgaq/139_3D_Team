#include "..\Public\Player_State_TeleportPunch.h"
#include "Player_IdleLoop.h"
#include "Transform.h"
#include "Data_Manager.h"
#include "SMath.h"
#include "GameInstance.h"

#include "Effect_Manager.h"
#include "Effect.h"

void CPlayer_State_TeleportPunch::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);


	// 텔레포트 펀치 이펙트
	m_pEffect = EFFECT_MANAGER->Play_Effect("Player/TeleportPunch/", "TeleportPunch_01.json", pActor, true, "Head");

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
		_float fForce = 0.6f;
		Direction eDir = Direction::Front;
		Power ePower = Power::Heavy;
		_bool bIsMelee = true;

		//pTarget->Set_Hitted(fDamage, vDir, fForce, 1.f, eDir, ePower, bIsMelee);
		//pActor->Create_Hitting_Effect(pTarget->Get_Position(), ePower);

		pActor->Hit_Direct(pTarget, fDamage, fForce, eDir, ePower, bIsMelee);


		// 텔레포트 펀치 피격 이펙트 생성
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_TeleportPunch.json", pActor, pActor->Calc_Front_Pos(_float3(0.f, 0.f, 1.2f)), true, pActor->Get_Position());	//pActor->Calc_Front_Pos() // 플레이어 앞 위치 계산

		Sound_Impact_Layer1();
		Sound_Impact_Layer2();
		Sound_Hit();


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

	if (m_pEffect != nullptr)	// 이펙트 해제
	{		
		EFFECT_MANAGER->Return_ToPool(m_pEffect);
	}

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

void CPlayer_State_TeleportPunch::Sound_Hit()
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
