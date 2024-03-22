#include "..\Public\Player_Bandit_Special_01.h"
#include "GameInstance.h"

#include "Player_Bandit_Reload_02.h"

void CPlayer_Bandit_Special_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Bandit_Special_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Bandit_Reload_02();
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Bandit_Special_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
