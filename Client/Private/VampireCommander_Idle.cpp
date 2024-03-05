#include "VampireCommander_Idle.h"

void CVampireCommander_Idle::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CVampireCommander>* CVampireCommander_Idle::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireCommander_Idle::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
