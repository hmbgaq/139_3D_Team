#include "VampireCommander_Taunt.h"

void CVampireCommander_Taunt::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
}

CState<CVampireCommander>* CVampireCommander_Taunt::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Taunt::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

CState<CVampireCommander>* CVampireCommander_Taunt::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Taunt_State(pActor, fTimeDelta, _iAnimIndex);
}
