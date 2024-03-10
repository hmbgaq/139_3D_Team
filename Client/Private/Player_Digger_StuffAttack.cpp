#include "..\Public\Player_Digger_StuffAttack.h"

#include "Player_Empowered_Idle.h"

void CPlayer_Digger_StuffAttack::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Digger_StuffAttack::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Digger_StuffAttack::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
