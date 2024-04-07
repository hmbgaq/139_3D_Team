#include "..\Public\Player_Interaction_Slide_Sequence.h"

void CPlayer_Interaction_Slide_Sequence::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(true);
}

CState<CPlayer>* CPlayer_Interaction_Slide_Sequence::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);



	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Interaction_Slide_Sequence::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
