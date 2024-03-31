#include "..\Public\Tank_HitNormal_B_01.h"

void CTank_HitNormal_B_01::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_HitNormal_B_01::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_HitNormal_B_01::Release(CTank* pActor)
{
	__super::Release(pActor);
}
