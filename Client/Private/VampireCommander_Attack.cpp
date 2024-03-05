#include "VampireCommander_Attack.h"

void CVampireCommander_Attack::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
}

CState<CVampireCommander>* CVampireCommander_Attack::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Attack::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

CState<CVampireCommander>* CVampireCommander_Attack::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);

}
