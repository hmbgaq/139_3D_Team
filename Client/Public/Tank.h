#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CTank : public CMonster_Character
{
private:
	CTank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CTank(const CTank& rhs);
	virtual ~CTank() = default;

public:
	_bool Is_ShieldBroken() { return m_fShieldBrokenTime > 0.f; };
	void Set_ShieldBroken();
	void Update_ShieldBrokenTime(_float fTimeDelta)
	{
		_float fResult = m_fShieldBrokenTime - fTimeDelta;
		m_fShieldBrokenTime = fResult > 0.f ? fResult : 0.f;
	}
	_bool Get_Shield_Enable() { return Get_Weapon(L"Weapon_Shield")->Get_Enable(); }
	_bool Get_Shield_Follow() { return Get_Weapon(L"Weapon_Shield")->Get_Follow(); }



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	virtual void Hitted_Left(Power ePower)	override;	
	virtual void Hitted_Right(Power ePower) override;	
	virtual void Hitted_Front(Power ePower) override;	
	//virtual void Hitted_Knock(Power ePower) override;
					
	//virtual void Hitted_Stun(Power ePower)	override;
	virtual void Hitted_Finish() override;
	virtual void Hitted_Dead(Power ePower)	override;	

	virtual void Set_Taunt() override;

public:
	void Create_GroundWave();


public:
	void Play_Sound_Attack_Melee();
	void Play_Sound_Attack_Shield();
	void Play_Sound_Wave();
	void Play_Sound_Bodyfall();
	void Play_Sound_Footstep();
	void Play_Sound_Hit();
	void Play_Sound_Shield_Crash();
	void Play_Sound_Shield_PickUp();

	void Play_Sound_Voice_Attack();
	void Play_Sound_Voice_Charge();
	void Play_Sound_Shield_Impact();


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

private:
	CActor<CTank>* m_pActor = { nullptr };
	//_bool m_bIsShieldBroken = { true };
	_float m_fShieldBrokenTime = { 0.f };
	CEffect* m_pEffect = { nullptr };
public:
	_bool m_bRushStart = false;
	_bool m_bRushStop = false;
public:
	static CTank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Tank_State
	{
		VampireTank_KnockFrontLight_F_01					  ,
		Tank_Charge_Fail_L									  ,
		Tank_Charge_Fail_R									  ,
		Tank_Charge_Hit_L									  ,
		Tank_Charge_Hit_R									  ,
		Tank_Charge_Loop									  ,
		Tank_Charge_Start									  ,
		Tank_Charge_Stop_L									  ,
		Tank_Charge_Stop_R									  ,
		Tank_GroundSlam_AoE_Stomp_01						  ,
		Tank_GroundWave_01									  ,
		Tank_GroundWaveNoShield_01							  ,
		Tank_MeleeDynamicNoShield_01						  ,
		Tank_MeleeDynamicNoShield_02						  ,
		Tank_MeleeDynamicShield_04							  ,
		Tank_MeleeDynamicShield_04_FollowUp_01				  ,
		Tank_MeleeDynamicShield_05							  ,
		Tank_MeleeDynamicShield_05_FollowUp_01				  ,
		Tank_MeleeShield_02									  ,
		Tank_MeleeShield_02_FollowUp_01						  ,
		Tank_Idle											  ,
		Tank_IdleAct_02										  ,
		Tank_IdlePoseLHAdditive								  ,
		Tank_Run_B											  ,
		Tank_Run_BL											  ,
		Tank_Run_BR											  ,
		Tank_Run_F											  ,
		Tank_Run_FL											  ,
		Tank_Run_FR											  ,
		Tank_Sprint_F										  ,
		Tank_Taunt_01										  ,
		Tank_Taunt_02										  ,
		Tank_Turn_L180										  ,
		Tank_Turn_L90										  ,
		Tank_Turn_R180										  ,
		Tank_Turn_R90										  ,
		Tank_Walk_B											  ,
		Tank_Walk_BL										  ,
		Tank_Walk_BR										  ,
		Tank_Walk_F											  ,
		Tank_Walk_FL										  ,
		Tank_Walk_FR										  ,
		Tank_DeathLight_B_01								  ,
		Tank_DeathNormal_F_01								  ,
		Tank_HitHeavy_B_01									  ,
		Tank_HitHeavyShield_F_01							  ,
		Tank_HitHeavyShield_FL90_01							  ,
		Tank_HitHeavyShield_FL_01							  ,
		Tank_HitHeavyShield_FR90_01							  ,
		Tank_HitHeavyShield_FR_01							  ,
		Tank_HitLight_B_01									  ,
		Tank_HitLight_BL_01									  ,
		Tank_HitLight_BR_01									  ,
		Tank_HitLightShield_F_01							  ,
		Tank_HitLightShield_F_02							  ,
		Tank_HitLightShield_FL_01							  ,
		Tank_HitLightShield_FR_01							  ,
		Tank_HitNormal_B_01									  ,
		Tank_HitNormal_BR_01								  ,
		Tank_HitNormalShield_F_01							  ,
		Tank_HitNormalShield_FL90_01						  ,
		Tank_HitNormalShield_FL_01							  ,
		Tank_HitNormalShield_FR90_01						  ,
		Tank_HitNormalShield_FR_01							  ,
		Tank_OpenState_HitLight_F_01						  ,
		Tank_OpenState_Loop									  ,
		Tank_OpenState_HitLight_FL_01						  ,
		Tank_OpenState_HitLight_FR_01						  ,
		Tank_WhipHold										  ,
		Tank_Burn_Loop										  ,
		Tank_OpenState_Start								  ,
		Tank_ShieldRegain									  ,
		Tank_Stun_Get_Up									  ,
		Tank_Stun_Loop										  ,
		Tank_Stun_Start										  ,
		Tank_ImpaleAttack									  ,
		Tank_ImpaleAttack_Failure							  ,
		Tank_Spawn_01										  ,
		Tank_Spawn_03										  ,
		Tank_Spawn_04_Charge								  ,
		Tank_VortexGrenade_Loop								  ,
		Tank_VortexGrenade_Stop								  ,
		Finisher_Tank										  ,

	};
};

END






