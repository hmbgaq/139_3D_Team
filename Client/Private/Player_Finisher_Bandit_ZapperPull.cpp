#include "..\Public\Player_Finisher_Bandit_ZapperPull.h"

void CPlayer_Finisher_Bandit_ZapperPull::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_Finisher_Bandit_ZapperPull::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Finisher_Bandit_ZapperPull::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
