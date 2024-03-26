#include "..\Public\Tank_MeleeDynamicShield_04.h"

void CTank_MeleeDynamicShield_04::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_MeleeDynamicShield_04::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_MeleeDynamicShield_04::Release(CTank* pActor)
{
	__super::Release(pActor);
}
