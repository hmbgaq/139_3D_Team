#include "..\Public\Player_Bandit_WalkAim_BR.h"

void CPlayer_Bandit_WalkAim_BR::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
}

CState<CPlayer>* CPlayer_Bandit_WalkAim_BR::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Go_Right(fTimeDelta * 0.5f);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Bandit_WalkAim_BR::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
