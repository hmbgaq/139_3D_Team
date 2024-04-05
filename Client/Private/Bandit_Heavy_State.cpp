#include "Bandit_Heavy_State.h"
#include "GameInstance.h"
#include "SMath.h"
#include "BanditHeavy_Idle.h"
#include "BanditHeavy_Run_F.h"

#include "BanditHeavy_Melee_LD.h"
#include "BanditHeavy_MeleeDynamic_LD.h"
#include "BanditHeavy_MeleeDynamic_RD.h"
#include "BanditHeavy_MeleeDynamic_LM.h"
#include "BanditHeavy_MeleeDynamic_LU.h"
#include "BanditHeavy_Melee_LM.h"
#include "BanditHeavy_Melee_LU.h"
#include "BanditHeavy_MeleeDynamicHeavy.h"
#include "BanditHeavy_MeleeHeavy.h"
#include "BanditHeavy_MeleeHeavy_02.h"
#include "BanditHeavy_StrongSlamDouble.h"
#include "BanditHeavy_Charge_Start.h"


void CBandit_Heavy_State::Initialize(CBandit_Heavy* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CBandit_Heavy_State::Release(CBandit_Heavy* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Normal_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Walk_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Run_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Heavy>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	//pState = Run(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Attack_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, _iAnimIndex);
	}

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Hit_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Heavy>* pState = { nullptr };

	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Knock_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Death_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		pActor->Set_Dead(true);
	}

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Spawn_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Normal(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Heavy>* pState = { nullptr };

	

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Run(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Idle(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Heavy>* pState = { nullptr };

	if (CBanditHeavy_Idle::g_iAnimIndex != _iAnimIndex && pActor->Is_Animation_End())
	{
		pState = new CBanditHeavy_Idle();
	}

	return pState;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Run(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//CState<CBandit_Heavy>* pState = { nullptr };

	if (15.f >= pActor->Calc_Distance() && CBanditHeavy_Run_F::g_iAnimIndex != _iAnimIndex)
	{
		return new CBanditHeavy_Run_F();
	}

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Attack(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_float fDistance = pActor->Calc_Distance();
	if (2.f > fDistance)
	{
		_uint iRand = SMath::Random(0,5);
		_uint iSelectedAnimation;
		switch (iRand)
		{
		case 0:
			iSelectedAnimation = CBanditHeavy_Melee_LD::g_iAnimIndex;
			break;
		case 1:
			iSelectedAnimation = CBanditHeavy_Melee_LM::g_iAnimIndex;
			break;
		case 2:
			iSelectedAnimation = CBanditHeavy_Melee_LU::g_iAnimIndex;
			break;
		case 3:
			iSelectedAnimation = CBanditHeavy_MeleeHeavy::g_iAnimIndex;
			break;
		case 4:
			iSelectedAnimation = CBanditHeavy_MeleeHeavy_02::g_iAnimIndex;
			break;
		default:
			iSelectedAnimation = CBanditHeavy_Melee_LM::g_iAnimIndex;
			break;
		}

		
		if (false == pActor->Compare_PrevState(iSelectedAnimation))
		{
			pActor->Set_PrevState(iSelectedAnimation);

			switch (iSelectedAnimation)
			{
			case CBanditHeavy_Melee_LD::g_iAnimIndex:
				return new CBanditHeavy_Melee_LD();
			case CBanditHeavy_Melee_LM::g_iAnimIndex:
				return new CBanditHeavy_Melee_LM();
			case CBanditHeavy_Melee_LU::g_iAnimIndex:
				return new CBanditHeavy_Melee_LU();
			case CBanditHeavy_MeleeHeavy::g_iAnimIndex:
				return new CBanditHeavy_MeleeHeavy();
			case CBanditHeavy_MeleeHeavy_02::g_iAnimIndex:
				return new CBanditHeavy_MeleeHeavy_02();
			default:
				return new CBanditHeavy_Melee_LM();
			}
		}
		else 
		{
			return new CBanditHeavy_Melee_LM();
		}

	}
	else if (4.f > fDistance)
	{
		_uint iRand = SMath::Random(0, 6);
		_uint iSelectedAnimation;
		switch (iRand)
		{
		case 0:
			iSelectedAnimation = CBanditHeavy_MeleeDynamic_LD::g_iAnimIndex;
			break;
		case 1:
			iSelectedAnimation = CBanditHeavy_MeleeDynamic_RD::g_iAnimIndex;
			break;
		case 2:
			iSelectedAnimation = CBanditHeavy_MeleeDynamic_LM::g_iAnimIndex;
			break;
		case 3:
			iSelectedAnimation = CBanditHeavy_MeleeDynamic_LU::g_iAnimIndex;
			break;
		case 4:
			iSelectedAnimation = CBanditHeavy_MeleeDynamicHeavy::g_iAnimIndex;
			break;
		case 5:
			iSelectedAnimation = CBanditHeavy_StrongSlamDouble::g_iAnimIndex;
			break;
		default:
			iSelectedAnimation = CBanditHeavy_MeleeDynamic_LM::g_iAnimIndex;
			break;
		}

		
		if (false == pActor->Compare_PrevState(iSelectedAnimation))
		{
			pActor->Set_PrevState(iSelectedAnimation);

			switch (iSelectedAnimation)
			{
			case CBanditHeavy_MeleeDynamic_LD::g_iAnimIndex:
				return new CBanditHeavy_MeleeDynamic_LD();
			case CBanditHeavy_MeleeDynamic_RD::g_iAnimIndex:
				return new CBanditHeavy_MeleeDynamic_RD();
			case CBanditHeavy_MeleeDynamic_LM::g_iAnimIndex:
				return new CBanditHeavy_MeleeDynamic_LM();
			case CBanditHeavy_MeleeDynamic_LU::g_iAnimIndex:
				return new CBanditHeavy_MeleeDynamic_LU();
			case CBanditHeavy_MeleeDynamicHeavy::g_iAnimIndex:
				return new CBanditHeavy_MeleeDynamicHeavy();
			case CBanditHeavy_StrongSlamDouble::g_iAnimIndex:
				return new CBanditHeavy_StrongSlamDouble();
			default:
				return new CBanditHeavy_MeleeDynamic_LM();
			}
		}
		else
		{
			return new CBanditHeavy_MeleeDynamic_LM();
		}

	}
	else if (7.f > fDistance)
	{
		if (pActor->Get_AttackCount() > 3.f)
			return new CBanditHeavy_Charge_Start();
	}

	return nullptr;
}

