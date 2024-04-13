#include "..\Public\Bandit_Heavy_State_Knock.h"

void CBandit_Heavy_State_Knock::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Play_Hitted_Sound();
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Knock::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CBandit_Heavy_State_Knock::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Knock::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Knock_State(pActor, fTimeDelta, _iAnimIndex);
}
