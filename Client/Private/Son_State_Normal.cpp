#include "Son_State_Normal.h"

void CSon_State_Normal::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);
}

CState<CSon>* CSon_State_Normal::Update(CSon* pActor, _float fTimeDelta)
{
	__super::Update(pActor,fTimeDelta);

	return nullptr;
}

void CSon_State_Normal::Release(CSon* pActor)
{
	__super::Release(pActor);
}

CState<CSon>* CSon_State_Normal::Update_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal_State(pActor, fTimeDelta, _iAnimIndex);
}
