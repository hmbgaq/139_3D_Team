#include "Infected_Melee_RM_01.h"

void CInfected_Melee_RM_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	switch (m_eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	{
		m_pWeapon
			->Set_Damage(3)
			->Set_Direction(Direction::Left)
			->Set_Power(Power::Light)
			->Set_Force(0.f);
	}
	break;
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

}

CState<CInfected>* CInfected_Melee_RM_01::Update(CInfected* pActor, _float fTimeDelta)
{
	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	{
		if (pActor->Is_Inputable_Front(30))
			m_pWeapon->Set_Enable(true);

		if (pActor->Is_Inputable_Front(40))
			m_pWeapon->Set_Enable(false);
	}
	break;
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
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

void CInfected_Melee_RM_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
