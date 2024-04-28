#include "Infected_Melee_RU_01.h"
#include "GameInstance.h"

void CInfected_Melee_RU_01::Initialize(CInfected* pActor)
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
			->Set_Damage(3)
			->Set_Direction(Direction::Left)
			->Set_Power(Power::Light)
			->Set_Force(0.f);
	}
	break;

	case CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}
}

CState<CInfected>* CInfected_Melee_RU_01::Update(CInfected* pActor, _float fTimeDelta)
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
		if (pActor->Is_Inputable_Front(27) && m_bFlags[1] == false)
		{
			m_pWeapon->Set_Enable(true);
			m_pGameInstance->Play_Sound(L"INFECTED_ATTACK", L"digger_attack_melee_whoosh001.wav", SOUND_ENEMY_ATTACK2, 7.f);
			m_bFlags[1] = true;
		}

		if (pActor->Is_Inputable_Front(37) && m_bFlags[2] == false)
		{
			m_pWeapon->Set_Enable(false);
			m_bFlags[2] = true;
		}
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

void CInfected_Melee_RU_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
