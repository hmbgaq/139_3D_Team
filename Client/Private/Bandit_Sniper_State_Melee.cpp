#include "..\Public\Bandit_Sniper_State_Melee.h"

void CBandit_Sniper_State_Melee::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Melee::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Sniper_State_Melee::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Melee::Update_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Melee_State(pActor, fTimeDelta, _iAnimIndex);
}
