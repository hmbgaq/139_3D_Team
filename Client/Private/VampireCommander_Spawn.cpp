#include "VampireCommander_Spawn.h"

void CVampireCommander_Spawn::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
}

CState<CVampireCommander>* CVampireCommander_Spawn::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Spawn::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

CState<CVampireCommander>* CVampireCommander_Spawn::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Spawn_State(pActor, fTimeDelta, _iAnimIndex);
}
