#include "..\Public\Player_CartRide_Loop.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "SpringCamera.h"
#include "Data_Manager.h"
#include "MasterCamera.h"

#include "Player_IdleLoop.h"

void CPlayer_CartRide_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
	pActor->Set_Animation_Upper(CPlayer::Player_State::Player_Winchester_Ironsights_AimPose, CModel::ANIM_STATE_LOOP);
	//Set_Is_Aim(pActor, true);

	//pActor->Set_Weapons_Enable_False();
}

CState<CPlayer>* CPlayer_CartRide_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();

	_bool bIsAim = m_pGameInstance->Mouse_Pressing(DIM_RB);
	

	if (true == bIsAim)
	{
		_uint iAnimIdle = ECast(CPlayer::Player_State::Player_IdleLoop);

		if (iAnimIdle != g_iAnimIndex)
			pActor->Set_Animation(iAnimIdle, CModel::ANIM_STATE_LOOP, true, false);

		Set_Is_Aim(pActor, bIsAim);
		__super::Update(pActor, fTimeDelta);
		
		if (pSpringCam)
			pSpringCam->Set_CameraOffset(_float3(1.f, 0.3f, -1.7f));

		if (m_pGameInstance->Mouse_Down(DIM_LB))
		{
			CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_WINCHESTER);
			pWeapon->Fire();
			pActor->Apply_Shake_And_Blur(Power::Medium);
			CUI_Manager::GetInstance()->Trigger_Crosshair(true);
			pActor->Activate_ShootingReaction();
		}
	}
	else 
	{
		if (pActor->Get_CurrentAnimIndex() != g_iAnimIndex)
			pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);

		Set_Is_Aim(pActor, bIsAim);

		if (pSpringCam)
			pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));

	}

	//if (m_pGameInstance->Key_Pressing(DIK_C))
	//{
	//	return new CPlayer_IdleLoop();
	//}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_CartRide_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	Set_Is_Aim(pActor, false);
}

void CPlayer_CartRide_Loop::Set_Is_Aim(CPlayer* pActor, _bool bFlag)
{
	pActor->Set_Splitted(bFlag);
	pActor->Set_UseMouseMove(bFlag);
	pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, bFlag);
}
