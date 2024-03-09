#include "..\Public\Player_InteractionClimbRope_Loop_02.h"

void CPlayer_InteractionClimbRope_Loop_02::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionClimbRope_Loop_02::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimbRope_Loop_02::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
