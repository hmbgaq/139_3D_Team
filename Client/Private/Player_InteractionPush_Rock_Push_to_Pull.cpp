#include "..\Public\Player_InteractionPush_Rock_Push_to_Pull.h"

void CPlayer_InteractionPush_Rock_Push_to_Pull::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionPush_Rock_Push_to_Pull::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionPush_Rock_Push_to_Pull::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
