#include "..\Public\Infected_State.h"
#include "GameInstance.h"

#include "Infected_Idle.h"
#include "Infected_IdlePose.h"
#include "Infected_RelaxedIdleAct_01.h"

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

#include "Infected_Melee_RD_01.h"
#include "Infected_Melee_RM_01.h"
#include "Infected_Melee_RM_02.h"
#include "Infected_Melee_RU_01.h"
#include "Infected_Melee_RU_02.h"

#include "Infected_Dodge_F_01_TEMP.h"
#include "Infected_Dodge_L_01_TEMP.h"
#include "Infected_Dodge_R_01_TEMP.h"




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

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Attack_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	//pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Hit_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//CState<CInfected>* pState = { nullptr };

	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}


	return nullptr;
}

CState<CInfected>* CInfected_State::Knock_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Dodge_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Spawn_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Death_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Normal(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Run(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Walk(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Walk(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{

	//if (true == m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	//	return nullptr;

	//if (m_pGameInstance->Key_Pressing(iKeyUp))
	//{
	//	if (m_pGameInstance->Key_Pressing(iKeyLeft))
	//	{
	//		if (CInfected_Walk_FL45::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Walk_FL45();
	//	}
	//	else if (m_pGameInstance->Key_Pressing(iKeyRight))
	//	{
	//		if (CInfected_Walk_FR45::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Walk_FR45();
	//	}
	//	else
	//	{
	//		if (CInfected_Walk_F::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Walk_F();
	//	}
	//}
	//else if (m_pGameInstance->Key_Pressing(iKeyDown))
	//{
	//	if (CInfected_Walk_B::g_iAnimIndex != _iAnimIndex)
	//		return new CInfected_Walk_B();
	//	
	//}
	//else
	//{
	//	if (CInfected_Idle::g_iAnimIndex != _iAnimIndex)
	//		return new CInfected_Idle();
	//}


	return nullptr;
}

CState<CInfected>* CInfected_State::Run(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	//{
	//	if (m_pGameInstance->Key_Pressing(iKeyUp))
	//	{
	//		if (m_pGameInstance->Key_Pressing(iKeyLeft))
	//		{
	//			if (CInfected_Run_FL45::g_iAnimIndex != _iAnimIndex)
	//				return new CInfected_Run_FL45();
	//		}
	//		else if (m_pGameInstance->Key_Pressing(iKeyRight))
	//		{
	//			if (CInfected_Run_FR45::g_iAnimIndex != _iAnimIndex)
	//				return new CInfected_Run_FR45();
	//		}
	//		else
	//		{
	//			if (CInfected_Run_F::g_iAnimIndex != _iAnimIndex)
	//				return new CInfected_Run_F();
	//		}
	//	}
	//	else if (m_pGameInstance->Key_Pressing(iKeyDown))
	//	{
	//		if (CInfected_Run_B::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Run_B();

	//	}
	//	else
	//	{
	//		if (CInfected_Idle::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Idle();
	//	}
	//}

	return nullptr;
}

CState<CInfected>* CInfected_State::Attack(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//if (m_pGameInstance->Mouse_Down(DIM_LB))
	//{
	//	CInfected::Infected_State eState = (CInfected::Infected_State)_iAnimIndex;
	//	switch (eState)
	//	{

	//	case Client::CInfected::Infected_Melee_RD_01:
	//		return new CInfected_Melee_RM_01();
	//		break;
	//	case Client::CInfected::Infected_Melee_RM_01:
	//		return new CInfected_Melee_RM_02();
	//		break;
	//	case Client::CInfected::Infected_Melee_RM_02:
	//		return new CInfected_Melee_RU_01();
	//		break;
	//	case Client::CInfected::Infected_Melee_RU_01:
	//		return new CInfected_Melee_RU_02();
	//		break;
	//	case Client::CInfected::Infected_Melee_RU_02:
	//		return new CInfected_Melee_RD_01();
	//		break;


	//	default:
	//		return new CInfected_Melee_RD_01();
	//		break;

	//	}
	//}

	//if (pActor->Is_Animation_End())
	//{
	//	return new CInfected_Idle();
	//}

	return nullptr;
}


CState<CInfected>* CInfected_State::Dodge(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{

	//if (m_pGameInstance->Key_Pressing(DIK_SPACE))
	//{
	//	if (m_pGameInstance->Key_Pressing(iKeyUp))
	//	{
	//		if (CInfected_Dodge_F_01_TEMP::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Dodge_F_01_TEMP();
	//	}
	//	else if (m_pGameInstance->Key_Pressing(iKeyLeft))
	//	{
	//		if (CInfected_Dodge_L_01_TEMP::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Dodge_L_01_TEMP();
	//	}
	//	else if (m_pGameInstance->Key_Pressing(iKeyRight))
	//	{
	//		if (CInfected_Dodge_R_01_TEMP::g_iAnimIndex != _iAnimIndex)
	//			return new CInfected_Dodge_R_01_TEMP();
	//	}

	//}

	//if (pActor->Is_Animation_End())
	//{
	//	return new CInfected_Idle();
	//}

	return nullptr;
}

