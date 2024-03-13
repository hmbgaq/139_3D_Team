#include "Bandit_Sniper_State_Cover.h"

void CBandit_Sniper_State_Cover::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Cover::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Sniper_State_Cover::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Cover::Update_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* Idle Pose -> 서서 숨고르는 Idle 2틱으로 넘어감 */
	return Cover_State(pActor, fTimeDelta, _iAnimIndex);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Cover::Sup_Normal_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* Idle 없이 바로 normal함수로 넘어가는용도  */
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}

