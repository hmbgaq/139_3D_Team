#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Sprint_F_Melee_02.h"
#include "Infected_Walk_B.h"

void CInfected_Sprint_F_Melee_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	
	switch (m_eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	{
		pActor->Get_Transform()->Set_Speed(13.f);

		m_pWeapon
			->Set_Damage(7)
			->Set_Direction(Direction::Left)
			->Set_Power(Power::Heavy)
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

CState<CInfected>* CInfected_Sprint_F_Melee_02::Update(CInfected* pActor, _float fTimeDelta)
{
	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	{
		if (pActor->Is_Inputable_Front(38))
			m_pWeapon->Set_Enable(true);

		if (pActor->Is_Inputable_Front(49))
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
		pActor->Get_Transform()->Set_Speed(10.f);
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Sprint_F_Melee_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
