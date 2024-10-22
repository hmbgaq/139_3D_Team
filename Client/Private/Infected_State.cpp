#include "SMath.h"
#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_State.h"
#include "Data_Manager.h"
#include "Body_Infected.h"	

#include "Infected_Idle.h"
#include "Infected_IdlePose.h"
#include "Infected_IdleAct_01.h"
#include "Infected_IdleAct_02.h"
#include "Infected_IdleAct_03.h"
#include "Infected_RelaxedIdleAct_01.h"
#include "Infected_RelaxedIdleAct_02.h"
#include "Infected_RelaxedIdleAct_03.h"

#include "Infected_Walk_B.h"
#include "Infected_Walk_F.h"
#include "Infected_Walk_FL.h"
#include "Infected_Walk_FL45.h"
#include "Infected_Walk_FR.h"
#include "Infected_Walk_FR45.h"

#include "Infected_Run_B.h"
#include "Infected_Run_F.h"
#include "Infected_Run_FL.h"
#include "Infected_Run_FL45.h"
#include "Infected_Run_FR.h"
#include "Infected_Run_FR45.h"
#include "Infected_Sprint_F_01.h"

#include "Infected_Melee_RD_01.h"
#include "Infected_Melee_RM_01.h"
#include "Infected_Melee_RM_02.h"
#include "Infected_Melee_RU_01.h"
#include "Infected_Melee_RU_02.h"

#include "Infected_Dodge_F_01_TEMP.h"
#include "Infected_Dodge_L_01_TEMP.h"
#include "Infected_Dodge_R_01_TEMP.h"

#include "Infected_MeleeDynamic_RU_01.h"
#include "Infected_MeleeDynamic_RU_02.h"
#include "Infected_MeleeDynamic_RU_03.h"

