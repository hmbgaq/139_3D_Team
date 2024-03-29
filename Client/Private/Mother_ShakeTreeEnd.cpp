#include "Mother_ShakeTreeEnd.h"
#include "Mother_Idle.h"

void CMother_ShakeTreeEnd::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_ShakeTreeEnd::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_Idle;
	}

	return nullptr;
}

void CMother_ShakeTreeEnd::Release(CMother* pActor)
{
	__super::Release(pActor);
	pActor->m_iSonDead = 0;
	pActor->m_bPhase = false;
}
