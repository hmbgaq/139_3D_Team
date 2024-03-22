#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Run_F.h"
#include "Infected_Sprint_F_Melee_02.h"
#include "Infected_Sprint_F_Melee_03.h"

void CInfected_Run_F::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false); /* 루트애니메이션 사용x */
	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true,false, 30); // 루트애니메이션 사용 false, 30의 키프레임부터 애니메이션 시작 
	
	pActor->Get_Transform()->Set_Speed(2.5f);	/* 움직이는 속도 제어 */
	pActor->Set_StiffnessRate_Upper(3.f);		/* 애니메이션 재생속도 제어 */
}

CState<CInfected>* CInfected_Run_F::Update(CInfected* pActor, _float fTimeDelta)
{
	pActor->Look_At_Target();
	pActor->Go_Straight(fTimeDelta);

	switch (pActor->Get_Info().eType)
	{
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
	{
		_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());

		if (fDist <= pActor->Get_Info().fAttack_Distance + 2.f)
		{
			_int iRemain = pActor->Get_Info().RandomNumber % 2;	

			pActor->Get_Transform()->Set_Speed(10.f);	/* 움직이는 속도 제어 */
			pActor->Set_StiffnessRate_Upper(1.f);		/* 애니메이션 재생속도 제어 */
			pActor->Set_MonsterAttackState(true);

			if ( iRemain == 0 )
				return new CInfected_Sprint_F_Melee_02();
			else 
				return new CInfected_Sprint_F_Melee_03();
		}
	}
	break;

	case Client::CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case Client::CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

	//if (pActor->Is_Inputable_Front(20))
	//	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	return nullptr;
}

void CInfected_Run_F::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
