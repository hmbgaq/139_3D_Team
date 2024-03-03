#include "..\Public\Player_TeleportPunch_R02_VeryFar.h"

void CPlayer_TeleportPunch_R02_VeryFar::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_TeleportPunch_R02_VeryFar::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return TeleportPunch(pActor, fTimeDelta, g_iAnimIndex);
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_TeleportPunch_R02_VeryFar::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
