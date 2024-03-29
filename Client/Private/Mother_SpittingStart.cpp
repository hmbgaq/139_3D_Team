#include "Mother_SpittingStart.h"
#include "Mother_SpittingLoop.h"
#include "MotherMouth.h"
#include "MotherMouth_SpittingMineStart.h"

void CMother_SpittingStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_SpittingStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMother_SpittingLoop;
	}
	
	return nullptr;
}

void CMother_SpittingStart::Release(CMother* pActor)
{
	__super::Release(pActor);
	pActor->m_pMonster->Set_Enable(true);
	pActor->m_pMonster->Get_Actor()->Set_State(new CMotherMouth_SpittingMineStart);
}
