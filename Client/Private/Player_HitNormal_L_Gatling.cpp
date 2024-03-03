#include "..\Public\Player_HitNormal_L_Gatling.h"

#include "Player_Empowered_Idle.h"

void CPlayer_HitNormal_L_Gatling::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_HitNormal_L_Gatling::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_HitNormal_L_Gatling::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
