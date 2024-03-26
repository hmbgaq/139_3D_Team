#include "..\Public\Tank_ShieldRegain.h"

void CTank_ShieldRegain::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_ShieldRegain::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_ShieldRegain::Release(CTank* pActor)
{
	__super::Release(pActor);
}
