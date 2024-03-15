#include "stdafx.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Infected_Walk_F.h"

void CInfected_Walk_F::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_Walk_F::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Walk_F::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
