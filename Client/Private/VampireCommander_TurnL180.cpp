#include "VampireCommander_TurnL180.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_TurnL180::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true,false);
}

CState<CVampireCommander>* CVampireCommander_TurnL180::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}
	return nullptr;
}

void CVampireCommander_TurnL180::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
