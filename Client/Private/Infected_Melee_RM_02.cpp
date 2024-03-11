#include "Infected_Melee_RM_02.h"

void CInfected_Melee_RM_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	
	switch (m_eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	{
		m_pWeapon
			->Set_Damage(5)
			->Set_Direction(Direction::Left)
			->Set_Power(Power::Medium)
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

CState<CInfected>* CInfected_Melee_RM_02::Update(CInfected* pActor, _float fTimeDelta)
{
	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	{
		if (pActor->Is_Inputable_Front(27))
			m_pWeapon->Set_Enable(true);

		if (pActor->Is_Inputable_Front(37))
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

void CInfected_Melee_RM_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
