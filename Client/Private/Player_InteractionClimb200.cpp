#include "..\Public\Player_InteractionClimb200.h"

void CPlayer_InteractionClimb200::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);
}

CState<CPlayer>* CPlayer_InteractionClimb200::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimb200::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
