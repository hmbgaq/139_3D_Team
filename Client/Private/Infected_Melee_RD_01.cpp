#include "Infected_Melee_RD_01.h"
#include "Collider.h"

void CInfected_Melee_RD_01::Initialize(CInfected* pActor)
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

CState<CInfected>* CInfected_Melee_RD_01::Update(CInfected* pActor, _float fTimeDelta)
{
	if (g_iAnimIndex != pActor->Get_CurrentAnimIndex())
		return nullptr;

	switch (m_eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		if (pActor->Is_Inputable_Front(30))
			m_pWeapon->Set_Enable(true);

		if (pActor->Is_Inputable_Front(41))
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

		pActor->Get_Collider()->Set_Enable(true);
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Melee_RD_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
