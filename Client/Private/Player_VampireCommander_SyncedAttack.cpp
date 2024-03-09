#include "..\Public\Player_VampireCommander_SyncedAttack.h"

#include "Player_Empowered_Idle.h"

void CPlayer_VampireCommander_SyncedAttack::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_VampireCommander_SyncedAttack::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_VampireCommander_SyncedAttack::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
