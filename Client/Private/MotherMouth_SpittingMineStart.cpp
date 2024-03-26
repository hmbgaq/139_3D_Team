#include "MotherMouth_SpittingMineStart.h"
#include "MotherMouth_SpittingMineLoop.h"

void CMotherMouth_SpittingMineStart::Initialize(CMotherMouth* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->m_bLoopFinished = false;
}

CState<CMotherMouth>* CMotherMouth_SpittingMineStart::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMotherMouth_SpittingMineLoop;
	}

	return nullptr;
}

void CMotherMouth_SpittingMineStart::Release(CMotherMouth* pActor)
{
	__super::Release(pActor);
}
