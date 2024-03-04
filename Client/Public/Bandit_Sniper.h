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
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

private:
	CActor<CBandit_Sniper>* m_pActor = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CBandit_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Bandit_Sniper_State
	{
		Sniper_Ironsights_AO_F									  ,
		Sniper_IdlePose											  ,
		Sniper_Ironsights_AO_FD									  ,
		Sniper_Ironsights_AO_FU									  ,
		Sniper_Ironsights_AO_L									  ,
		Sniper_Ironsights_AO_LD									  ,
		Sniper_Ironsights_AO_LU									  ,
		Sniper_Ironsights_AO_R									  ,
		Sniper_Ironsights_AO_RD									  ,
		Sniper_Ironsights_AO_RU									  ,
		Sniper_Ironsights_Fire									  ,
		Sniper_Ironsights_Reload								  ,
		Sniper_Melee_01											  ,
		Sniper_Melee_02											  ,
		Sniper_Melee_03											  ,
		Sniper_MeleeDynamic_01									  ,
		Sniper_MeleeDynamic_02									  ,
		Sniper_MeleeDynamic_03									  ,
		Sniper_CoverHigh_Idle									  ,
		Sniper_CoverHigh_Reload									  ,
		Sniper_CoverHigh_Side_L_AimPose							  ,
		Sniper_CoverHigh_Side_L_Start							  ,
		Sniper_CoverHigh_Side_L_Stop							  ,
		Sniper_CoverHigh_Side_R_AimPose							  ,
		Sniper_CoverHigh_Side_R_Start							  ,
		Sniper_CoverHigh_Side_R_Stop							  ,
		Sniper_CoverLow_Idle									  ,
		Sniper_CoverLow_Over_AimPose							  ,
		Sniper_CoverLow_Over_Start								  ,
		Sniper_CoverLow_Over_Stop								  ,
		Sniper_CoverLow_Reload									  ,
		Sniper_CoverLow_Side_L_AimPose							  ,
		Sniper_CoverLow_Side_L_Start							  ,
		Sniper_CoverLow_Side_L_Stop								  ,
		Sniper_CoverLow_Side_R_AimPose							  ,
		Sniper_CoverLow_Side_R_Start							  ,
		Sniper_CoverLow_Side_R_Stop								  ,
		Sniper_Crouch_Idle										  ,
		Sniper_Crouch_Start										  ,
		Sniper_Crouch_Stop										  ,
		Sniper_Crouch_Turn_90_L									  ,
		Sniper_Crouch_Turn_90_R									  ,
		Sniper_Turn_L180										  ,
		Sniper_Turn_L90											  ,
		Sniper_Turn_R180										  ,
		Sniper_Turn_R90											  ,
		Sniper_Walk_B											  ,
		Sniper_Walk_BL											  ,
		Sniper_Walk_BR											  ,
		Sniper_Walk_F											  ,
		Sniper_Walk_FL											  ,
		Sniper_Walk_FR											  ,
		Sniper_WalkAim_B										  ,
		Sniper_WalkAim_BL										  ,
		Sniper_WalkAim_BR										  ,
		Sniper_WalkAim_F										  ,
		Sniper_WalkAim_FL										  ,
		Sniper_WalkAim_FR										  ,
		Sniper_DeathLight_B_01									  ,
		Sniper_DeathLight_F_01									  ,
		Sniper_DeathNormal_B_01									  ,
		Sniper_DeathNormal_F_01									  ,
		Sniper_HitHeavy_B_01									  ,
		Sniper_HitHeavy_BL_01									  ,
		Sniper_HitHeavy_BR_01									  ,
		Sniper_HitHeavy_F_01									  ,
		Sniper_HitHeavy_FL_01									  ,
		Sniper_HitHeavy_FR_01									  ,
		Sniper_HitLight_F_01									  ,
		Sniper_KnockFrontLight_B_01								  ,
		Sniper_KnockFrontLight_F_01								  ,
		Sniper_KnockFrontLight_F_02								  ,
		Sniper_Weakspot_Death_01								  ,
		Sniper_Stun_Loop										  ,
		Sniper_Burn_Loop_TEMP									  ,
		Sniper_CoverHigh_LeanLeft_HitHeavy_01_TEMP				  ,
		Sniper_CoverHigh_LeanRight_HitHeavy_01_TEMP				  ,
		Sniper_CoverLow_LeanLeft_HitHeavy_01_TEMP				  ,
		Sniper_CoverLow_LeanOver_HitHeavy_01_TEMP				  ,
		Sniper_CoverLow_LeanRight_HitHeavy_01_TEMP				  ,
		Sniper_Electrocute_Loop_TEMP							  
	};
};

END







