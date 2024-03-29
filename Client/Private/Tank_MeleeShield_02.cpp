#include "..\Public\Tank_MeleeShield_02.h"

void CTank_MeleeShield_02::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_MeleeShield_02::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_MeleeShield_02::Release(CTank* pActor)
{
	__super::Release(pActor);
}
