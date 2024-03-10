#include "..\Public\Player_InteractionDoor_Open.h"

void CPlayer_InteractionDoor_Open::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionDoor_Open::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionDoor_Open::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
