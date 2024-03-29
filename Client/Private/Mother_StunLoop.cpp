#include "Mother_StunLoop.h"
#include "Mother_StunEnd.h"

void CMother_StunLoop::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CMother>* CMother_StunLoop::Update(CMother* pActor, _float fTimeDelta)
{
	//구슬같은거 약점으로 다 파괴해야지만 다음으로 넘어감 
	return nullptr;
}

void CMother_StunLoop::Release(CMother* pActor)
{
	__super::Release(pActor);
}	
