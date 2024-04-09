#include "..\Public\Player_InteractionClimb100.h"

void CPlayer_InteractionClimb100::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);
}

CState<CPlayer>* CPlayer_InteractionClimb100::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimb100::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
