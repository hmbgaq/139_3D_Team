#include "..\Public\Player_InteractionPush_Rock_Start.h"

void CPlayer_InteractionPush_Rock_Start::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionPush_Rock_Start::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionPush_Rock_Start::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
