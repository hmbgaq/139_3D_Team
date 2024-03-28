#include "Son_RangeAttackFar.h"
#include "Son_Idle.h"

void CSon_RangeAttackFar::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_RangeAttackFar::Update(CSon* pActor, _float fTimeDelta)
{

	//뱉어내기 전에 pActor->m_bLookAt = false;
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_RangeAttackFar::Release(CSon* pActor)
{
	__super::Release(pActor);
}
