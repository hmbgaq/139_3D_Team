#include "..\Public\Player_Winchester_Ironsights_Reload_01.h"
#include "GameInstance.h"

#include "Player_Winchester_Ironsights_AimPose.h"

void CPlayer_Winchester_Ironsights_Reload_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, true);
}

CState<CPlayer>* CPlayer_Winchester_Ironsights_Reload_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Winchester_Ironsights_AimPose();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_Ironsights_Reload_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
