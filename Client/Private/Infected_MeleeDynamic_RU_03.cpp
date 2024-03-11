#include "..\Public\Infected_MeleeDynamic_RU_03.h"

void CInfected_MeleeDynamic_RU_03::Initialize(CInfected* pActor)
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
			->Set_Damage(7)
			->Set_Direction(Direction::Left)
			->Set_Power(Power::Heavy)
			->Set_Force(0.f);
	}
	break;

	case CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

}

CState<CInfected>* CInfected_MeleeDynamic_RU_03::Update(CInfected* pActor, _float fTimeDelta)
{
	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		if (pActor->Is_Inputable_Front(23))
			m_pWeapon->Set_Enable(true);

		if (pActor->Is_Inputable_Front(35))
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

void CInfected_MeleeDynamic_RU_03::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
