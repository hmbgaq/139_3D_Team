#include "..\Public\Infected_MeleeDynamic_RU_02.h"
#include "GameInstance.h"

void CInfected_MeleeDynamic_RU_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	switch (m_eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		m_pWeapon
			->Set_Damage(5)
			->Set_Direction(Direction::Left)
			->Set_Power(Power::Medium)
			->Set_Force(0.f);
	}
	break;

	case CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}
}

CState<CInfected>* CInfected_MeleeDynamic_RU_02::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (g_iAnimIndex != pActor->Get_CurrentAnimIndex())
		return nullptr;

	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		if (pActor->Is_Inputable_Front(23)) /* 이상일때 */
		{
			m_pWeapon->Set_Enable(true);
			_int iRandom = m_pGameInstance->Random_Int(1, 8);
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

			m_pGameInstance->Play_Sound(L"INFECTED_ATTACK", strSoundKey, SOUND_ENEMY_ATTACK2, 7.f);
		}

		if (pActor->Is_Inputable_Front(36))
			m_pWeapon->Set_Enable(false);
	}
	break;

	case CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_MeleeDynamic_RU_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
