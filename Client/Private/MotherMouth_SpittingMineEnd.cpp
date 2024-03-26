#include "MotherMouth_SpittingMineEnd.h"
#include "MotherMouth_Idle.h"

void CMotherMouth_SpittingMineEnd::Initialize(CMotherMouth* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMotherMouth>* CMotherMouth_SpittingMineEnd::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CMotherMouth_Idle;
	}

	return nullptr;
}

void CMotherMouth_SpittingMineEnd::Release(CMotherMouth* pActor)
{
	__super::Release(pActor);
	pActor->Set_Enable(false);
}
