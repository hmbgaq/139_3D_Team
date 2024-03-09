#include "..\Public\Player_Stalker_GrabAttack.h"

#include "Player_Empowered_Idle.h"

void CPlayer_Stalker_GrabAttack::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Stalker_GrabAttack::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Stalker_GrabAttack::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
