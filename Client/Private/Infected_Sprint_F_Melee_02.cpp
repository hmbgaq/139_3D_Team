#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Sprint_F_Melee_02.h"
#include "Infected_Walk_B.h"

void CInfected_Sprint_F_Melee_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	/* 원상복구 */
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

CState<CInfected>* CInfected_Sprint_F_Melee_02::Update(CInfected* pActor, _float fTimeDelta)
{
	if (g_iAnimIndex != pActor->Get_CurrentAnimIndex())
		return nullptr;

	switch (pActor->Get_Info().eType)
	{
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		if ( pActor->Is_Inputable_Front(38) && m_bFlags[0] == false )
		{
			m_bFlags[0] = true;
			m_pWeapon->Set_Enable(true);
		}

		if (pActor->Is_Inputable_Front(49) && m_bFlags[1] == false)
		{
			m_bFlags[1] = true;
			m_pWeapon->Set_Enable(false);
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

void CInfected_Sprint_F_Melee_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
