#include "..\Public\Player_TeleportPunch_L01_Alt.h"

void CPlayer_TeleportPunch_L01_Alt::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_TeleportPunch_State(CPlayer::TeleportPunch_State::Player_TeleportPunch_L01_Alt);
}

CState<CPlayer>* CPlayer_TeleportPunch_L01_Alt::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Animation_End()) 
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

}

void CPlayer_TeleportPunch_L01_Alt::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
