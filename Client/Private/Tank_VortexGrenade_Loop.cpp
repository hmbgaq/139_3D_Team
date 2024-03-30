#include "..\Public\Tank_VortexGrenade_Loop.h"

void CTank_VortexGrenade_Loop::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CTank_VortexGrenade_Loop::Update(CTank* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_VortexGrenade_Loop::Release(CTank* pActor)
{
	__super::Release(pActor);
}
