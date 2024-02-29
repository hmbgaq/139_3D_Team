#include "..\Public\Infected_Run_FR45.h"

void CInfected_Run_FR45::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Run_FR45::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Run_FR45::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
