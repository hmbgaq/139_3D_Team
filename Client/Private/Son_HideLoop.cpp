#include "Son_HideLoop.h"
#include "Son_Spawn.h"

void CSon_HideLoop::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CSon>* CSon_HideLoop::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Spawn;
	}

	return nullptr;
}

void CSon_HideLoop::Release(CSon* pActor)
{
	__super::Release(pActor);
}
