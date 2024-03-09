#include "VampireCommander_Stun_Loop.h"

void CVampireCommander_Stun_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CVampireCommander>* CVampireCommander_Stun_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//마지막에 때려서 맞으면 루프 상태에서 강제로 Stun_Stop으로 변경 시킬 거임 
	return nullptr;
}

void CVampireCommander_Stun_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
