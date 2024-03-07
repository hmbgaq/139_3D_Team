#include "..\Public\VampireCommander_Walk_FL.h"

void CVampireCommander_Walk_FL::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CVampireCommander>* CVampireCommander_Walk_FL::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireCommander_Walk_FL::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
