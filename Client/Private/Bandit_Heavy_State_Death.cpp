#include "..\Public\Bandit_Heavy_State_Death.h"
#include "Data_Manager.h"

void CBandit_Heavy_State_Death::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);
	CData_Manager::GetInstance()->Add_CurEXP(BANDIT_HEAVY_EXP);

	pActor->Play_Sound_Death();
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Death::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Heavy_State_Death::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Death::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Death_State(pActor, fTimeDelta, _iAnimIndex);
}
