#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Run_F.h"
#include "Infected_Sprint_F_Melee_02.h"

void CInfected_Run_F::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Run_F::Update(CInfected* pActor, _float fTimeDelta)
{
	pActor->Look_At_Target();

	switch (pActor->Get_Info().eType)
	{
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());

		pActor->Get_Transform()->Set_Speed(15.f);

		if (fDist <= pActor->Get_Info().fAttack_Distance + 2.f)
		{
			return new CInfected_Sprint_F_Melee_02();
		}
	}
	break;

	case Client::CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case Client::CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Run_F::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
