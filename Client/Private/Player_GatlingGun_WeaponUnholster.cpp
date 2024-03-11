#include "..\Public\Player_GatlingGun_WeaponUnholster.h"
#include "GameInstance.h"

#include "Player_Bandit_Gatling.h"

void CPlayer_GatlingGun_WeaponUnholster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_GatlingGun_WeaponUnholster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Aim_Walk(fTimeDelta);

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Bandit_Gatling();
	}

	return nullptr;
}

void CPlayer_GatlingGun_WeaponUnholster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
