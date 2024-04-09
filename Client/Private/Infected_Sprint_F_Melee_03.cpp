#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Walk_B.h"
#include "Infected_Sprint_F_Melee_03.h"

void CInfected_Sprint_F_Melee_03::Initialize(CInfected* pActor)
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

	_float stiff = pActor->Get_StiffnessRate();
	_float speed = pActor->Get_Transform()->Get_Speed();
}

CState<CInfected>* CInfected_Sprint_F_Melee_03::Update(CInfected* pActor, _float fTimeDelta)
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
		if ( pActor->Is_Inputable_Front(37) && m_bFlags[0] == false )
		{
			m_pWeapon->Set_Enable(true);
			m_bFlags[0] = true;
		}
		if ( pActor->Is_Inputable_Front(47) && m_bFlags[1] == false )
		{
			m_pWeapon->Set_Enable(false);
			m_bFlags[1] = true;
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
		pActor->Get_Transform()->Set_Speed(10.f);
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Sprint_F_Melee_03::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
