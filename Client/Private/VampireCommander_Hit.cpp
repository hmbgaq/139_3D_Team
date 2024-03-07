#include "VampireCommander_Hit.h"

void CVampireCommander_Hit::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
}

CState<CVampireCommander>* CVampireCommander_Hit::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Hit::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

CState<CVampireCommander>* CVampireCommander_Hit::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return HitNormal_State(pActor, fTimeDelta, _iAnimIndex);
}
