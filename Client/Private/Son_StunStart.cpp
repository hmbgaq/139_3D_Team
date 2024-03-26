#include "Son_StunStart.h"
#include "Son_StunLoop.h"

void CSon_StunStart::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_StunStart::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_StunLoop;
	}

	return nullptr;
}

void CSon_StunStart::Release(CSon* pActor)
{
	__super::Release(pActor);
}
