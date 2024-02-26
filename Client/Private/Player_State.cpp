#include "..\Public\Player_State.h"
#include "GameInstance.h"

#include "Player_Empowered_MeleeCombo_01.h"
#include "Player_Empowered_MeleeCombo_02.h"
#include "Player_Empowered_MeleeCombo_03.h"

#include "Player_MeleeCombo_01.h"
#include "Player_MeleeCombo_02.h"
#include "Player_MeleeCombo_03_SlamAOEJump.h"
#include "Player_MeleeCombo_04.h"


#include "Player_Empowered_Shot.h"
#include "Player_Empowered_SlamGround.h"
#include "Player_SlamDown_v2.h"
#include "Player_SlamDown_v3.h"
#include "Player_SlamDown_ManHero.h"

#include "Player_DodgeBlink_B_03.h"
#include "Player_DodgeBlink_L_03.h"
#include "Player_DodgeBlink_R_03.h"

#include "Player_Dodge_B.h"
#include "Player_Dodge_L.h"
#include "Player_Dodge_R.h"

#include "Player_Roll_B.h"
#include "Player_Roll_F.h"
#include "Player_Roll_L.h"
#include "Player_Roll_R.h"


#include "Player_DodgeBlink_CW.h"
#include "Player_DodgeBlink_CCW.h"

#include "Player_Run_B.h"
#include "Player_Run_BL.h"
#include "Player_Run_BL135.h"
#include "Player_Run_BR.h"
#include "Player_Run_BR135.h"

#include "Player_Run_F.h"
#include "Player_Run_FL.h"
#include "Player_Run_FL45.h"
#include "Player_Run_FR.h"
#include "Player_Run_FR45.h"


#include "Player_Empowered_Idle.h"
#include "Player_IdleLoop.h"


void CPlayer_State::Initialize(CPlayer* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CPlayer>* CPlayer_State::Update(CPlayer* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CPlayer_State::Release(CPlayer* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CPlayer>* CPlayer_State::Normal_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Attack_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Run_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Dodge_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	//pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	pState = Roll(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Roll_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	//pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}




CState<CPlayer>* CPlayer_State::Normal(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Run(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;

}

CState<CPlayer>* CPlayer_State::Run(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Pressing(iKeyUp))
	{
		if (m_pGameInstance->Key_Pressing(iKeyLeft))
		{
			if (CPlayer_Run_FL45::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Run_FL45();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyRight))
		{
			if (CPlayer_Run_FR45::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Run_FR45();
		}
		else
		{
			if (CPlayer_Run_F::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Run_F();
		}
	}
	else if (m_pGameInstance->Key_Pressing(iKeyDown))
	{
		if (m_pGameInstance->Key_Pressing(iKeyLeft))
		{
			if (CPlayer_Run_BL135::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Run_BL135();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyRight))
		{
			if (CPlayer_Run_BR135::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Run_BR135();
		}
		else
		{
			if (CPlayer_Run_B::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Run_B();
		}
	}
	else if (m_pGameInstance->Key_Pressing(iKeyLeft))
	{
		if (CPlayer_Run_FL::g_iAnimIndex != _iAnimIndex)		// 임시
			return new CPlayer_Run_FL();
	}
	else if (m_pGameInstance->Key_Pressing(iKeyRight))	
	{
		if (CPlayer_Run_FR::g_iAnimIndex != _iAnimIndex)		// 임시
			return new CPlayer_Run_FR();
	}
	else
	{
		if (CPlayer_IdleLoop::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_IdleLoop();
	}


	return nullptr;
}


CState<CPlayer>* CPlayer_State::Attack(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = MeleeCombo(pActor, fTimeDelta, _iAnimIndex);

	if (pState)
		return pState;

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::MeleeCombo(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;
		switch (eState)
		{

		case Client::CPlayer::Player_Empowered_MeleeCombo_01:
			return new CPlayer_Empowered_MeleeCombo_02();
			break;
		case Client::CPlayer::Player_Empowered_MeleeCombo_02:
			return new CPlayer_Empowered_MeleeCombo_03();
			break;
		case Client::CPlayer::Player_Empowered_MeleeCombo_03:
			return new CPlayer_Empowered_Shot();
			break;


		case Client::CPlayer::Player_MeleeCombo_01:
			return new CPlayer_MeleeCombo_02();
			break;
		case Client::CPlayer::Player_MeleeCombo_02:
			return new CPlayer_MeleeCombo_03_SlamAOEJump();
			break;
		case Client::CPlayer::Player_MeleeCombo_03_SlamAOEJump:
			return new CPlayer_MeleeCombo_04();
			break;
		case Client::CPlayer::Player_MeleeCombo_04:
			break;



		default:
			return new CPlayer_MeleeCombo_01();
			break;

		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}


	return nullptr;
}

CState<CPlayer>* CPlayer_State::Dodge(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Pressing(DIK_SPACE))
	{
		if (true)
		{
			if (m_pGameInstance->Key_Pressing(iKeyDown))
			{
				if (CPlayer_DodgeBlink_B_03::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_DodgeBlink_B_03();
			}
			else if (m_pGameInstance->Key_Pressing(iKeyLeft))
			{
				if (CPlayer_DodgeBlink_L_03::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_DodgeBlink_L_03();
			}
			else if (m_pGameInstance->Key_Pressing(iKeyRight))
			{
				if (CPlayer_DodgeBlink_R_03::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_DodgeBlink_R_03();
			}
		}

		//if (false) // 느림
		//{
		//	if (m_pGameInstance->Key_Pressing(iKeyDown))
		//	{
		//		if (CPlayer_Dodge_B::g_iAnimIndex != _iAnimIndex)
		//			return new CPlayer_Dodge_B();
		//	}
		//	else if (m_pGameInstance->Key_Pressing(iKeyLeft))
		//	{
		//		if (CPlayer_Dodge_L::g_iAnimIndex != _iAnimIndex)
		//			return new CPlayer_Dodge_L();
		//	}
		//	else if (m_pGameInstance->Key_Pressing(iKeyRight))
		//	{
		//		if (CPlayer_Dodge_R::g_iAnimIndex != _iAnimIndex)
		//			return new CPlayer_Dodge_R();
		//	}
		//}


		if (m_pGameInstance->Key_Pressing(iKeyUp))		// 임시
		{
			if (CPlayer_Roll_F::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Roll_F();
		}

	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}


CState<CPlayer>* CPlayer_State::Roll(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		switch (eState)
		{
		case CPlayer_DodgeBlink_B_03::g_iAnimIndex:
		case CPlayer_Dodge_B::g_iAnimIndex:
			return new CPlayer_Roll_B();
			break;

		case CPlayer_DodgeBlink_L_03::g_iAnimIndex:
		case CPlayer_Dodge_L::g_iAnimIndex:
			return new CPlayer_Roll_L();
			break;

		case CPlayer_DodgeBlink_R_03::g_iAnimIndex:
		case CPlayer_Dodge_R::g_iAnimIndex:
			return new CPlayer_Roll_R();
			break;

		default:
			return new CPlayer_Roll_F();
			break;

		}

	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}
