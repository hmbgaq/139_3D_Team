#include "..\Public\Infected_Run_FL.h"

void CInfected_Run_FL::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Run_FL::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Run_FL::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
