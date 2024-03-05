#include "VampireCommander_Taunt5.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_Taunt5::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Taunt5::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}


	return nullptr;
}

void CVampireCommander_Taunt5::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
