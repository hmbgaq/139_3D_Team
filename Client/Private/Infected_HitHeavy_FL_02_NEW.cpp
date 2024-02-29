#include "..\Public\Infected_HitHeavy_FL_02_NEW.h"

void CInfected_HitHeavy_FL_02_NEW::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
}

CState<CInfected>* CInfected_HitHeavy_FL_02_NEW::Update(CInfected* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_HitHeavy_FL_02_NEW::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
