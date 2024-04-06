#pragma once

#include "Character_Client.h"
#include "Actor.h"

BEGIN(Engine)
class CPhysXCollider;
END


BEGIN(Client)
//class CUI_Manager;

class CPlayer final : public CCharacter_Client
{

public:
	enum class TeleportPunch_State
	{
		Player_TeleportPunch_L01_Alt,
		Player_TeleportPunch_L01_VeryFar,
		Player_TeleportPunch_L02_Alt,
		Player_TeleportPunch_L03_Alt,

		Player_TeleportPunch_R01_Alt,
		Player_TeleportPunch_R02_Alt,
		Player_TeleportPunch_R02_VeryFar,
		Player_TeleportPunch_R03_Alt,

		TeleportPunch_State_End
	};

	enum class HUD 
	{
		LEFT_TOP,
		LEFT_RIGHT,
		LEFT_BOTTOM,
		LEFT_LEFT,

		RIGHT_TOP,
		RIGHT_RIGHT,
		RIGHT_BOTTOM,
		RIGHT_LEFT,

		HUD_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	_bool Is_Rotate_In_CameraDir() {
		return m_bRotate_In_CameraDir;
	}
	void Set_Rotate_In_CameraDir(_bool _bRotate_In_CameraDir) {
		m_bRotate_In_CameraDir = _bRotate_In_CameraDir;
	}

	void	Set_Navigation(CNavigation* pNavigation);

public:
	void Aim_Walk(_float fTimeDelta);
	void Activate_ShootingReaction(_float fHeight = 20.f);

public:
	string Get_HUD_Tag(HUD eHUD);
	void Set_HUD_MaxCooltime(HUD eHUD, _float fCurrnetCooltime);
	void Set_HUD_Cooltime(HUD eHUD, _float fCurrnetCooltime);
	void Activate_HUD_Skill(HUD eHUD);
	_bool Is_HUD_Cooltime_End(HUD eHUD);
	

public://!For. Interact
	void SetState_InteractJumpDown100();
	void SetState_InteractJumpDown200();
	void SetState_InteractJumpDown300();
	void SetState_InteractVault100();
	void SetState_InteractVault200();
	void SetState_InteractCartRideStart();
	void SetState_InteractCartRideLoop();
	void SetState_InteractCartRideWagonJump();

	void SetState_InteractionPush_Rock_Idle();
	void SetState_InteractionPull_Rock_Idle();


	void SetState_InteractClimb100();
	void SetState_InteractClimb200();
	void SetState_InteractClimb300();
	void SetState_InteractClimb450();
	void SetState_InteractSlide();
	void SetState_InteractSmallLever();
	void SetState_InteractPlankStart();

	void SetState_InteractRopeDown();
	void SetState_InteractClimbRope();

	void SetState_InteractDoorOpen();
	void SetState_InteractDoorPush();

	void SetState_InteractLadderUpStart();

	void SetState_InteractWhipSwing();
	void SetState_InteractWhipPull();

	void SetState_InteractRotationValve();


public:
	void Search_Target(_float fMaxDistance = 10.f);

