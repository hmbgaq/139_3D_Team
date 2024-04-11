#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CVampireCommander final : public CMonster_Character
{
private:
	CVampireCommander(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CVampireCommander(const CVampireCommander& rhs);
	virtual ~CVampireCommander() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Ratation_Target_Test();

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

protected:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Dead(Power ePower)	override;
	virtual void Hitted_Stun(Power ePower) override;
	virtual void Hitted_Finish() override;
	virtual void Hitted_Weakness() override;
	

public:
	CActor<CVampireCommander>* Get_Actor() {return m_pActor;}
	void Set_Actor(CActor<CVampireCommander>* _pActor) { m_pActor = _pActor; }
private:
	CActor<CVampireCommander>* m_pActor = { nullptr };

public:
	CEffect* m_pMapEffect = { nullptr }; // TEST
	CEffect* m_pAuraEffect = { nullptr }; // TEST
public://뼈 13개 
	vector<string> m_vRandomBones = {"LeftHandIK","RightHandIK" ,"Hips","Spine1","Head","Neck","RightShoulder","LeftElbowRoll","LeftShoulderRoll_01","Right_BigWing_03","Left_BigWing_03","LeftLeg","RightLeg"};
public:
	vector<class CUI_Weakness*> m_pWeakneesUIs;
	_bool m_bTurn = true;
	_bool m_bCntDead_Active = { false };
	_bool m_bAuraDead = false;
	_float	m_fCntDead_Time = 3.f;
	_float fTimeAcc = 0.f;
public:
	/* 원형객체를 생성한다. */
	static CVampireCommander* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	enum VampireCommander_State
	{
		VampireCommander_AttackMelee_01                                  ,
		VampireCommander_AttackRanged_01								 ,
		VampireCommander_AttackRanged_02								 ,
		VampireCommander_Leap_01_Loop									 ,
		VampireCommander_Leap_01_Start									 ,
		VampireCommander_Leap_01_Stop									 ,
		VampireCommander_Leap_01_Stop_Melee								 ,
		VampireCommander_Stun_Stop_Melee								 ,
		VampireCommander_Stun_Stop										 ,
		VampireCommander_Idle											 ,
		VampireCommander_Taunt_01										 ,
		VampireCommander_Taunt_02										 ,
		VampireCommander_Taunt_03										 ,
		VampireCommander_Taunt_04										 ,
		VampireCommander_Taunt_05										 ,
		VampireCommander_Turn_L180										 ,
		VampireCommander_Turn_L90										 ,
		VampireCommander_Turn_R180										 ,
		VampireCommander_Turn_R90										 ,
		VampireCommander_Walk_B										 ,
		VampireCommander_Walk_BL										 ,
		VampireCommander_Walk_BL135									 ,
		VampireCommander_Walk_BR										 ,
		VampireCommander_Walk_BR135									 ,
		VampireCommander_Walk_F										 ,
		VampireCommander_Walk_FL										 ,
		VampireCommander_Walk_FL45										 ,
		VampireCommander_Walk_FR										 ,
		VampireCommander_Walk_FR45										 ,
		VampireCommander_AttackRanged_03_d_HitLight_F_01				 ,
		VampireCommander_HitLight_F_01									 ,
		VampireCommander_HitLight_F_02									 ,
		VampireCommander_HitLight_F_03									 ,
		VampireCommander_HitNormal_F_01								 ,
		VampireCommander_HitNormal_F_02								 ,
		VampireCommander_HitNormal_F_03								 ,
		VampireCommander_InTheAir_Hit_OpenState						 ,
		VampireCommander_Stun_Hit_FL									 ,
		VampireCommander_Stun_Loop										 ,
		VampireCommander_Stun_Hit_FR									 ,
		VampireCommander_BloodRange_02_Loop							 ,
		VampireCommander_BloodRange_02_Start							 ,
		VampireCommander_BloodRange_02_Stop							 ,
		VampireCommander_BloodRange_02_Stun_Start						 ,
		VampireCommander_Electrocute_Loop								 ,
		VampireCommander_InTheAir_BloodRange_02_Start					 ,
		VampireCommander_InTheAir_Loop									 ,
		VampireCommander_Stun_Start									 ,
		VampireCommander_SyncedAttack									 ,
		VampireCommander_SyncedAttack_Failure							 ,
		Finisher_VampireCommander_VS									 ,
		sus_left_45													 ,
		sus_left_90													 ,
		sus_right_45													 ,
		sus_right_90													 ,
		VampireCommander_AttackMelee_02								 ,
		VampireCommander_AttackRanged_03_d								 ,
		VampireCommander_Electrocute_Stop_02							 ,
		VampireCommander_Spawn_01										 ,
		VampireCommander_Spawn_02										 ,
		VampireCommander_Spawn_03										 ,
		VampireCommander_Spawn_03_Loop									 ,
		VampireCommander_StunDead_F_01									 ,
		VampireCommander_VortexGrenade_Loop							 ,
		VampireCommander_Walk_F_Levitate								 ,
		jump_in_start													 ,
		jump_loop														 ,
		jump_out														 ,
		N_Additive_LandCompression										 ,
		N_Idle_Pose
	};
};

END