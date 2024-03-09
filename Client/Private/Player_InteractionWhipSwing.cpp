#include "..\Public\Player_InteractionWhipSwing.h"

void CPlayer_InteractionWhipSwing::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionWhipSwing::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionWhipSwing::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
