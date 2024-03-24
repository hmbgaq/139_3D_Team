#include "stdafx.h"
#include "Transform.h"
#include "Infected_MeleeDynamic_RU_01.h"

void CInfected_MeleeDynamic_RU_01::Initialize(CInfected* pActor)
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

CState<CInfected>* CInfected_MeleeDynamic_RU_01::Update(CInfected* pActor, _float fTimeDelta)
{
	if (g_iAnimIndex != pActor->Get_CurrentAnimIndex())
		return nullptr;

	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		if (pActor->Is_Inputable_Front(22))
			m_pWeapon->Set_Enable(true);

		if (pActor->Is_Inputable_Front(34))
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

void CInfected_MeleeDynamic_RU_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
