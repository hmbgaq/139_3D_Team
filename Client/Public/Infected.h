#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CInfected : public CMonster_Character
{
protected:
	CInfected(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CInfected(const CInfected& rhs);
	virtual ~CInfected() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

protected:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Knock(_bool bIsCannonball) override;
	virtual void Hitted_Dead(Power ePower)	override;


private:
	CActor<CInfected>* m_pActor = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CInfected* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Infected_State
	{
		Infected_Melee_RD_01							   ,
		Infected_Melee_RM_01							   ,
		Infected_Melee_RM_02							   ,
		Infected_Melee_RU_01							   ,
		Infected_Melee_RU_02							   ,
		Infected_MeleeDynamic_RU_01						   ,
		Infected_MeleeDynamic_RU_02						   ,
		Infected_MeleeDynamic_RU_03						   ,
		Infected_Sprint_F_Melee_02						   ,
		Infected_Sprint_F_Melee_03						   ,
		Infected_Idle									   ,
		Infected_IdleAct_01								   ,
		Infected_IdleAct_02								   ,
		Infected_IdleAct_03								   ,
		Infected_IdlePose								   ,
		Infected_InteractionVault200					   ,
		Infected_JumpDown_200_forward					   ,
		Infected_RelaxedIdleAct_01						   ,
		Infected_RelaxedIdleAct_02						   ,
		Infected_RelaxedIdleAct_03						   ,
		Infected_Run_B									   ,
		Infected_Run_F									   ,
		Infected_Run_FL									   ,
		Infected_Run_FL45								   ,
		Infected_Run_FR									   ,
		Infected_Run_FR45								   ,
		Infected_SpawnClimb_01							   ,
		Infected_SpawnClimb_02							   ,
		Infected_SpawnCrawl_01							   ,
		Infected_SpawnCrawl_02							   ,
		Infected_SpawnFromCoffin0						   ,
		Infected_SpawnFromCoffin65						   ,
		Infected_SpawnFromCoffin90						   ,
		Infected_SpawnFromEgg_01						   ,
		Infected_SpawnFromEgg_02						   ,
		Infected_SpawnJumpDown_01						   ,
		Infected_SpawnJumpOut_01						   ,
		Infected_SpawnJumpOut_03						   ,
		Infected_SpecialIdle							   ,
		Infected_SpecialIdle_to_Idle					   ,
		Infected_Sprint_F_01							   ,
		Infected_Taunt_01								   ,
		Infected_Taunt_02								   ,
		Infected_Turn_L180								   ,
		Infected_Turn_L90								   ,
		Infected_Turn_R180								   ,
		Infected_Turn_R90								   ,
		Infected_Walk_B									   ,
		Infected_Walk_F									   ,
		Infected_Walk_FL								   ,
		Infected_Walk_FL45								   ,
		Infected_Walk_FR								   ,
		Infected_Walk_FR45								   ,
		Infected_DeathHeavy_F_01_NEW					   ,
		Infected_DeathLight_B_01_NEW					   ,
		Infected_DeathLight_F_01_NEW					   ,
		Infected_HitHeavy_B_01_NEW						   ,
		Infected_HitHeavy_BL_01_NEW						   ,
		Infected_HitHeavy_BR_01_NEW						   ,
		Infected_HitHeavy_F_01_NEW						   ,
		Infected_HitHeavy_F_02_NEW						   ,
		Infected_HitHeavy_FL90_01_NEW					   ,
		Infected_HitHeavy_FL_01_NEW						   ,
		Infected_HitHeavy_FL_02_NEW						   ,
		Infected_HitHeavy_FR90_01_NEW					   ,
		Infected_HitHeavy_FR_01_NEW						   ,
		Infected_HitHeavy_FR_02_NEW						   ,
		Infected_HitLight_B_01_NEW						   ,
		Infected_HitLight_BL_01_NEW						   ,
		Infected_HitLight_BR_01_NEW						   ,
		Infected_HitLight_F_01_NEW						   ,
		Infected_HitLight_FL_01_NEW						   ,
		Infected_HitLight_FR_01_NEW						   ,
		Infected_HitLight_L_01_NEW						   ,
		Infected_HitLight_R_01_NEW						   ,
		Infected_HitLightOpened_F_01_NEW				   ,
		Infected_HitNormal_B_01_NEW						   ,
		Infected_HitNormal_BL_01_NEW					   ,
		Infected_HitNormal_BR_01_NEW					   ,
		Infected_HitNormal_F_01_NEW						   ,
		Infected_HitNormal_F_02_NEW						   ,
		Infected_HitNormal_FL90_01_NEW					   ,
		Infected_HitNormal_FL_01_NEW					   ,
		Infected_HitNormal_FR90_01_NEW					   ,
		Infected_HitNormal_FR_01_NEW					   ,
		Infected_KnockFrontLight_B_01_NEW				   ,
		Infected_KnockFrontLight_F_01_NEW				   ,
		Infected_Scared_01								   ,
		Infected_Scared_02								   ,
		Infected_Scared_03								   ,
		Infected_Scared_04								   ,
		Infected_Burn_Loop								   ,
		Infected_Electrocute_Loop						   ,
		Infected_HitHeavyToStun_01_NEW					   ,
		Infected_HitNormalToStun_01_NEW					   ,
		Infected_Stun_Loop_NEW							   ,
		Boohag_Electrocut_Additive_OpenState			   ,
		BooHag_OpenStateHitNormal_F						   ,
		Infected_Dodge_F_01_TEMP						   ,
		Infected_Dodge_L_01_TEMP						   ,
		Infected_Dodge_R_01_TEMP						   ,
		Infected_Hit_whip_end_down						   ,
		Infected_HitLightOpened_L_01					   ,
		Infected_HitLightOpened_R_01					   ,
		Infected_HitNormal_F_03_NEW						   ,
		Infected_KnockFrontCannonball_F_01_TEMP			   ,
		Infected_MeleeDynamic_Dodge_B_4m				   ,
		Infected_OpenStatePull_F_01						   ,
		Infected_OpenStateTimeout						   ,
		Infected_SpawnGround							   ,
		Infected_VortexGrenade_Loop						   ,
		Infected_VortexGrenade_Stop						   
	};
};

END




