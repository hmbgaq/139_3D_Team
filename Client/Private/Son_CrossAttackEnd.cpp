#include "Son_CrossAttackEnd.h"
#include "Son_Idle.h"

void CSon_CrossAttackEnd::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_CrossAttackEnd::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_CrossAttackEnd::Release(CSon* pActor)
{
	__super::Release(pActor);
}
