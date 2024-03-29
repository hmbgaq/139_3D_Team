#include "..\Public\Tank_HitLightShield_F_02.h"

void CTank_HitLightShield_F_02::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_HitLightShield_F_02::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_HitLightShield_F_02::Release(CTank* pActor)
{
	__super::Release(pActor);
}
