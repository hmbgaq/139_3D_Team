#include "Mother_StunStart.h"
#include "Mother_StunLoop.h"

void CMother_StunStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_StunStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_StunLoop;
	}

	return nullptr;
}

void CMother_StunStart::Release(CMother* pActor)
{
	__super::Release(pActor);
}
