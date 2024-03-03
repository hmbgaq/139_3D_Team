#include "..\Public\Bandit_Heavy_State_Normal.h"

void CBandit_Heavy_State_Normal::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Normal::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Heavy_State_Normal::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Normal::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
