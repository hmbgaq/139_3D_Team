#include "Mother_SpittingStart.h"
#include "Mother_SpittingLoop.h"

void CMother_SpittingStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_SpittingStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_SpittingLoop;
	}

	return nullptr;
}

void CMother_SpittingStart::Release(CMother* pActor)
{
	__super::Release(pActor);
}
