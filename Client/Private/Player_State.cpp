#include "..\Public\Player_State.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"
#include "Effect.h"

#pragma region 플레이어 상태 헤더

#include "Player_Empowered_MeleeCombo_01.h"
#include "Player_Empowered_MeleeCombo_02.h"
#include "Player_Empowered_MeleeCombo_03.h"
#include "Player_MeleeCombo_01.h"
#include "Player_MeleeCombo_02.h"
#include "Player_MeleeCombo_02_L_NEW.h"
#include "Player_MeleeCombo_03_SlamAOEJump.h"
#include "Player_MeleeCombo_04.h"
#include "Player_Empowered_Shot.h"
#include "Player_Empowered_SlamGround.h"
#include "Player_Empowered_Idle.h"
#include "Player_IdleLoop.h"
#include "Player_SlamDown_v2.h"
#include "Player_SlamDown_v3.h"
#include "Player_SlamDown_ManHero.h"
#include "Player_DodgeBlink_B_03.h"
#include "Player_DodgeBlink_L_03.h"
#include "Player_DodgeBlink_R_03.h"
#include "Player_Dodge_B.h"
#include "Player_Dodge_L.h"
#include "Player_Dodge_R.h"
#include "Player_Roll_B.h"
#include "Player_Roll_F.h"
#include "Player_Roll_L.h"
#include "Player_Roll_R.h"
#include "Player_DodgeBlink_CW.h"
#include "Player_DodgeBlink_CCW.h"
#include "Player_Sprint_F.h"
#include "Player_Run_B.h"
#include "Player_Run_BL.h"
#include "Player_Run_BL135.h"
#include "Player_Run_BR.h"
#include "Player_Run_BR135.h"
#include "Player_Run_F.h"
#include "Player_Run_FL.h"
#include "Player_Run_FL45.h"
#include "Player_Run_FR.h"
#include "Player_Run_FR45.h"
#include "Player_Walk_B.h"
#include "Player_Walk_BL.h"
#include "Player_Walk_BL135.h"
#include "Player_Walk_BR.h"
#include "Player_Walk_BR135.h"
#include "Player_Walk_F.h"
#include "Player_Walk_FL.h"
#include "Player_Walk_FL45.h"
#include "Player_Walk_FR.h"
#include "Player_Walk_FR45.h"
#include "Player_TeleportPunch_L01_Alt.h"
#include "Player_TeleportPunch_L01_VeryFar.h"
#include "Player_TeleportPunch_L02_Alt.h"
#include "Player_TeleportPunch_L03_Alt.h"
#include "Player_TeleportPunch_R01_Alt.h"
#include "Player_TeleportPunch_R02_Alt.h"
#include "Player_TeleportPunch_R02_VeryFar.h"
#include "Player_TeleportPunch_R03_Alt.h"
#include "Player_HitNormal_B.h"
#include "Player_HitNormal_B_Gatling.h"
#include "Player_HitNormal_F.h"
#include "Player_HitNormal_F_02_NEW.h"
#include "Player_HitNormal_F_Gatling.h"
#include "Player_HitNormal_L.h"
#include "Player_HitNormal_L_Gatling.h"
#include "Player_HitNormal_R.h"
#include "Player_HitNormal_R_Gatling.h"
#include "Player_Rifle_IdleWeaponHolster.h"
#include "Player_Rifle_Ironsights_Fire.h"
#include "Player_Empowered_Winchester_IdleFire.h"
#include "Player_Empowered_Winchester_Start.h"
#include "Player_Empowered_Winchester_Stop.h"
#include "Player_Winchester_Ironsights_AimPose.h"
#include "Player_Winchester_Ironsights_Reload_01.h"
#include "Player_Winchester_Ironsights_Reload_02.h"
#include "Player_Winchester_LowerHolster.h"
#include "Player_Winchester_WeaponUnholster.h"
#include "Player_Winchester_MeleeDynamic.h"
#include "Player_Crossbow_Ironsights_AimPose.h"
#include "Player_Crossbow_Ironsights_AO.h"
#include "Player_Crossbow_Ironsights_Idle.h"
#include "Player_Crossbow_Ironsights_Reload.h"
#include "Player_Crossbow_LowerHolster.h"
#include "Player_Crossbow_WeaponUnholster_Fast.h"
#include "Player_Revolver_HandCannonHold.h"
#include "Player_Revolver_Hip_ReloadFast_Alt03.h"
#include "Player_Revolver_Ironsights_ReloadFast.h"
#include "Player_Revolver_WeaponHolster.h"
#include "Player_Revolver_WeaponUnholster.h"
#include "Player_Bandit_Ironsights_Reload_02.h"
#include "Player_Bandit_Reload_02.h"
#include "Player_Bandit_Special_01.h"
#include "Player_Bandit_SprintAim_F.h"
#include "Player_William_RevolverFanningStart_01.h"
#include "Player_William_RevolverFanningLoop_01.h"
#include "Player_William_RevolverFanningStop_01.h"
#include "Player_William_RevolverFanningStart_02.h"
#include "Player_William_RevolverFanningLoop_02.h"
#include "Player_William_RevolverFanningStop_02.h"
#include "Player_Shotgun_Fire_LongRange.h"
#include "Player_Shotgun_Fire_ShortRange.h"
#include "Player_Shotgun_WeaponHolster.h"
#include "Player_ShotgunElectric_Fire_ShortRange.h"
#include "Player_William_ShotgunShot_01.h"
#include "Player_Bandit_Gatling.h"
#include "Player_Bandit_Gatling_Step_L.h"
#include "Player_GatlingGun_WeaponHolster.h"
#include "Player_GatlingGun_WeaponUnholster.h"
#include "Player_FlameBelcher_WeaponHolster.h"
#include "Player_FlameBelcher_WeaponUnholster.h"
#include "Player_FlameBelcher_WeaponUnholster_v2.h"
#include "Player_Grenade_Throw_v2.h"
#include "Player_Grenade_WeaponHolster.h"
#include "Player_Grenade_WeaponUnholster.h"
#include "Player_MeleeDynamic_04.h"
#include "Player_Leap_01_Lower.h"
#include "Player_Leap_01_Higher.h"
#include "Player_MeleeUppercut_01v2.h"
#include "Player_EnergyWhip_CloseRange_01.h"
#include "Player_EnergyWhip_Leap.h"
#include "Player_EnergyWhip_LeapShort.h"
#include "Player_EnergyWhip_LongRange.h"
#include "Player_EnergyWhip_Pull.h"
#include "Player_OpenStateCombo_8hit.h"
#include "Player_SlamTwoHand_TEMP.h"
#include "Player_MeleeKick.h"
#include "Player_CartRide_Loop.h"
#include "Player_InteractionGlamour_Activate.h"
#include "Player_MeleeSlashAlt_01.h"
#include "Player_MeleeFollowUp.h"
#include "Player_DeathNormal_F_01.h"

