#include "..\Public\Player_Winchester_WeaponUnholster.h"
#include "GameInstance.h"

#include "Player_Winchester_Ironsights_AimPose.h"

#include "Data_Manager.h"
#include "MasterCamera.h"
#include "SMath.h"

void CPlayer_Winchester_WeaponUnholster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Reset_UpperAngle();
	pActor->Set_Splitted(true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, true);
	pWeapon->Play_Weapon_Sound_UnHolster();


}

CState<CPlayer>* CPlayer_Winchester_WeaponUnholster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Winchester_Ironsights_AimPose();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_WeaponUnholster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	
	//pActor->Set_Splitted(false);
}
