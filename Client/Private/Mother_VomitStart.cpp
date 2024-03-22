#include "Mother_VomitStart.h"
#include "Mother_VomitLoop.h"

void CMother_VomitStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_VomitStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_VomitLoop;
	}

	return nullptr;
}

void CMother_VomitStart::Release(CMother* pActor)
{
}
