#include "..\Public\Hawk_State_Normal.h"

void CHawk_State_Normal::Initialize(CHawk* pActor)
{
	__super::Initialize(pActor);
}

CState<CHawk>* CHawk_State_Normal::Update(CHawk* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CHawk_State_Normal::Release(CHawk* pActor)
{
	__super::Release(pActor);
}

CState<CHawk>* CHawk_State_Normal::Update_State(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;//Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
