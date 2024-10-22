#include "..\Public\Infected_IdlePose.h"

void CInfected_IdlePose::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_IdlePose::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_IdlePose::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
