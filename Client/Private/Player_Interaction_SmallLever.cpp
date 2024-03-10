#include "..\Public\Player_Interaction_SmallLever.h"

void CPlayer_Interaction_SmallLever::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Interaction_SmallLever::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Interaction_SmallLever::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
