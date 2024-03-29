#include "..\Public\Tank_Burn_Loop.h"

void CTank_Burn_Loop::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_Burn_Loop::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Burn_Loop::Release(CTank* pActor)
{
	__super::Release(pActor);
}
