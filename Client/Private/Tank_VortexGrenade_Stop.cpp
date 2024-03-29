#include "..\Public\Tank_VortexGrenade_Stop.h"

void CTank_VortexGrenade_Stop::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_VortexGrenade_Stop::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_VortexGrenade_Stop::Release(CTank* pActor)
{
	__super::Release(pActor);
}
