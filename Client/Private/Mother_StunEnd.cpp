#include "Mother_StunEnd.h"
#include "Mother_ShakeTreeStart.h"
#include "Mother_Dead.h"

void CMother_StunEnd::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_StunEnd::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Get_CurHP() <= 0.f)
		return new CMother_Dead;

	if (pActor->Is_Animation_End())
	{
		return new CMother_ShakeTreeStart;
	}

	return nullptr;
}

void CMother_StunEnd::Release(CMother* pActor)
{
	__super::Release(pActor);
	pActor->m_bPhase = false;
}
