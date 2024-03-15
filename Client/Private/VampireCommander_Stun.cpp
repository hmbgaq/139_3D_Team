#include "..\Public\VampireCommander_Stun.h"

void CVampireCommander_Stun::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Stun(true);
}

CState<CVampireCommander>* CVampireCommander_Stun::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Stun::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	pActor->Set_Stun(false);
}

CState<CVampireCommander>* CVampireCommander_Stun::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Stun_State(pActor, fTimeDelta, _iAnimIndex);
}
