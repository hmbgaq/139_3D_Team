#include "..\Public\Player_Winchester_Ironsights_AimPose.h"
#include "GameInstance.h"

#include "Player_Winchester_Ironsights_Reload_01.h"
#include "Player_Winchester_Ironsights_Reload_02.h"
#include "UI_Manager.h"
#include "MasterCamera.h"
#include "Data_Manager.h"
#include "SpringCamera.h"

#include "Renderer.h"

#include "Effect_Manager.h"
#include "Bone.h"
#include "SMath.h"

void CPlayer_Winchester_Ironsights_AimPose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation(CPlayer::Player_State::Player_IdleLoop, CModel::ANIM_STATE_LOOP, true, false);
	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_LOOP);
	//pActor->Reset_UpperAngle();
	pActor->Set_Splitted(true);
	pActor->Set_UseMouseMove(true);
	//pActor->Set_StiffnessRate_Upper(0.5f);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, true);
	
	pWeapon->Play_Weapon_Sound_Aim();
}

CState<CPlayer>* CPlayer_Winchester_Ironsights_AimPose::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::RIFLE);
		_bool bIsCooltimeEnd = pActor->Activate_HUD_Skill(eSelectedHUD);
		if (true == bIsCooltimeEnd)
		{
			CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_WINCHESTER);
			pWeapon->Fire();
			pWeapon->Play_Weapon_Sound_Fire();
			pActor->Apply_Shake_And_Blur(Power::Medium);
			CUI_Manager::GetInstance()->Trigger_Crosshair(true);
			pActor->Activate_ShootingReaction();

			EFFECT_MANAGER->Play_Effect("Player/Revolver/", "Revolver_13.json", pActor);



			return new CPlayer_Winchester_Ironsights_Reload_01();
		}

	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_Ironsights_AimPose::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_UseMouseMove(false);
	
	//pActor->Set_Splitted(false);
}
