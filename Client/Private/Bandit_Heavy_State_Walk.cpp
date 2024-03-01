#include "..\Public\Bandit_Heavy_State_Walk.h"

void CBandit_Heavy_State_Walk::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Walk::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Heavy_State_Walk::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Walk::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Walk_State(pActor, fTimeDelta, _iAnimIndex);
}
