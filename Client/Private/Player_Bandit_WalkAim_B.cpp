#include "..\Public\Player_Bandit_WalkAim_B.h"

void CPlayer_Bandit_WalkAim_B::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
}

CState<CPlayer>* CPlayer_Bandit_WalkAim_B::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Go_Backward(fTimeDelta * 0.5f);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Bandit_WalkAim_B::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
