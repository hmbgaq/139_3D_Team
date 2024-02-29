#include "..\Public\Infected_Walk_FR.h"

void CInfected_Walk_FR::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_Walk_FR::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_Walk_FR::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
