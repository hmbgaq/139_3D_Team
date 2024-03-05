#include "VampireCommander_Taunt4.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_Taunt4::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Taunt4::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle;
	}


	return nullptr;
}

void CVampireCommander_Taunt4::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
