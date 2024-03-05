#include "VampireCommander_Taunt3.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_Taunt3::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Taunt3::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}


	return nullptr;
}

void CVampireCommander_Taunt3::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
