#include "..\Public\Player_State.h"
#include "GameInstance.h"

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

#pragma endregion



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
	if (m_pGameInstance->Mouse_Up(DIM_RB))
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
	if (m_pGameInstance->Mouse_Up(DIM_RB))
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
		return new CPlayer_IdleLoop();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Finisher_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CPlayer>* CPlayer_State::EnergyWhip_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}


CState<CPlayer>* CPlayer_State::Normal(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CPlayer>* pState = { nullptr };

	//pState = Rifle(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	pState = Winchester(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	//pState = Crossbow(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	//pState = Revolver(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	//pState = Shotgun(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	//pState = Gatilng(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	//pState = FlameBelcher(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	//pState = Grenade(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;




	pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
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

	pState = TeleportPunch(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

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
	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;
		switch (eState)
		{

		case Client::CPlayer::Player_Empowered_MeleeCombo_01:
			return new CPlayer_Empowered_MeleeCombo_02();
			break;
		case Client::CPlayer::Player_Empowered_MeleeCombo_02:
			return new CPlayer_Empowered_MeleeCombo_03();
			break;
		case Client::CPlayer::Player_Empowered_MeleeCombo_03:
			return new CPlayer_MeleeCombo_04();
			break;


		case Client::CPlayer::Player_MeleeCombo_01:
			return new CPlayer_MeleeCombo_02();
			break;
		case Client::CPlayer::Player_MeleeCombo_02:
			return new CPlayer_MeleeCombo_02_L_NEW();
			break;
		case Client::CPlayer::Player_MeleeCombo_02_L_NEW:
			return new CPlayer_Empowered_MeleeCombo_03();
			break;

		case Client::CPlayer::Player_MeleeCombo_03_SlamAOEJump:
			return new CPlayer_MeleeCombo_04();
			break;
		case Client::CPlayer::Player_MeleeCombo_04:
			return new CPlayer_MeleeCombo_02();
			break;



		default:
			return new CPlayer_MeleeCombo_01();
			break;

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
		if (CPlayer_Winchester_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Winchester_WeaponUnholster();

		//if (CPlayer_Winchester_Ironsights_AimPose::g_iAnimIndex != _iAnimIndex)
		//	return new CPlayer_Winchester_Ironsights_AimPose();

		/*CPlayer::Player_State eAnimIndex = (CPlayer::Player_State)_iAnimIndex;

		if (false == pActor->Is_Splitted())
		{
			if (CPlayer_Winchester_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
				return new CPlayer_Winchester_WeaponUnholster();
		}
		else if (pActor->Is_UpperAnimation_End())
		{
			switch (eAnimIndex)
			{
			case CPlayer::Player_State::Player_Winchester_WeaponUnholster:
			case CPlayer::Player_State::Player_Winchester_Ironsights_Reload_01:
			case CPlayer::Player_State::Player_Winchester_Ironsights_Reload_02:
			case CPlayer::Player_State::Player_Winchester_MeleeDynamic:
				return new CPlayer_Winchester_Ironsights_AimPose();

			case CPlayer::Player_State::Player_Empowered_Winchester_IdleFire:
				if (true)
				{
					return new CPlayer_Winchester_Ironsights_AimPose();
				}
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_LB) && eAnimIndex == CPlayer::Player_State::Player_Winchester_Ironsights_AimPose)
		{
			return new CPlayer_Winchester_Ironsights_Reload_01();
		}*/

	}

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
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		if (CPlayer_Revolver_WeaponUnholster::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Revolver_WeaponUnholster();
	}

	return nullptr;
}

CState<CPlayer>* CPlayer_State::Shotgun(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		if (CPlayer_Shotgun_Fire_LongRange::g_iAnimIndex != _iAnimIndex)
			return new CPlayer_Shotgun_Fire_LongRange();
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
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
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
		CPlayer::Player_State eState = (CPlayer::Player_State)_iAnimIndex;
		switch (eState)
		{

		case CPlayer_TeleportPunch_L01_Alt::g_iAnimIndex:
		case CPlayer_TeleportPunch_L01_VeryFar::g_iAnimIndex:
			return new CPlayer_TeleportPunch_R02_Alt();
			break;
		case CPlayer_TeleportPunch_L02_Alt::g_iAnimIndex:
			return new CPlayer_TeleportPunch_R03_Alt();
			break;
		case CPlayer_TeleportPunch_L03_Alt::g_iAnimIndex:
			return new CPlayer_TeleportPunch_R01_Alt();
			break;

		case CPlayer_TeleportPunch_R01_Alt::g_iAnimIndex:
			return new CPlayer_TeleportPunch_L02_Alt();
			break;
		case CPlayer_TeleportPunch_R02_Alt::g_iAnimIndex:
		case CPlayer_TeleportPunch_R02_VeryFar::g_iAnimIndex:
			return new CPlayer_TeleportPunch_L03_Alt();
			break;
		case CPlayer_TeleportPunch_R03_Alt::g_iAnimIndex:
			return new CPlayer_TeleportPunch_L01_Alt();
			break;

		default:
			return new CPlayer_TeleportPunch_L01_Alt();
			break;
		}

	}

	if (m_pGameInstance->Key_Down(DIK_X))
	{
		if (pActor->Is_Animation_End())
		{
			return new CPlayer_IdleLoop();
		}
	}

	

	return nullptr;
}

CState<CPlayer>* CPlayer_State::EnergyWhip(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}
