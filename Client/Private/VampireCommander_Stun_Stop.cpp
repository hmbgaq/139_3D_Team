#include "VampireCommander_Stun_Stop.h"
#include "VampireCommander_Idle.h"


void CVampireCommander_Stun_Stop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Stun_Stop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}
	return nullptr;
}

void CVampireCommander_Stun_Stop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
