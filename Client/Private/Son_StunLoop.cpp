#include "Son_StunLoop.h"
#include "Son_StunEnd.h"

void CSon_StunLoop::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_StunLoop::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_StunEnd;
	}

	return nullptr;
}

void CSon_StunLoop::Release(CSon* pActor)
{
	__super::Release(pActor);
}