	void Chasing_Attack(_float fTimeDelta, _float fMaxDistance = 5.f, _uint iCount = 3);

public:
	void		 LeftHUDCoolDown(const string& strUIName, _float fCoolTime);
	_float		 Get_LeftHUDMaxCoolTime(const string& strUIName);

private:
	void		 KeyInput(_float fTimeDelta);
	//CUI_Manager* m_pUIManager = nullptr;
	_bool		 m_bShowOption = false;
	_bool		 m_bShowSkillWindow = false;
	_bool		 m_bShowDiedScreen = false;

public:
	CActor<CPlayer>* Get_Actor() { return m_pActor; }
	void Set_Actor(CActor<CPlayer>* _pActor) { m_pActor = _pActor; }

public:
	virtual void Check_Frustum() override;

public:
	_float Get_ChargingTime() { return m_fChargingTime; };
	void Set_ChargingTime(_float _fChargingTime) { m_fChargingTime = _fChargingTime; }
	void Update_ChargingTime(_float fTimeDelta);

public:
	TeleportPunch_State Get_TeleportPunch_State() { return m_eTeleportPunch_State; };
	void Set_TeleportPunch_State(TeleportPunch_State _eTeleportPunch_State) { m_eTeleportPunch_State = _eTeleportPunch_State; };

public:
	void Slam();
	void Throw_Dynamite();
	void Teleport();

	

public:
	_bool Is_Exist_Ladder_Count() { return m_iLadderCount > 0; }
	void Remove_Ladder_Count() { m_iLadderCount = m_iLadderCount - 1 > 0 ? m_iLadderCount - 1 : 0; };
	void Set_Ladder_Count(_uint _iLadderCount) { m_iLadderCount = _iLadderCount; }

public:
	_bool Is_Interection() { return m_bIsInterection; }
	void Set_Interection(_bool _bIsInterection) { m_bIsInterection = _bIsInterection; }

protected:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Knock(_bool bIsCannonball) override;
	virtual void Hitted_Dead(Power ePower)	override;




private:
	CActor<CPlayer>* m_pActor = { nullptr };
	_bool	m_bRotate_In_CameraDir = { false };

private:
	_float	m_fChargingTime = { 0.f };
	_uint m_iLadderCount = { 0 };
	_bool m_bIsInterection = { false };

