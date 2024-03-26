#include "stdafx.h"
#include "Bandit_Sniper_State_Hit.h"

void CBandit_Sniper_State_Hit::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Hit::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Sniper_State_Hit::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Hit::Update_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Hit_State(pActor, fTimeDelta, _iAnimIndex);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Hit::Death_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//pActor->Set_Dead(true);

	/* Test */
	//pActor->Get_Weapon(TEXT("Weapon_Sniper"))->Set_RenderPass(ECast(MONSTER_SHADER::COMMON_DISSOLVE));
	pActor->Get_Body()->Set_RenderPass(ECast(MONSTER_SHADER::COMMON_DISSOLVE));

	return nullptr;
	//return __super::Death_State(pActor, fTimeDelta, _iAnimIndex);
}
