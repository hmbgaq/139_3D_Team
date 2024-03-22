#include "Mother_Hit.h"
#include "Mother_StunLoop.h"
#include "Mother_StunEnd.h"

void CMother_Hit::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_Hit::Update(CMother* pActor, _float fTimeDelta)
{
	//int나 뭔가 확인할수 있는 수단 2개를 둬서 그값이 1개라면 StunLoop로 2개라면 StunEnd로 

	return nullptr;
}

void CMother_Hit::Release(CMother* pActor)
{
	__super::Release(pActor);
}
