#include "..\Public\Player_Winchester_Ironsights_AimPose.h"
#include "GameInstance.h"

#include "Player_Winchester_Ironsights_Reload_01.h"
#include "Player_Winchester_Ironsights_Reload_02.h"
#include "UI_Manager.h"
#include "MasterCamera.h"
#include "Data_Manager.h"
#include "SpringCamera.h"

#include "Renderer.h"

void CPlayer_Winchester_Ironsights_AimPose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation(CPlayer::Player_State::Player_IdleLoop, CModel::ANIM_STATE_LOOP, true, false);
	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_LOOP);
	pActor->Set_Splitted(true);
	//pActor->Set_StiffnessRate_Upper(0.5f);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, true);

	//pActor->Reset_UpperAngle();


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
		CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_WINCHESTER);
		pWeapon->Fire();
		CUI_Manager::GetInstance()->Trigger_Crosshair(true);
		m_pGameInstance->Set_RadialBlurTime(0.3f);
		
		return new CPlayer_Winchester_Ironsights_Reload_01();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_Ironsights_AimPose::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	
	//pActor->Set_Splitted(false);
}
