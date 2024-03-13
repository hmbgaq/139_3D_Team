#include "..\Public\Player_Winchester_Ironsights_AimPose.h"
#include "GameInstance.h"

#include "Player_Winchester_Ironsights_Reload_01.h"
#include "Player_Winchester_Ironsights_Reload_02.h"

void CPlayer_Winchester_Ironsights_AimPose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_LOOP);
	pActor->Set_Splitted(true);
	//pActor->Set_StiffnessRate_Upper(0.5f);

	pActor->Set_Weapon_Enable(WEAPON_WINCHESTER, true);
	//CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Winchester"));
	//pWeapon->Set_Enable(true);
}

CState<CPlayer>* CPlayer_Winchester_Ironsights_AimPose::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		pActor->Activate_ShootingReaction();
		CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Winchester"));
		pWeapon->Fire();
		
		return new CPlayer_Winchester_Ironsights_Reload_01();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_Ironsights_AimPose::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
