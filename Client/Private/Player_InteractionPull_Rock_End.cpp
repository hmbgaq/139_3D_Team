#include "..\Public\Player_InteractionPull_Rock_End.h"

void CPlayer_InteractionPull_Rock_End::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionPull_Rock_End::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionPull_Rock_End::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
