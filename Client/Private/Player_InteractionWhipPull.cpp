#include "..\Public\Player_InteractionWhipPull.h"

void CPlayer_InteractionWhipPull::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionWhipPull::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionWhipPull::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
