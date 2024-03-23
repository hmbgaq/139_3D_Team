#include "..\Public\Infected_OpenStateTimeout.h"

void CInfected_OpenStateTimeout::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_OpenStateTimeout::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_OpenStateTimeout::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
