#include "..\Public\VampireCommander_Walk_BL.h"

void CVampireCommander_Walk_BL::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CVampireCommander>* CVampireCommander_Walk_BL::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireCommander_Walk_BL::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
