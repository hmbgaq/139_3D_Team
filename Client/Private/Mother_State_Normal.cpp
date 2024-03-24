#include "Mother_State_Normal.h"

void CMother_State_Normal::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);
}

CState<CMother>* CMother_State_Normal::Update(CMother* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CMother_State_Normal::Release(CMother* pActor)
{
	__super::Release(pActor);
}

CState<CMother>* CMother_State_Normal::Update_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
