#include "..\Public\Bandit_Sniper_State_Death.h"
#include "Data_Manager.h"

void CBandit_Sniper_State_Death::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);
	CData_Manager::GetInstance()->Add_CurEXP(BANDIT_SNIPER_EXP);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Death::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Sniper_State_Death::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Sniper>* CBandit_Sniper_State_Death::Update_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Death_State(pActor, fTimeDelta, _iAnimIndex);
}
