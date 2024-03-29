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
		return Normal(pActor, fTimeDelta, _iAnimIndex);
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

	if (pActor->Get_Target() && CBanditHeavy_Run_F::g_iAnimIndex != _iAnimIndex)
	{
		return new CBanditHeavy_Run_F();
	}

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Attack(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CCharacter* pTarget = pActor->Get_Target();
	if (nullptr == pTarget)	return nullptr;

	_float fDistance = pTarget->Calc_Distance();
	if (2.f > fDistance)
	{
		_uint iRand = SMath::Random(0,5);
		switch (iRand)
		{
		case 0:
			return new CBanditHeavy_Melee_LD();
		case 1:
			return new CBanditHeavy_Melee_LM();
		case 2:
			return new CBanditHeavy_Melee_LU();
		case 3:
			return new CBanditHeavy_MeleeHeavy();
		case 4:
			return new CBanditHeavy_MeleeHeavy_02();
		default:
			return new CBanditHeavy_Melee_LM();
		}
	}
	else if (8.f > fDistance)
	{
		_uint iRand = SMath::Random(0, 6);
		switch (iRand)
		{
		case 0:
			return new CBanditHeavy_MeleeDynamic_LD();
		case 1:
			return new CBanditHeavy_MeleeDynamic_RD();
		case 2:
			return new CBanditHeavy_MeleeDynamic_LM();
		case 3:
			return new CBanditHeavy_MeleeDynamic_LU();
		case 4:
			return new CBanditHeavy_MeleeDynamicHeavy();
		case 5:
			return new CBanditHeavy_StrongSlamDouble();
		default:
			return new CBanditHeavy_MeleeDynamic_LM();
		}
	}

	return nullptr;
}

