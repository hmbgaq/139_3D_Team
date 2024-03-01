#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CBandit_Heavy final : public CMonster_Character
{
private:
	CBandit_Heavy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBandit_Heavy(const CBandit_Heavy& rhs);
	virtual ~CBandit_Heavy() = default;

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

private:
	CActor<CBandit_Heavy>* m_pActor = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CBandit_Heavy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Bandit_Heavy_State
	{
		BanditHeavy_Charge_Attack_Full						  ,
		BanditHeavy_Charge_Start							  ,
		BanditHeavy_HammerThrow								  ,
		BanditHeavy_Melee_LD								  ,
		BanditHeavy_Melee_LM								  ,
		BanditHeavy_Melee_LU								  ,
		BanditHeavy_MeleeDynamic_LD							  ,
		BanditHeavy_MeleeDynamic_LM							  ,
		BanditHeavy_MeleeDynamic_LU							  ,
		BanditHeavy_MeleeDynamic_RD							  ,
		BanditHeavy_MeleeDynamicHeavy						  ,
		BanditHeavy_MeleeHeavy								  ,
		BanditHeavy_MeleeHeavy_02							  ,
		BanditHeavy_ClimbUp									  ,
		BanditHeavy_Idle									  ,
		BanditHeavy_IdleAct_03								  ,
		BanditHeavy_IdleAct_04								  ,
		BanditHeavy_IdleAct_05								  ,
		BanditHeavy_IdleAct_07								  ,
		BanditHeavy_IdlePose								  ,
		BanditHeavy_JumpDown_200_forward					  ,
		BanditHeavy_JumpDown_300_Forward					  ,
		BanditHeavy_RelaxedIdleAct_01						  ,
		BanditHeavy_RelaxedIdleAct_02						  ,
		BanditHeavy_RelaxedIdleAct_03						  ,
		BanditHeavy_RelaxedIdleAct_04						  ,
		BanditHeavy_RelaxedIdleAct_05						  ,
		BanditHeavy_RelaxedIdleAct_06						  ,
		BanditHeavy_RelaxedIdleAct_07						  ,
		BanditHeavy_RelaxedIdleAct_08						  ,
		BanditHeavy_RelaxedIdleAct_Special_01				  ,
		BanditHeavy_RelaxedIdleAct_Special_02				  ,
		BanditHeavy_RelaxedIdleAct_Special_Stop				  ,
		BanditHeavy_Run_B									  ,
		BanditHeavy_Run_BL									  ,
		BanditHeavy_Run_BR									  ,
		BanditHeavy_Run_F									  ,
		BanditHeavy_Run_FL									  ,
		BanditHeavy_Run_FR									  ,
		BanditHeavy_SpawnGround								  ,
		BanditHeavy_Sprint_F								  ,
		BanditHeavy_Taunt_01								  ,
		BanditHeavy_Taunt_02								  ,
		BanditHeavy_Taunt_03								  ,
		BanditHeavy_Taunt_04								  ,
		BanditHeavy_Turn_L180								  ,
		BanditHeavy_Turn_L90								  ,
		BanditHeavy_Turn_R180								  ,
		BanditHeavy_Turn_R90								  ,
		BanditHeavy_Walk_B									  ,
		BanditHeavy_Walk_BL									  ,
		BanditHeavy_Walk_BR									  ,
		BanditHeavy_Walk_F									  ,
		BanditHeavy_Walk_FL									  ,
		BanditHeavy_Walk_FR									  ,
		BanditHeavy_AssassinTransformation_01				  ,
		BanditHeavy_DeathHeavy_F_01							  ,
		BanditHeavy_DeathLight_B_01							  ,
		BanditHeavy_DeathLight_F_01							  ,
		BanditHeavy_HitHeavy_B_01							  ,
		BanditHeavy_HitHeavy_F_01							  ,
		BanditHeavy_HitLight_B_01							  ,
		BanditHeavy_HitLight_F_01							  ,
		BanditHeavy_HitLight_F_02							  ,
		BanditHeavy_HitLight_F_03							  ,
		BanditHeavy_HitNormal_B180_01						  ,
		BanditHeavy_HitNormal_B_01							  ,
		BanditHeavy_HitNormal_F_01							  ,
		BanditHeavy_HitNormal_F_02							  ,
		BanditHeavy_HitNormal_L_01							  ,
		BanditHeavy_HitNormal_R_01							  ,
		BanditHeavy_KnockFrontCannonball_F_01				  ,
		BanditHeavy_KnockUp									  ,
		BanditHeavy_KnockUp_Hit								  ,
		BanditHeavy_OpenState_HitLight_FL_01				  ,
		BanditHeavy_OpenState_Loop							  ,
		BanditHeavy_OpenState_HitLight_FR_01				  ,
		BanditHeavy_WhipHold								  ,
		ManHeavy_KnockFrontCannonball_F_02					  ,
		BanditHeavy_Burn_Loop_01							  ,
		BanditHeavy_Burn_Start_01							  ,
		BanditHeavy_Burn_Stop_01							  ,
		BanditHeavy_Electrocute_Loop						  ,
		BanditHeavy_HitNormalToStun_01						  ,
		BanditHeavy_OpenState_Death							  ,
		BanditHeavy_OpenState_Start							  ,
		BanditHeavy_OpenState_Stop							  ,
		BanditHeavy_Stun_Loop								  ,
		BanditHeavy_AchillesReaction						  ,
		BanditHeavy_KnockFrontHeavy_F_01					  ,
		BanditHeavy_KnockInAirDown							  ,
		BanditHeavy_KnockUp_Down_End						  ,
		BanditHeavy_KnockUp_Down_Start						  ,
		BanditHeavy_KnockUp_Hit_FixedMovement				  ,
		BanditHeavy_StrongKick								  ,
		BanditHeavy_StrongSlamDouble						  ,
		BanditHeavy_Vampiric_Spawn_01						  ,
		BanditHeavy_VortexGrenade_Loop						  ,
		BanditHeavy_VortexGrenade_Stop						  ,
		BanditHeavyVampiric_Healing_Interrupt				  ,
		BanditHeavyVampiric_Healing_Loop					  ,
		BanditHeavyVampiric_Healing_Start					  ,
		BanditHeavyVampiric_Healing_Stop					  ,
		ManHeavy_OpenStatePull_F_01							  ,
		ManHeavy_OpenStatePull_F_02							  ,
		VampireTank_KnockFrontLight_F_01					  ,
	};
};

END




