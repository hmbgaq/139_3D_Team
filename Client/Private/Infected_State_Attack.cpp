#include "stdafx.h"
#include "Infected_Walk_B.h"
#include "Infected_State_Attack.h"
#include "Body_Infected.h"
#include "Weapon_Infected.h"
#include "GameInstance.h"

void CInfected_State_Attack::Initialize(CInfected* pActor)
{
	m_eType = pActor->Get_Info().eType;
	m_pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch")); /* 이름 이걸로 공통사항으로 밀어야함 */
	Safe_AddRef(m_pWeapon);

	AttackState_Setting(pActor);

	dynamic_cast<CWeapon_Infected*>(m_pWeapon)->Play_Trail(true);	// 트레일 켜기

	__super::Initialize(pActor);

	pActor->Set_Monster_State(CMonster_Character::Monster_State::ATTACK);
	pActor->Set_MonsterAttackState(true);

	/*_int iRandom = m_pGameInstance->Random_Int(1, 8);
	wstring strSoundKey = L"";

	switch (iRandom)
	{
	case 1:
		strSoundKey = L"Bats_Attack_Start_Layer_01_02.wav";
		break;
	case 2:
		strSoundKey = L"Bats_Attack_Start_Layer_01_03.wav";
		break;
	case 3:
		strSoundKey = L"Bats_Attack_Start_Layer_01_04.wav";
		break;
	case 4:
		strSoundKey = L"digger_attack_melee_whoosh001.wav";
		break;
	case 5:
		strSoundKey = L"digger_attack_melee_whoosh002.wav";
		break;
	case 6:
		strSoundKey = L"digger_attack_melee_whoosh003.wav";
		break;
	case 7:
		strSoundKey = L"digger_attack_melee_whoosh004.wav";
		break;
	case 8:
		strSoundKey = L"digger_attack_melee_whoosh005.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"INFECTED_ATTACK", strSoundKey, SOUND_ENEMY_ATTACK2, 7.f);*/
}

CState<CInfected>* CInfected_State_Attack::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	pActor->Set_MonsterAttackState(true);

	return nullptr;
}

void CInfected_State_Attack::Release(CInfected* pActor)
{
	if (nullptr != m_pWeapon) 
	{
		dynamic_cast<CWeapon_Infected*>(m_pWeapon)->Play_Trail(false); // 트레일 끄기
		m_pWeapon->Set_Enable(false);

		Safe_Release(m_pWeapon);
	}
	
	__super::Release(pActor);
	pActor->Set_MonsterAttackState(false);

	pActor->Set_Monster_State(CMonster_Character::Monster_State::Monster_State_End);
}

void CInfected_State_Attack::AttackState_Setting(CInfected* pActor)
{
	CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_Infected::RENDER_STATE::ATTACK);
}

CState<CInfected>* CInfected_State_Attack::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return new CInfected_Walk_B();
}
