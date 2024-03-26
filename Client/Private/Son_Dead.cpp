#include "Son_Dead.h"

void CSon_Dead::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CSon>* CSon_Dead::Update(CSon* pActor, _float fTimeDelta)
{
	//애니메이션이 끝나면 삭제 
	if (pActor->Is_Animation_End())
	{
		pActor->Set_Dead(true);
	}

	return nullptr;
}

void CSon_Dead::Release(CSon* pActor)
{
	__super::Release(pActor);
}
