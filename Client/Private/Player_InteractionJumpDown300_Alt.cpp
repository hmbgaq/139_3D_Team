#include "..\Public\Player_InteractionJumpDown300_Alt.h"

void CPlayer_InteractionJumpDown300_Alt::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionJumpDown300_Alt::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionJumpDown300_Alt::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