#pragma endregion

#include "MasterCamera.h"
#include "Data_Manager.h"
#include "SpringCamera.h"


void CPlayer_State::Initialize(CPlayer* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	pActor->Set_Rotate_In_CameraDir(true);
}

CState<CPlayer>* CPlayer_State::Update(CPlayer* pActor, _float fTimeDelta)
{
	pActor->Set_Rotate_In_CameraDir(true);

	return nullptr;
}

void CPlayer_State::Release(CPlayer* pActor)
{
	Safe_Release(m_pGameInstance);
	pActor->Set_Rotate_In_CameraDir(false);
}

CState<CPlayer>* CPlayer_State::Normal_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Attack_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Walk_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Run_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Dodge_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	//pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	if (true == CData_Manager::GetInstance()->Is_AdditionalSkill_Learned(Additional_Skill::HERO_PUNCH))
	{
		pState = Melee_Dynamic(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}

	pState = Roll(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Roll_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	//pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Hit_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//if (0.f >= pActor->Get_Hp())
	//{
	//	return new CPlayer_DeathNormal_F_01();
	//}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Aim_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CPlayer>* CPlayer_State::Rifle_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//CState<CPlayer>* pState = { nullptr };

	//pState = Rifle(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	if (m_pGameInstance->Mouse_Up(DIM_RB))
	{
		if (CPlayer_Rifle_IdleWeaponHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Rifle_IdleWeaponHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Winchester_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//CState<CPlayer>* pState = { nullptr };

	//pState = Winchester(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	if (m_pGameInstance->Mouse_Up(DIM_RB))
	{
		if (CPlayer_Winchester_LowerHolster::g_iAnimIndex != _iAnimIndex)
		{
			return new CPlayer_Winchester_LowerHolster();
		}

		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		if (pSpringCam)
		{
			pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
		}
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Crossbow_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Up(DIM_RB))
	{
		if (CPlayer_Crossbow_LowerHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Crossbow_LowerHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Revolver_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Up(DIK_E))
	{
		if (CPlayer_Revolver_WeaponHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Revolver_WeaponHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Shotgun_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Up(DIM_RB))
	{
		if (CPlayer_Shotgun_WeaponHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Shotgun_WeaponHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Gatling_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Up(DIM_RB))
	{
		if (CPlayer_GatlingGun_WeaponHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_GatlingGun_WeaponHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::FlameBelcher_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Up(DIM_RB))
	{
		if (CPlayer_FlameBelcher_WeaponHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_FlameBelcher_WeaponHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Grenade_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Down(DIK_G))
	{
		if (CPlayer_Grenade_WeaponHolster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Grenade_WeaponHolster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Interaction_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Death_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//임시 코드
	if (pActor->Is_Animation_End())
	{
		pActor->Set_Invincible(false);
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Finisher_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::EnergyWhip_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::TeleportPunch_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return TeleportPunch(pActor, fTimeDelta, _iAnimIndex);
}

CState<CPlayer>* CPlayer_State::Slam_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{


	return Slam(pActor, fTimeDelta, _iAnimIndex);
}


CState<CPlayer>* CPlayer_State::Normal(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL_TOOL))
	{
		//if (m_pGameInstance->Key_Pressing(DIK_C))
		//{
		//	return new CPlayer_CartRide_Loop();
		//}

		CData_Manager* pDataManager = CData_Manager::GetInstance();

		if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::HEAL))
		{
			pState = Heal(pActor, fTimeDelta, _iAnimIndex);
			if (pState)	return pState;
		}


		pState = EnergyWhip(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;


		if (true == pDataManager->Is_AdditionalWeapon_Acquired(Additional_Weapon::RIFLE))
		{
			pState = Winchester(pActor, fTimeDelta, _iAnimIndex);
			if (pState)	return pState;
		}

		//pState = Crossbow(pActor, fTimeDelta, _iAnimIndex);
		//if (pState)	return pState;

		if (true == pDataManager->Is_AdditionalWeapon_Acquired(Additional_Weapon::REVOLVER))
		{
			pState = Revolver(pActor, fTimeDelta, _iAnimIndex);
			if (pState)	return pState;
		}

		if (true == pDataManager->Is_AdditionalWeapon_Acquired(Additional_Weapon::SHOTGUN))
		{
			pState = Shotgun(pActor, fTimeDelta, _iAnimIndex);
			if (pState)	return pState;
		}

		//pState = Gatilng(pActor, fTimeDelta, _iAnimIndex);
		//if (pState)	return pState;

		//pState = FlameBelcher(pActor, fTimeDelta, _iAnimIndex);
		//if (pState)	return pState;

		pState = Grenade(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;

		pState = Attack(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}


	pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Run(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Walk(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;

}

CState<CPlayer>* CPlayer_State::Walk(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (true == m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		return nullptr;

	if (m_pGameInstance->Key_Pressing(iKeyUp))
	{
		if (m_pGameInstance->Key_Pressing(iKeyLeft))
		{
			if (CPlayer_Walk_FL45::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Walk_FL45();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyRight))
		{
			if (CPlayer_Walk_FR45::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Walk_FR45();
		}
		else
		{
			if (CPlayer_Walk_F::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Walk_F();
		}
	}
	else if (m_pGameInstance->Key_Pressing(iKeyDown))
	{
		if (m_pGameInstance->Key_Pressing(iKeyLeft))
		{
			if (CPlayer_Walk_BL135::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Walk_BL135();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyRight))
		{
			if (CPlayer_Walk_BR135::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Walk_BR135();
		}
		else
		{
			if (CPlayer_Walk_B::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Walk_B();
		}
	}
	else if (m_pGameInstance->Key_Pressing(iKeyLeft))
	{
		if (CPlayer_Walk_FL::g_iAnimIndex != _iAnimIndex)		// 임시
			return new CPlayer_Walk_FL();
	}
	else if (m_pGameInstance->Key_Pressing(iKeyRight))
	{
		if (CPlayer_Walk_FR::g_iAnimIndex != _iAnimIndex)		// 임시
			return new CPlayer_Walk_FR();
	}
	else
	{
		if (CPlayer_IdleLoop::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_IdleLoop();
	}


	return nullptr;
}

CState<CPlayer>* CPlayer_State::Run(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if (m_pGameInstance->Key_Pressing(iKeyUp))
		{
			if (m_pGameInstance->Key_Pressing(iKeyLeft))
			{
				if (CPlayer_Run_FL45::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Run_FL45();
			}
			else if (m_pGameInstance->Key_Pressing(iKeyRight))
			{
				if (CPlayer_Run_FR45::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Run_FR45();
			}
			else
			{
				if (CPlayer_Run_F::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Run_F();
			}
		}
		else if (m_pGameInstance->Key_Pressing(iKeyDown))
		{
			if (m_pGameInstance->Key_Pressing(iKeyLeft))
			{
				if (CPlayer_Run_BL135::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Run_BL135();
			}
			else if (m_pGameInstance->Key_Pressing(iKeyRight))
			{
				if (CPlayer_Run_BR135::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Run_BR135();
			}
			else
			{
				if (CPlayer_Run_B::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Run_B();
			}
		}
		else if (m_pGameInstance->Key_Pressing(iKeyLeft))
		{
			if (CPlayer_Run_FL::g_iAnimIndex != _iAnimIndex)		// 임시
				return new CPlayer_Run_FL();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyRight))
		{
			if (CPlayer_Run_FR::g_iAnimIndex != _iAnimIndex)		// 임시
				return new CPlayer_Run_FR();
		}
		else 
		{
			if (CPlayer_IdleLoop::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_IdleLoop();
		}
		
	}

	return nullptr;

}


CState<CPlayer>* CPlayer_State::Attack(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	CData_Manager* pDataManager = CData_Manager::GetInstance();

	if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::SUPER_CHARGE))
	{
		pState = TeleportPunch(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}
	
	if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::HIT_EIGHT))
	{
		pState = OpenStateCombo_8hit(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}

	if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::QUAKE_PUNCH))
	{
		pState = Slam(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}

	if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::KICK))
	{
		pState = Kick(pActor, fTimeDelta, _iAnimIndex);
		if (pState)	return pState;
	}



	pState = MeleeCombo(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::MeleeCombo(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CData_Manager* pDataManager = CData_Manager::GetInstance();

	if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::UPPER_CUT))
	{
		if (0.3f <= pActor->Get_ChargingTime())
		{
			pActor->Set_ChargingTime(0.f);
			return new CPlayer_MeleeUppercut_01v2();
		}
	}

	CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		switch (eState)
		{
		//case Client::CPlayer::Player_Empowered_MeleeCombo_03:
		//	return new CPlayer_MeleeCombo_04();

		case Client::CPlayer::Player_MeleeCombo_01:
			return new CPlayer_MeleeCombo_02();
		case Client::CPlayer::Player_MeleeCombo_02:
			return new CPlayer_MeleeCombo_02_L_NEW();
		//case Client::CPlayer::Player_MeleeCombo_02_L_NEW:
		//	return new CPlayer_Empowered_MeleeCombo_03();
		case Client::CPlayer::Player_MeleeCombo_02_L_NEW:
			return new CPlayer_MeleeCombo_04();

		//case Client::CPlayer::Player_MeleeCombo_04:
		//	return new CPlayer_MeleeCombo_02();

		case Client::CPlayer::Player_MeleeCombo_04:
			return new CPlayer_MeleeSlashAlt_01();

		case Client::CPlayer::Player_MeleeSlashAlt_01:
			return new CPlayer_MeleeFollowUp();

		case Client::CPlayer::Player_MeleeFollowUp:
			return new CPlayer_MeleeCombo_01();
		}
	}

	if (
		CPlayer_MeleeUppercut_01v2::g_iAnimIndex != _iAnimIndex
		&& CPlayer_MeleeCombo_01::g_iAnimIndex != _iAnimIndex
		&& CPlayer_MeleeCombo_02::g_iAnimIndex != _iAnimIndex
		&& CPlayer_MeleeCombo_02_L_NEW::g_iAnimIndex != _iAnimIndex
		//&& CPlayer_Empowered_MeleeCombo_03::g_iAnimIndex != _iAnimIndex
		&& CPlayer_MeleeCombo_04::g_iAnimIndex != _iAnimIndex
		&& CPlayer_MeleeSlashAlt_01::g_iAnimIndex != _iAnimIndex
		&& CPlayer_MeleeFollowUp::g_iAnimIndex != _iAnimIndex
		)
	{
		if (m_pGameInstance->Mouse_Up(DIM_LB))
		{
			return new CPlayer_MeleeCombo_01();
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}


	return nullptr;
}

CState<CPlayer>* CPlayer_State::Dodge(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Pressing(DIK_SPACE))
	{
		if (true)
		{
			if (m_pGameInstance->Key_Pressing(iKeyDown))
			{
				if (CPlayer_DodgeBlink_B_03::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_DodgeBlink_B_03();
			}
			else if (m_pGameInstance->Key_Pressing(iKeyLeft))
			{
				if (CPlayer_DodgeBlink_L_03::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_DodgeBlink_L_03();
			}
			else if (m_pGameInstance->Key_Pressing(iKeyRight))
			{
				if (CPlayer_DodgeBlink_R_03::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_DodgeBlink_R_03();
			}
			else 
			{
				if (CPlayer_Sprint_F::g_iAnimIndex != _iAnimIndex)
					return new CPlayer_Sprint_F();
			}
		}

		//if (false) // 느림
		//{
		//	if (m_pGameInstance->Key_Pressing(iKeyDown))
		//	{
		//		if (CPlayer_Dodge_B::g_iAnimIndex != _iAnimIndex)
		//			return new CPlayer_Dodge_B();
		//	}
		//	else if (m_pGameInstance->Key_Pressing(iKeyLeft))
		//	{
		//		if (CPlayer_Dodge_L::g_iAnimIndex != _iAnimIndex)
		//			return new CPlayer_Dodge_L();
		//	}
		//	else if (m_pGameInstance->Key_Pressing(iKeyRight))
		//	{
		//		if (CPlayer_Dodge_R::g_iAnimIndex != _iAnimIndex)
		//			return new CPlayer_Dodge_R();
		//	}
		//}


		if (m_pGameInstance->Key_Pressing(iKeyUp))		// 임시
		{
			if (CPlayer_Roll_F::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Roll_F();
		}

	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}


CState<CPlayer>* CPlayer_State::Roll(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_pGameInstance->Key_Pressing(iKeyDown))
		{
			if (CPlayer_Roll_B::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Roll_B();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyLeft))
		{
			if (CPlayer_Roll_L::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Roll_L();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyRight))
		{
			if (CPlayer_Roll_R::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Roll_R();
		}
		else if (m_pGameInstance->Key_Pressing(iKeyUp))
		{
			if (CPlayer_Roll_F::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Roll_F();
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Rifle(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		if (CPlayer_Rifle_Ironsights_Fire::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Rifle_Ironsights_Fire();

	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Winchester(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		if (pSpringCam)
		{
			pSpringCam->Set_CameraOffset(_float3(1.f, 0.3f, -1.7f));
		}
		
		if (CPlayer_Winchester_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Winchester_WeaponUnholster();

	}

// 	else
// 	{
// 		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
// 		if (pSpringCam)
// 		{
// 			pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
// 		}
// 	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Crossbow(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		if (CPlayer_Crossbow_WeaponUnholster_Fast::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Crossbow_WeaponUnholster_Fast();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Revolver(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Pressing(DIK_E))
	{
		//if (CPlayer_William_RevolverFanningStart_01::g_iAnimIndex != _iAnimIndex)
		//	return new CPlayer_William_RevolverFanningStart_01();

		//if (CPlayer_William_RevolverFanningStart_02::g_iAnimIndex != _iAnimIndex)
		//	return new CPlayer_William_RevolverFanningStart_02();

		//if (CPlayer_Bandit_Special_01::g_iAnimIndex != _iAnimIndex)
		//	return new CPlayer_Bandit_Special_01();

		CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::REVOLVER);
		_bool bIsCooltimeEnd = pActor->Is_HUD_Cooltime_End(eSelectedHUD, REVOLVER_DELAY * 2.f);
		if (true == bIsCooltimeEnd) 
		{
			if (CPlayer_Revolver_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Revolver_WeaponUnholster();
		}
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Shotgun(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{

	if (m_pGameInstance->Key_Down(DIK_F))
	{
		_float fCost = -1.f;
		CData_Manager* pDataManager = CData_Manager::GetInstance();
		if (true == pDataManager->Is_AdditionalWeapon_Acquired(Additional_Weapon::SHOTGUN_UPGRADE)) 
		{
			fCost = 0.f;
			//fCost = SHOTGUN_MAXCOOLTIME / 2.f;
		}

		CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::SHOTGUN);
		_bool bIsCooltimeEnd = pActor->Activate_HUD_Skill(eSelectedHUD, fCost);
		if (true == bIsCooltimeEnd)
		{
			if (CPlayer_ShotgunElectric_Fire_ShortRange::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_ShotgunElectric_Fire_ShortRange();
		}

	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Gatilng(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		if (CPlayer_GatlingGun_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_GatlingGun_WeaponUnholster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::FlameBelcher(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		if (CPlayer_FlameBelcher_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_FlameBelcher_WeaponUnholster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Grenade(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Down(DIK_G))
	{
		if (CPlayer_Grenade_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Grenade_WeaponUnholster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::TeleportPunch(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Down(DIK_Z))
	{
		CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::SUPER_CHARGE);
		_bool bIsCooltimeEnd = pActor->Activate_HUD_Skill(eSelectedHUD);
		if (true == bIsCooltimeEnd)
		{
			pActor->Activate_SuperCharge();

			//! 유정 : 슈퍼차지 이펙트 재생
			//EFFECT_MANAGER->Play_Effect("Player/SuperCharge/", "SuperCharge_05.json", nullptr, pActor->Get_Position());
			EFFECT_MANAGER->Play_Effect("Player/SuperCharge/", "SuperCharge_06.json", pActor, false);
			pActor->Play_Sound_SuperCharge_Enter();
		}


	}

	_bool bIsLearned = CData_Manager::GetInstance()->Is_AdditionalSkill_Learned(Additional_Skill::TELEPORT_PUNCH);

	if (
		true == pActor->Is_SuperCharge() 
		&& (m_pGameInstance->Mouse_Down(DIM_LB) || m_pGameInstance->Mouse_Up(DIM_LB)) 
		&& true == bIsLearned
		)
	{
		pActor->Search_Target(30.f);
		CCharacter* pTarget = pActor->Get_Target();
		if (nullptr == pTarget)
			return nullptr;

		CPlayer::TeleportPunch_State eState = pActor->Get_TeleportPunch_State();

		switch (eState)
		{
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_L01_Alt:
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_L01_VeryFar:
			return new CPlayer_TeleportPunch_R02_Alt();
			break;
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_L02_Alt:
			return new CPlayer_TeleportPunch_R03_Alt();
			break;
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_L03_Alt:
			return new CPlayer_TeleportPunch_R01_Alt();
			break;

		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_R01_Alt:
			return new CPlayer_TeleportPunch_L02_Alt();
			break;
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_R02_Alt:
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_R02_VeryFar:
			return new CPlayer_TeleportPunch_L03_Alt();
			break;
		case Client::CPlayer::TeleportPunch_State::Player_TeleportPunch_R03_Alt:
			return new CPlayer_TeleportPunch_L01_Alt();
			break;
		default:
			return new CPlayer_TeleportPunch_L01_Alt();
			break;
		}

	}

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::EnergyWhip(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Down(DIK_Q)) 
	{
		if (CPlayer_EnergyWhip_CloseRange_01::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_EnergyWhip_CloseRange_01();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Melee_Dynamic(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_LB))
	{
		if (CPlayer_Sprint_F::g_iAnimIndex == _iAnimIndex)
			return new CPlayer_Leap_01_Lower();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::OpenStateCombo_8hit(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	pActor->Search_Target(1.f);
	CCharacter* pTarget = pActor->Get_Target();
	if (pTarget && pTarget->Is_ElectrocuteTime())
	{
		if (m_pGameInstance->Mouse_Down(DIM_LB)
			|| m_pGameInstance->Mouse_Pressing(DIM_LB)
			|| m_pGameInstance->Mouse_Up(DIM_LB))
		{
			if (CPlayer_OpenStateCombo_8hit::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_OpenStateCombo_8hit();
		}

	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Slam(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;

	if (m_pGameInstance->Key_Down(DIK_X))
	{
		CData_Manager* pDataManager = CData_Manager::GetInstance();

		switch (eState)
		{
		case CPlayer::Player_SlamDown_v2:
			if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::QUAKE_PUNCH2))
			{
				return new CPlayer_SlamDown_v3();
			}
			
			break;
		case CPlayer::Player_SlamDown_v3:
			if (true == pDataManager->Is_AdditionalSkill_Learned(Additional_Skill::QUAKE_PUNCH3))
			{
				return new CPlayer_SlamTwoHand_TEMP();
			}
			break;
		case CPlayer::Player_SlamTwoHand_TEMP:
			break;
		default:
			CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::SLAM_DOWM);
			_bool bIsCooltimeEnd = pActor->Activate_HUD_Skill(eSelectedHUD);
			if (true == bIsCooltimeEnd)
			{
				return new CPlayer_SlamDown_v2();
			}
			break;
		}
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Kick(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Key_Down(DIK_R))
	{
		CPlayer::HUD eSelectedHUD = pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::KICK);
		_bool bIsCooltimeEnd = pActor->Activate_HUD_Skill(eSelectedHUD);
		if (true == bIsCooltimeEnd)
		{
			if (CPlayer_MeleeKick::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_MeleeKick();
		}
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Heal(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{

	if (m_pGameInstance->Key_Down(DIK_C))
	{
		_bool bIsCootimeEnd = pActor->Activate_HUD_Skill(pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::HEAL)); //CPlayer::HUD::LEFT_RIGHT
		if (true == bIsCootimeEnd)
		{
			if (CPlayer_InteractionGlamour_Activate::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_InteractionGlamour_Activate();
		}
	}

	return nullptr;
}