	_bool m_bIsActivated_TeleportPunch = { false };
	TeleportPunch_State m_eTeleportPunch_State = { TeleportPunch_State::TeleportPunch_State_End };


public:
	_bool	m_bPlayerCheck = true;

private:
	CPhysXCollider* m_pPhysXCollider = { nullptr };

private:
	_float m_MaxCooltimes[ECast(HUD::HUD_END)];



public:
	/* 원형객체를 생성한다. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Player_State
	{																		
		Player_Beast_Finisher_Phase3_Attack_01										,
		Player_Beast_SyncedAttack_Phase1_Reaction_01								,
		Player_Beast_SyncedAttack_Phase2_Reaction_01								,
		Player_ParasiteMaster_SyncedAttack											,
		Player_L06_Wagon_Jump														,
		Player_GS01_Remover_Jesse_gmp												,
		Player_GetUp																,
		Player_KneelsDown_01														,
		Player_KneelsDown_Blood														,
		Player_Jesse_Turns_back														,
		Player_LS_Cutscene_Gatling_Player											,
		Player_VampireCommander_SyncedAttack										,
		Player_Flail																,
		Player_N_FallLoop															,
		Player_N_Jump																,
		Player_N_Land																,
		Player_Tank_ImpaleAttack													,
		Player_Bandit_Gatling														,
		Player_Bandit_Gatling_Step_L												,
		Player_Bandit_Ironsights_Reload_02											,
		Player_Bandit_Reload_02														,
		Player_Bandit_Special_01													,
		Player_Bandit_InteractionVault100											,
		Player_Bandit_JumpDown_3m													,
		Player_Bandit_RunAim_B														,
		Player_Bandit_RunAim_BL														,
		Player_Bandit_RunAim_BR														,
		Player_Bandit_RunAim_F														,
		Player_Bandit_RunAim_FL														,
		Player_Bandit_RunAim_FR														,
		Player_Bandit_SprintAim_F													,
		Player_Bandit_WalkAim_B														,
		Player_Bandit_WalkAim_BL													,
		Player_Bandit_WalkAim_BR													,
		Player_Bandit_WalkAim_F														,
		Player_Bandit_WalkAim_FL													,
		Player_Bandit_WalkAim_FR													,
		Player_CoOp_Fallen_Loop														,
		Player_CoOp_Fallen_Start													,
		Player_CoOp_Fallen_Stop														,
		Player_Crossbow_Ironsights_AimPose											,
		Player_IdlePose																,
		Player_Crossbow_Ironsights_Idle												,
		Player_Crossbow_Ironsights_Reload											,
		Player_Crossbow_LowerHolster												,
		Player_Crossbow_Ironsights_AO												,
		Player_Finisher_Bandit_01													,
		Player_Finisher_Bandit_02													,
		Player_Finisher_Bandit_03													,
		Player_Finisher_Bandit_04													,
		Player_Finisher_Bandit_05													,
		Player_Finisher_Bandit_06													,
		Player_Finisher_Bandit_ZapperPull											,
		Player_Finisher_Bandit_ZapperPull_02										,
		Player_Finisher_Bandit_ZapperPull_03										,
		Player_Finisher_BanditHeavy													,
		Player_Finisher_BanditHeavy_02												,
		Player_Finisher_BanditHeavy_Vampiric_01										,
		Player_Finisher_BanditHeavy_Vampiric_02										,
		Player_Finisher_BanditHeavy_ZapperLeap										,
		Player_Finisher_BanditHeavy_ZapperPull_01									,
		Player_Finisher_BanditHeavy_ZapperPull_02									,
		Player_Finisher_CommanderLesser												,
		Player_Finisher_Digger														,
		Player_Finisher_Gambit														,
		Player_Finisher_Gambit_ZapperPull											,
		Player_Finisher_Infected_01													,
		Player_Finisher_Infected_02													,
		Player_Finisher_Infected_03													,
		Player_Finisher_Infected_04													,
		Player_Finisher_Infected_ZapperLeap											,
		Player_Finisher_Infected_ZapperPull											,
		Player_Finisher_ParasiteMaster_01											,
		Player_Finisher_Ranger														,
		Player_Finisher_ReverseFoot_01												,
		Player_Finisher_ReverseFoot_02												,
		Player_Finisher_ReverseFoot_03												,
		Player_Finisher_ReverseFoot_04												,
		Player_Finisher_ReverseFoot_05												,
		Player_Finisher_ReverseFoot_ZapperPull										,
		Player_Finisher_Stalker														,
		Player_Finisher_Tank														,
		Player_Finisher_Tentacle													,
		Player_Finisher_VampireCommander_VS											,
		Player_Screamer_Finisher_ZapperPull_01										,
		Player_Screamer_Finisher_ZapperPull_02										,
		Player_Screamer_Finisher_ZapperPull_03										,
		Player_TakedownRevolver_ManHero_01											,
		Player_Trickster_Finisher_01												,
		Player_FlameBelcher_WeaponHolster											,
		Player_FlameBelcher_WeaponUnholster											,
		Player_FlameBelcher_WeaponUnholster_v2										,
		Player_GatlingGun_WeaponHolster												,
		Player_GatlingGun_WeaponUnholster											,
		Player_Grenade_Throw_v2														,
		Player_Grenade_WeaponHolster												,
		Player_Grenade_WeaponUnholster												,
		Player_HunterTool_BlindingRod												,
		Player_HunterTool_Heal_05													,
		Player_HunterTool_RingOfProtection											,
		Player_Revolver_HandCannonHold												,
		Player_Revolver_Hip_ReloadFast_Alt03										,
		Player_Revolver_Ironsights_ReloadFast										,
		Player_Revolver_WeaponHolster												,
		Player_IdlePose_															,
		Player_Revolver_WeaponUnholster												,
		Player_Rifle_IdleWeaponHolster												,
		Player_Rifle_Ironsights_Reload												,
		Player_Shotgun_Fire_LongRange												,
		Player_Shotgun_Fire_ShortRange												,
		Player_Shotgun_WeaponHolster												,
		Player_ShotgunElectric_Fire_ShortRange										,
		Player_Winchester_Ironsights_AimPose										,
		Player_Winchester_Ironsights_Reload_01										,
		Player_Winchester_Ironsights_Reload_02										,
		Player_Winchester_LowerHolster												,
		Player_Winchester_MeleeDynamic												,
		Player_Winchester_WeaponUnholster											,
		Player_Inventory_DefaultPose												,
		Player_Inventory_Empowered_Blast											,
		Player_Inventory_Empowered_DefaultPose										,
		Player_Inventory_Empowered_Nova												,
		Player_Inventory_Empowered_RailGun											,
		Player_Inventory_Gauntlet													,
		Player_Inventory_Lamp														,
		Player_Inventory_Revolver													,
		Player_Inventory_Shotgun													,
		Player_Inventory_Whip														,
		Player_Inventory_Winchester													,
		Player_InventoryRevolver													,
		Player_InventoryRevolverHolster												,
		Player_InventoryRevolverUnholster											,
		Player_InventoryShotgunHolster												,
		Player_InventoryShotgunUnholster											,
		Player_InventoryWinchesterHolster											,
		Player_InventoryWinchesterUnholster											,
		Player_Pivot_B																,
		Player_Pivot_BL																,
		Player_Pivot_F																,
		Player_Pivot_FR																,
		Player_Run_B																,
		Player_Run_BL																,
		Player_Run_BL135															,
		Player_Run_BR																,
		Player_Run_BR135															,
		Player_Run_F																,
		Player_Run_FL																,
		Player_Run_FL45																,
		Player_Run_FR																,
		Player_Run_FR45																,
		Player_Sprint_F																,
		Player_Sprint_F_NEW															,
		Player_Sprint_F_StartLong_NEW												,
		Player_Sprint_F_Transition													,
		Player_Turn_L180															,
		Player_Turn_R180															,
		Player_Walk_B																,
		Player_Walk_BL																,
		Player_Walk_BL135															,
		Player_Walk_BR																,
		Player_Walk_BR135															,
		Player_Walk_F																,
		Player_Walk_FL																,
		Player_Walk_FL45															,
		Player_Walk_FR																,
		Player_Walk_FR45															,
		Player_DeathLight_F_01														,
		Player_DeathLight_F_02														,
		Player_DeathNormal_F_01														,
		Player_DeathNormal_F_02														,
		Player_Dodge_B																,
		Player_Dodge_L																,
		Player_Dodge_R																,
		Player_HitNormal_B															,
		Player_HitNormal_B_Gatling													,
		Player_HitNormal_F															,
		Player_HitNormal_F_02_NEW													,
		Player_HitNormal_F_Gatling													,
		Player_HitNormal_L															,
		Player_HitNormal_L_Gatling													,
		Player_HitNormal_R															,
		Player_HitNormal_R_Gatling													,
		Player_HitPeriodic_01														,
		Player_HitPeriodic_03														,
		Player_HitPeriodic_04														,
		Player_KnockFrontLight_F_02													,
		Player_PushFromBlock_Strong													,
		Player_Roll_B																,
		Player_Roll_F																,
		Player_Roll_L																,
		Player_Roll_R																,
		Player_Crippler_Use															,
		Player_Crossbow_WeaponUnholster_Fast										,
		Player_Sniper_WalkAim_B														,
		Player_Sniper_WalkAim_BL													,
		Player_Sniper_WalkAim_BR													,
		Player_Sniper_WalkAim_F														,
		Player_Sniper_WalkAim_FL													,
		Player_Sniper_WalkAim_FR													,
		Player_William_RevolverFanningLoop_01										,
		Player_William_RevolverFanningLoop_02										,
		Player_William_RevolverFanningStart_01										,
		Player_William_RevolverFanningStart_02										,
		Player_William_RevolverFanningStop_01										,
		Player_William_RevolverFanningStop_02										,
		Player_William_ShotgunShot_01												,
		Player_Empowered_Idle														,
		Player_Empowered_IdlePose													,
		Player_Empowered_MeleeCombo_01												,
		Player_Empowered_MeleeCombo_02												,
		Player_Empowered_MeleeCombo_03												,
		Player_Empowered_Shot														,
		Player_Empowered_SlamGround													,
		Player_Empowered_Start														,
		Player_Empowered_Stop														,
		Player_Empowered_Winchester_IdleFire										,
		Player_Empowered_Winchester_Start											,
		Player_Empowered_Winchester_Stop											,
		Player_Leap_01_Higher														,
		Player_Leap_01_Lower														,
		Player_Leap_05																,
		Player_MeleeCombo_01														,
		Player_MeleeCombo_02														,
		Player_MeleeCombo_02_L_NEW													,
		Player_MeleeCombo_03_SlamAOEJump											,
		Player_MeleeDynamic_04														,
		Player_MeleeKick															,
		Player_MeleeSlashAlt_01														,
		Player_MeleeUppercut_01v2													,
		Player_OpenStateCombo_8hit													,
		Player_TeleportPunch_L01_Alt												,
		Player_TeleportPunch_L01_VeryFar											,
		Player_TeleportPunch_L02_Alt												,
		Player_TeleportPunch_L03_Alt												,
		Player_TeleportPunch_R01_Alt												,
		Player_TeleportPunch_R02_Alt												,
		Player_TeleportPunch_R02_VeryFar											,
		Player_TeleportPunch_R03_Alt												,
		Player_SlamDown_v2															,
		Player_SlamDown_v3															,
		Player_SlamDown_ManHero														,
		Player_MeleeSwap_01															,
		Player_MeleeSwap_02															,
		Player_Leap_Long_Recovery													,
		Player_Leap_Shorter_Recovery												,
		Player_WhipPull																,
		Player_IdleLoop																,
		Player_Interaction_Slide_Sequence											,
		Player_InteractionClimb100													,
		Player_InteractionClimb100_02												,
		Player_InteractionClimb200													,
		Player_InteractionClimb300													,
		Player_InteractionClimb450													,
		Player_InteractionDoorPush													,
		Player_InteractionJumpDown100												,
		Player_InteractionJumpDown200												,
		Player_InteractionJumpDown300												,
		Player_InteractionJumpDown300_Alt											,
		Player_InteractionPushButton												,
		Player_InteractionVault100													,
		Player_InteractionVault200													,
		Player_BeamWalk_IdlePose													,
		Player_BeamWalk_IdlePose_02													,
		Player_BeamWalk_JumpOver													,
		Player_BeamWalk_Loop														,
		Player_BeamWalk_Loop_01														,
		Player_BeamWalk_Loop_02														,
		Player_BeamWalk_Start														,
		Player_BeamWalk_Stop														,
		Player_BeamWalk_Unbalance													,
		Player_Cargo_LedgeStrafe_R_Custom_01										,
		Player_CartRide_JumpOff_01													,
		Player_CartRide_Loop														,
		Player_CartRide_Start														,
		Player_CrouchUnder_End														,
		Player_CrouchUnder_Gate														,
		Player_CrouchUnder_Loop														,
		Player_CrouchUnder_Start													,
		Player_CrouchUseCables														,
		Player_InspectFlameBelcher													,
		Player_Interaction_Destroy													,
		Player_Interaction_Glamour													,
		Player_Interaction_SmallLever												,
		Player_InteractionBattery													,
		Player_InteractionClimbRope_IdlePose										,
		Player_InteractionClimbRope_IdlePose_02										,
		Player_InteractionClimbRope_Loop_01											,
		Player_InteractionClimbRope_Loop_02											,
		Player_InteractionClimbRope_Start											,
		Player_InteractionClimbRope_Stop											,
		Player_InteractionDoor_Open													,
		Player_InteractionGlamour_Activate											,
		Player_InteractionGlamour_End												,
		Player_InteractionLadder_Up_IdlePose										,
		Player_InteractionLadder_Up_Loop											,
		Player_InteractionLadder_Up_Start											,
		Player_InteractionLadder_Up_Stop											,
		Player_InteractionPlank_DodgeR_01											,
		Player_InteractionPlank_Up_IdlePose_01										,
		Player_InteractionPlank_Up_IdlePose_02										,
		Player_InteractionPlank_Up_Loop_01											,
		Player_InteractionPlank_Up_Loop_02											,
		Player_InteractionPlank_Up_Start											,
		Player_InteractionPlank_Up_Stop												,
		Player_InteractionPull														,
		Player_InteractionPull_Rock_End												,
		Player_InteractionPull_Rock_Idle											,
		Player_InteractionPull_Rock_Loop											,
		Player_InteractionPull_Unable												,
		Player_InteractionPull_WindUp												,
		Player_InteractionPush_Rock_End												,
		Player_InteractionPush_Rock_Idle											,
		Player_InteractionPush_Rock_Loop											,
		Player_InteractionPush_Rock_Pull_to_Push									,
		Player_InteractionPush_Rock_Push_to_Pull									,
		Player_InteractionPush_Rock_Start											,
		Player_InteractionPush_Unable												,
		Player_InteractionPush_WindUp												,
		Player_InteractionRope_Down_Loop											,
		Player_InteractionRope_Down_Start											,
		Player_InteractionRope_Down_Stop											,
		Player_InteractionRotateValve_01											,
		Player_InteractionWhipPull													,
		Player_InteractionWhipSwing													,
		Player_LedgeStrafe_Alt_L_IdlePose											,
		Player_LedgeStrafe_Alt_L_Loop												,
		Player_LedgeStrafe_Alt_L_Start												,
		Player_LedgeStrafe_Alt_L_Stop												,
		Player_LedgeStrafe_Alt_L_WalkOver											,
		Player_LedgeStrafe_Alt_L_WalkUnder											,
		Player_LedgeStrafe_Alt_R_IdlePose											,
		Player_LedgeStrafe_Alt_R_Loop												,
		Player_LedgeStrafe_Alt_R_Start												,
		Player_LedgeStrafe_Alt_R_Stop												,
		Player_LedgeStrafe_L_Idle													,
		Player_LedgeStrafe_L_Loop													,
		Player_LedgeStrafe_L_Start													,
		Player_LedgeStrafe_L_Stop													,
		Player_LedgeStrafe_R_Idle													,
		Player_LedgeStrafe_R_Loop													,
		Player_LedgeStrafe_R_Start													,
		Player_LedgeStrafe_R_Stop													,
		Player_LedgeStrafe_StepSlip_L												,
		Player_LongJump_2m															,
		Player_LongJump_4m															,
		Player_LongJump_4m_down														,
		Player_LongJumpStill_2m														,
		Player_LongJumpStill_4m														,
		Player_LongJumpStill_4m_down												,
		Player_OpenLootCrate														,
		Player_OpenLootCrate_NEW													,
		Player_OpenLootCrate_Small													,
		Player_ResetMachine															,
		Player_SqueezeThrough_L_Idle												,
		Player_SqueezeThrough_L_Loop												,
		Player_SqueezeThrough_L_Start												,
		Player_SqueezeThrough_L_Stop												,
		Player_SqueezeThrough_R_Idle												,
		Player_SqueezeThrough_R_Loop												,
		Player_SqueezeThrough_R_Start												,
		Player_SqueezeThrough_R_Stop												,
		Player_Workbench_WeaponUpgrade_01											,
		Player_ZipLine_Loop															,
		Player_ZipLine_Start														,
		Player_ZipLine_Start_to_Loop												,
		Player_ZipLine_Stop															,
		Player_BeamWalk_BJ_01														,
		Player_BeamWalk_Cargo_01_Idle_01											,
		Player_BeamWalk_Cargo_01_Idle_02											,
		Player_BeamWalk_Cargo_01_Idle_03											,
		Player_BeamWalk_Cargo_01_Idle_04											,
		Player_BeamWalk_Cargo_01_Loop_01											,
		Player_BeamWalk_Cargo_01_Loop_02											,
		Player_BeamWalk_Cargo_01_Loop_03											,
		Player_BeamWalk_Cargo_01_Start												,
		Player_BeamWalk_Cargo_01_Stop												,
		Player_BeamWalk_Forest_01													,
		Player_BeamWalk_Tesla_02													,
		Player_LongJump_Canyon_01													,
		Player_MoF_LedgeStrafe_L_RockFall											,
		Player_SqueezeTrough_Dick_01												,
		Player_SqueezeTrough_Raid_01_IdlePose_01									,
		Player_SqueezeTrough_Raid_01_IdlePose_02									,
		Player_SqueezeTrough_Raid_01_IdlePose_03									,
		Player_SqueezeTrough_Raid_01_IdlePose_04									,
		Player_SqueezeTrough_Raid_01_IdlePose_05									,
		Player_SqueezeTrough_Raid_01_IdlePose_06									,
		Player_SqueezeTrough_Raid_01_Loop_L_01										,
		Player_SqueezeTrough_Raid_01_Loop_L_02										,
		Player_SqueezeTrough_Raid_01_Loop_R_01										,
		Player_SqueezeTrough_Raid_01_Loop_R_02										,
		Player_SqueezeTrough_Raid_01_Loop_R_03										,
		Player_SqueezeTrough_Raid_01_Shift_L_R										,
		Player_SqueezeTrough_Raid_01_Start											,
		Player_SqueezeTrough_Raid_01_Stop											,
		Player_SqueezeTrough_Raid_01_Stuck_R_01										,
		Player_SqueezeTrough_Tesla_01												,
		Player_Tesla_LedgeStrafe_L													,
		Player_Tesla_LedgeStrafe_L_02												,
		Player_ZipLine_OF_01_Loop													,
		Player_ZipLine_OF_01_Start													,
		Player_ZipLine_OF_01_Start_to_Loop											,
		Player_ZipLine_OF_01_Stop													,
		Player_MeleeCombo_04														,
		Player_MeleeFollowUp														,
		Player_MeleeInPlace_9Hit													,
		Player_CrowdThrough_L_Loop													,
		Player_CrowdThrough_L_Start													,
		Player_CrowdThrough_L_Stop													,
		Player_EnergyWhip_CloseRange_01												,
		Player_EnergyWhip_Leap														,
		Player_EnergyWhip_LeapShort													,
		Player_EnergyWhip_LongRange													,
		Player_EnergyWhip_Pull														,
		Player_GlamourAdditive														,
		Player_HitHeavy_F_5m														,
		Player_SqueezeThrough_Alt_L_IdlePose										,
		Player_SqueezeThrough_Alt_L_Loop											,
		Player_SqueezeThrough_Alt_L_Start											,
		Player_SqueezeThrough_Alt_L_Stop											,
		Player_SqueezeThrough_Alt_R_IdlePose										,
		Player_SqueezeThrough_Alt_R_Loop											,
		Player_SqueezeThrough_Alt_R_Start											,
		Player_SqueezeThrough_Alt_R_Stop											,
		Player_Turn_L90																,
		Player_Turn_R90																,
		Player_WhipSlam																,
		Player_Uppercut_03_TEMP														,
		Player_Uppercut_04_TEMP														,
		Player_Uppercut_05v3_TEMP													,
		Player_UppercutAOE															,
		Player_DodgeBlink_B_03														,
		Player_DodgeBlink_CCW														,
		Player_DodgeBlink_CW														,
		Player_DodgeBlink_L_03														,
		Player_DodgeBlink_R_03														,
		Player_Inventory_Idle														,
		Player_Digger_StuffAttack													,
		Player_Stalker_GrabAttack													,
		Player_Jesse_SniperIntro													,
		Player_PushFromBlock														,
		Player_SlamTwoHand_TEMP														,
		Player_SpinSlam_TEMP														,
	};

};

END




