#include "..\Public\Bandit_Heavy_State_Death.h"

void CBandit_Heavy_State_Death::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);
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
