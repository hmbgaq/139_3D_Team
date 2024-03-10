#include "..\Public\Player_HitHeavy_F_5m.h"

#include "Player_Empowered_Idle.h"

void CPlayer_HitHeavy_F_5m::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_HitHeavy_F_5m::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_HitHeavy_F_5m::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
