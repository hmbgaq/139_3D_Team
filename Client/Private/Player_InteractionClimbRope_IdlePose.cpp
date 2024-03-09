#include "..\Public\Player_InteractionClimbRope_IdlePose.h"

void CPlayer_InteractionClimbRope_IdlePose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionClimbRope_IdlePose::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimbRope_IdlePose::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
