#include "Son_Hit.h"
#include "Son_Idle.h"

void CSon_Hit::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_Hit::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_Hit::Release(CSon* pActor)
{
	__super::Release(pActor);
}
