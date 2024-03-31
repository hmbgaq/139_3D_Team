#include "..\Public\Bandit_Heavy_State_Hit.h"

void CBandit_Heavy_State_Hit::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

}

CState<CBandit_Heavy>* CBandit_Heavy_State_Hit::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Heavy_State_Hit::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);

}

CState<CBandit_Heavy>* CBandit_Heavy_State_Hit::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Hit_State(pActor, fTimeDelta, _iAnimIndex);
}
