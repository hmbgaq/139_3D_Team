#include "Son_CrossAttackLoop.h"
#include "Son_CrossAttackEnd.h"

void CSon_CrossAttackLoop::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_CrossAttackLoop::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_CrossAttackEnd;
	}

	return nullptr;
}
	
void CSon_CrossAttackLoop::Release(CSon* pActor)
{
	__super::Release(pActor);
}
