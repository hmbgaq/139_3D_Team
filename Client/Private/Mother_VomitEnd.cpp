#include "Mother_VomitEnd.h"
#include "Mother_Idle.h"

void CMother_VomitEnd::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_VomitEnd::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}

	return nullptr;
}

void CMother_VomitEnd::Release(CMother* pActor)
{
	__super::Release(pActor);
}