void CInfected_State::Initialize(CInfected* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CInfected>* CInfected_State::Update(CInfected* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CInfected_State::Release(CInfected* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CInfected>* CInfected_State::Normal_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Walk_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Run_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Run(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Attack_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	return nullptr;
}

CState<CInfected>* CInfected_State::Hit_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{ 
	if (pActor->Is_Animation_End())
	{
		return new CInfected_IdlePose();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Knock_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return Normal_State(pActor, fTimeDelta, _iAnimIndex);
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Dodge_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return Normal_State(pActor, fTimeDelta, _iAnimIndex);
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Spawn_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* 몬스터 Init에서 셋팅한 Spawn Animation이 끝나면 도달하는곳 */
	if (pActor->Is_Animation_End()) 
	{
		_int iRandom = SMath::Random(1, 6);

		switch (iRandom)
		{
		case 1:
			return new CInfected_RelaxedIdleAct_03();
			break;
		case 2:
			return new CInfected_IdleAct_01();
			break;
		case 3:
			return new CInfected_IdleAct_02();
			break;
		case 4:
			return new CInfected_IdleAct_03();
			break;
		case 5:
			return new CInfected_RelaxedIdleAct_01();
			break;
		case 6:
			return new CInfected_RelaxedIdleAct_02();
			break;
		default:
			return new CInfected_IdleAct_01();
			break;
		}
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Death_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		if (false == m_bFlags[0] )
		{
			if (ECast(LEVEL::LEVEL_INTRO_BOSS) != m_pGameInstance->Get_NextLevel())
			{
				// 플레이어 15 경험치 얻음 - A, B, C 해당
				// D는 무조건 Release Summoning임. 다른곳에서 안나타나므로 
				CData_Manager::GetInstance()->Add_CurEXP(INFECTED_EXP);
			}
			
			m_bFlags[0] = true;

			pActor->Set_EnemyHUD_Dead();
			pActor->Set_Dead(true);
		}	
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Release_Summoning(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* Death = 경험치 줌. <> Release Summoning = 경험치 안줌 */
	if (false == m_bFlags[0])
	{
		//pActor->Set_CntDead_Time(1.f);
		//pActor->Set_CntDead_Active(true);
		pActor->Set_Dead(true);
		m_bFlags[0] = true;
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Stun_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Finisher_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Electrocute_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return Normal(pActor, fTimeDelta, _iAnimIndex);
	}

	return nullptr;
}

/* 중앙제어 */
CState<CInfected>* CInfected_State::Normal(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_float WalkDistance = pActor->Get_Info().fWalk_Distance;	 // 10.f // 2.f 
	_float AttackDistance = pActor->Get_Info().fAttack_Distance; // 3.5f // 2.f

	CState<CInfected>* pState = { nullptr };

	// 그외일때 
	_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());	// 현재 플레이어와의 거리 계산 

	// 0 ~ Attack ~ Walk  
	if (fDist > WalkDistance)
	{
		pState = Run(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}
	else if (AttackDistance < fDist && fDist <= WalkDistance)
	{
		pState = Walk(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}
	else 
	{
		pState = Attack(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}
	

	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr; /* 현상태 유지 */
}

CState<CInfected>* CInfected_State::Walk(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());

	CInfected::INFECTED_TYPE Type = pActor->Get_Info().Get_Type();

	switch (Type)
	{
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
		pActor->Set_MonsterAttackState(false);
		return new CInfected_Walk_F();
		break;

	case Client::CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case Client::CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Run(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	pActor->Set_MonsterAttackState(false);
	_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());

	CInfected::INFECTED_TYPE Type = pActor->Get_Info().Get_Type();

	switch (Type)
	{
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
			return new CInfected_Run_F();
		break;
	case Client::CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case Client::CInfected::INFECTED_TYPE::INFECTED_WASTER:
			return new CInfected_Sprint_F_01();
		break;
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Attack(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* 현재 플레이어와의 거리 */
	_float fDist = pActor->Calc_Distance(m_pGameInstance->Get_Player());

	/* 몬스터의 Info Desc */
	CInfected::INFECTED_DESC Info = pActor->Get_Info();
	_int iActNumber = Info.RandomNumber % 4;
	pActor->Look_At_Target();

	/* 타입별, 거리별 조절해야함 */
	switch (Info.eType)
	{
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_A:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_B:
	case Client::CInfected::INFECTED_TYPE::INFECTED_VESSEL_C:
		/* fDist = 현재 플레이어와의 거리 */
		if (0.f <= fDist && fDist < Info.fAttack_Distance - 1.5f) // 0 ~ 공격사거리 - 1.5
		{
			pActor->Set_MonsterAttackState(true);

			switch (iActNumber)
			{
			case 1:
				return new CInfected_Melee_RD_01();
				break;
			case 2:
				return new CInfected_Melee_RM_01();
				break;
			case 3:
				return new CInfected_Melee_RM_02();
				break;
			default:
				return new CInfected_Melee_RU_02();
				break;
			}
		}
		else if (Info.fAttack_Distance - 1.5f <= fDist && fDist <= Info.fAttack_Distance) // 공격사거리 - 1.5 ~ 공격사거리
		{
			pActor->Set_MonsterAttackState(true);

			switch (iActNumber)
			{
			case 1:
				return new CInfected_MeleeDynamic_RU_01(); /* 걸어가면서 공격 */
				break;
			case 2:
				return new CInfected_MeleeDynamic_RU_02();
				break;
			case 3:
				return new CInfected_MeleeDynamic_RU_03();
				break;
			default:
				return new CInfected_MeleeDynamic_RU_01();
				break;
			}
		}
		break;

	case Client::CInfected::INFECTED_TYPE::INFECTED_PROTEUS:
		break;
	case Client::CInfected::INFECTED_TYPE::INFECTED_WASTER:
		break;
	}

	return nullptr;
}


CState<CInfected>* CInfected_State::Dodge(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (CInfected::INFECTED_TYPE::INFECTED_PROTEUS == pActor->Get_Info().Get_Type())
	{
		return nullptr; /* 클래스 갑시다 ㅇㅋ 난자러감 */
	}
	else
		return nullptr;

	//if (pActor->Is_Animation_End())
	//{
	//	return new CInfected_Idle();
	//}

	return nullptr;
}

CState<CInfected>* CInfected_State::Patrol(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

//if (fDist >= pActor->Get_Info().fWalk_Distance)
//{
//	pState = Run(pActor, fTimeDelta, _iAnimIndex);
//	if (pState)	return pState;
//}
//else if (pActor->Get_Info().fAttack_Distance <= fDist && fDist < pActor->Get_Info().fWalk_Distance)
//{
//	pState = Walk(pActor, fTimeDelta, _iAnimIndex);
//	if (pState)	return pState;
//}
//else if (0 <= fDist && fDist < pActor->Get_Info().fAttack_Distance)
//{
//	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
//	if (pState)	return pState;
//}
