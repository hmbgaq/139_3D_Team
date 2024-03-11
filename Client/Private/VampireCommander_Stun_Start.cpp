#include "VampireCommander_Stun_Start.h"
#include "VampireCommander_Stun_Loop.h"

void CVampireCommander_Stun_Start::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Stun_Start::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Stun_Loop();
	}

	return nullptr;
}

void CVampireCommander_Stun_Start::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
