#include "Son_StunEnd.h"
#include "Son_Idle.h"

void CSon_StunEnd::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_StunEnd::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_StunEnd::Release(CSon* pActor)
{
	__super::Release(pActor);
}
	