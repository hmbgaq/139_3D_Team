#include "Mother_ShakeTreeStart.h"
#include "Mother_ShakeTreeLoop.h"

void CMother_ShakeTreeStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_ShakeTreeStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_ShakeTreeLoop;
	}


	return nullptr;
}

void CMother_ShakeTreeStart::Release(CMother* pActor)
{
	__super::Release(pActor);
}
