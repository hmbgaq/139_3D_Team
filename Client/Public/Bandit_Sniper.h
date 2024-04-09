#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CBandit_Sniper final : public CMonster_Character
{
private:
	CBandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBandit_Sniper(const CBandit_Sniper& rhs);
	virtual ~CBandit_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	_bool	Get_ProtectExist() { return m_bProtectExist; }
	_float3 Get_BonePos() { return m_pWeapon->Get_Position(); }
	void	Set_ColliderSize(_float fSizeX, _float fSizeY, _float fSizeZ);

	/* For. Bullet */
	_int Get_BulletCnt() { return m_iBulletCnt; }
	void Set_BulletCnt(_int iCnt) { m_iBulletCnt = iCnt; }
	void Add_BulletCnt() { m_iBulletCnt += 1; }
	void Sniping_Target(_float4 TargetPos);

	/* Dead */
	
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Option();


private:
	_bool					m_bProtectExist	= {false};
	_bool					m_bDeadState = { false };
	_bool					m_bTaunt = false;
	_int					m_iBulletCnt = 0;
	_float					m_fTimeAcc = 0.f;

	CActor<CBandit_Sniper>* m_pActor			= { nullptr };
	CWeapon*				m_pWeapon			= { nullptr };

public:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Knock(_bool bIsCannonball) override;
	virtual void Hitted_Dead(Power ePower)	override;

	virtual void Hitted_Electrocute()	override;
	virtual void Hitted_OpenState_Pull() override;
	virtual void Hitted_Opened(Direction eDirection) override;
	virtual void Hitted_KnockUp() override;

public:
	static CBandit_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

public:
	enum Bandit_Sniper_State
	{
		/* Idle */
		Sniper_Introduction_03				, // ▶◀( 0) (Idle)
		Sniper_Idle							, // ▶◀( 1) (Idle)
		Sniper_IdlePose						, // ▶◀( 2) (Idle)

		/* 공격 */
		Sniper_CoverHigh_Idle				, // ▶◀( 3) (Cover / Sniping)
		Sniper_CoverHigh_Reload				, // ▶◀( 4) (Cover / Sniping)
		Sniper_CoverLow_Idle				, // ▶◀( 5) (Cover / Sniping) 방어막 있을때 
		Sniper_CoverLow_Over_Start			, // ▶◀( 6) (Cover / Sniping)
		Sniper_CoverLow_Over_AimPose		, // ▶◀( 7) (Cover / Sniping)
		Sniper_CoverLow_Over_Stop			, // ▶◀( 8) (Cover / Sniping)
		Sniper_CoverLow_Reload				, // ▶◀( 9) (Cover / Sniping)
		Sniper_Crouch_Idle					, // ▶◀(10) (Cover / Sniping) 방어막 없을때 
		Sniper_Crouch_Start					, // ▶◀(11) (Cover / Sniping)
		Sniper_Crouch_Stop					, // ▶◀(12) (Cover / Sniping)
		Sniper_Crouch_Turn_90_R				, // ▶◀(13) (Cover / Sniping)
		Sniper_Crouch_Turn_90_L				, // ▶◀(14) (Cover / Sniping)
		Sniper_Taunt_01						, // ▶◀(15) (Cover / Sniping)
		Sniper_Taunt_02						, // ▶◀(16) (Cover / Sniping)
		Sniper_Taunt_03						, // ▶◀(17) (Cover / Sniping)

		/* 일반 Hit*/
		Sniper_KnockFrontLight_F_01			, // ▶◀(18) (Hit) 왼쪽으로 맞아서 뒤로 넘어진뒤 다시 일어남 - 정면응시
		Sniper_KnockFrontLight_F_02			, // ▶◀(19) (Hit) 오른쪽으로 맞아서 뒤로 넘어진뒤 다시 일어남 - 정면응시 
		Bandit_KnockUp_Low_Fixed			, // ▶◀(20) (Hit) 파워공격으로 올려쳤을때 
		Bandit_HitNormal_FR_01_NEW			, // (21) (Hit)
		Bandit_HitNormal_FL_01_NEW			, // (22) (Hit) 
		Bandit_HitHeavy_F_01_NEW			, // (23) (Hit)
		Bandit_HitHeavy_FL_01_NEW			, // (24) (Hit)
		Bandit_HitHeavy_FR_01_NEW			, // (25) (Hit)
		Bandit_KnockFrontHeavy_F_01			, // (26) (Hit)
		/* 플레이어 특수기 Hit*/
		Bandit_KnockFrontCannonball_F_01	, // (27) (Hit)
		Bandit_KnockFrontCannonball_F_02	, // (28) (Hit)
		Infected_OpenStatePull_F_01			, // (29) (Hit)
		Bandit_HitLightOpened_R_01			, // (30) (Hit)
		Bandit_IdlePose						, // (31) Infected 애니메이션 넣으면 들어갈수밖에없는듯 자동으로 들어감 
		Bandit_HitLightOpened_L01			, // (32) (Hit)
		Sniper_Stun_Loop					, // ▶◀(33) (Hit)
		Sniper_Burn_Loop_TEMP				, // ▶◀(34) (Hit)
		Sniper_Electrocute_Loop_TEMP		, // ▶◀(35) (Hit)

		/* Death*/
		Sniper_DeathLight_F_01				, // ▶◀(36) (Death)
		Bandit_DeathNormal_F_03_NEW			, // ▶◀(37) (Death)
		Sniper_DeathNormal_F_01				, // ▶◀(38) (Death)
		Bandit_DeathHeavy_F_03_NEW			, // ▶◀(39) (Death)
		Sniper_Weakspot_Death_01			, // ▶◀(40) (Death)

		Sniper_Animation_End
	};
};

END







