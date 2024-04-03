#include "Mother_Dead.h"

void CMother_Dead::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_Dead::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{	//죽여 버리기 여기서 
		pActor->Set_Dead(true);
	}

	return nullptr;
}

void CMother_Dead::Release(CMother* pActor)
{
	__super::Release(pActor);
}
