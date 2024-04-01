#include "..\Public\Player_InteractionGlamour_Activate.h"

void CPlayer_InteractionGlamour_Activate::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionGlamour_Activate::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_InteractionGlamour_Activate::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
