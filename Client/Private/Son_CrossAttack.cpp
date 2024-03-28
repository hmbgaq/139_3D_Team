#include "Son_CrossAttack.h"
#include "Son_CrossAttackLoop.h"

void CSon_CrossAttack::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->m_bLookAt = false;
}

CState<CSon>* CSon_CrossAttack::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_CrossAttackLoop;
	}

	return nullptr;
}

void CSon_CrossAttack::Release(CSon* pActor)
{
	__super::Release(pActor);
}
