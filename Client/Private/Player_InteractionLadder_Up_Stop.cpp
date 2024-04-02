#include "..\Public\Player_InteractionLadder_Up_Stop.h"

void CPlayer_InteractionLadder_Up_Stop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Interection(false);
}

CState<CPlayer>* CPlayer_InteractionLadder_Up_Stop::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionLadder_Up_Stop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
