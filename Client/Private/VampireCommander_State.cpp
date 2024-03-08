#include "VampireCommander_State.h"
#include "GameInstance.h"

#include "VampireCommander_Idle.h"
#include "VampireCommander_BloodRange_Loop.h"
#include "VampireCommander_BloodRange_Start.h"
#include "VampireCommander_BloodRange_Stop.h"
#include "VampireCommander_BloodRange_Stun_Start.h"
#include "VampireCommander_Dead.h"
#include "VampireCommander_HitCenter.h"
#include "VampireCommander_HitLeft.h"
#include "VampireCommander_HitRight.h"
#include "VampireCommander_Leap_Loop.h"
#include "VampireCommander_Leap_Stop.h"
#include "VampireCommander_Leap_Strat.h"
#include "VampireCommander_Melee1.h"
#include "VampireCommander_Melee2.h"
#include "VampireCommander_Ranged1.h"
#include "VampireCommander_Ranged2.h"
#include "VampireCommander_Ranged3.h"
#include "VampireCommander_SyncedAttack.h"
#include "VampireCommander_SyncedAttack_Fail.h"
#include "VampireCommander_Taunt1.h"
#include "VampireCommander_Taunt2.h"
#include "VampireCommander_Taunt3.h"
#include "VampireCommander_Taunt4.h"
#include "VampireCommander_Taunt5.h"
#include "VampireCommander_Walk_F.h"

#include "SMath.h"
void CVampireCommander_State::Initialize(CVampireCommander* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CVampireCommander>* CVampireCommander_State::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CVampireCommander_State::Release(CVampireCommander* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CVampireCommander>* CVampireCommander_State::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Normal_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CVampireCommander>* CVampireCommander_State::Walk_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (50.f < pActor->Calc_Distance())
	{
		return new CVampireCommander_Idle;
	}
	//return new CVampireCommander_Walk_F();
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Attack_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_int iRandom =  SMath::Random(0,3); // 근접용 랜덤 
	_int iRandomRange = SMath::Random(0, 2);//원거리용 랜덤 
	//근접 공격!! 
	if (5.f > pActor->Calc_Distance() && 0.1f < pActor->Calc_Distance())
	{
		switch (iRandom)
		{
		case 0 :
			pActor->m_bLookAt = true;
			return new CVampireCommander_Melee1;
		case 1:
			pActor->m_bLookAt = true;
			return new CVampireCommander_Melee2;
		case 2:
			pActor->m_bLookAt = true;
			return new CVampireCommander_Ranged2;
		case 3:
			pActor->m_bLookAt = true;
			return new CVampireCommander_SyncedAttack_Fail;
		}
	}
	else if (30.f > pActor->Calc_Distance() && 5.f < pActor->Calc_Distance())
	{
		switch (iRandomRange)
		{
		case 0:
			pActor->m_bLookAt = true;
			return new CVampireCommander_Ranged1;
		case 1:
			pActor->m_bLookAt = true;
			return new CVampireCommander_Ranged3;
		case 2:
			pActor->m_bLookAt = true;
			return new CVampireCommander_Leap_Strat;
		}
	}
	
}

CState<CVampireCommander>* CVampireCommander_State::HitNormal_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Stun_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Taunt_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Spawn_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Dead_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Normal(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CVampireCommander>* pState = { nullptr };

	if (50.f < pActor->Calc_Distance())
	{
		pActor->m_bLookAt = true;
		pState = Idle(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}

	else if (50.f > pActor->Calc_Distance() && 30.f < pActor->Calc_Distance())
	{
		pActor->m_bLookAt = true;
		pState = Walk_State(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}

 	if (30.f > pActor->Calc_Distance() && 0.1f < pActor->Calc_Distance())
 	{
 
 		pState = Attack_State(pActor, fTimeDelta, _iAnimIndex);
 		if (pState)	return pState;
 	}
// 
// 	if (7.f > pActor->Calc_Distance() && 0.5f < pActor->Calc_Distance())
// 	{
// 
// 		pState = Attack_State(pActor, fTimeDelta, _iAnimIndex);
// 		if (pState)	return pState;
// 	}
	

	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Idle(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	pActor->m_bLookAt = true;
	return new CVampireCommander_Idle();
}
