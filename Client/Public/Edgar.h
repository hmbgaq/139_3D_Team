#pragma once

#include "Character_Client.h"
#include "Actor.h"

BEGIN(Engine)
END

BEGIN(Client)


class CEdgar final : public CCharacter_Client
{
private:
	CEdgar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEdgar(const CEdgar& rhs);
	virtual ~CEdgar() = default;

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
	void Search_Target(_float fMaxDistance = 10.f);

public:
	virtual void Check_Frustum() override;

protected:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;

private:
	CActor<CEdgar>* m_pActor = { nullptr };

public:
	static CEdgar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;


public:
	enum Edgar_State
	{
		Edgar_Cargo_01														,
		Edgar_Cargo_02														,
		Edgar_Cargo_Dynamite												,
		Edgar_Cargo_DLG														,
		Edgar_cargo_OL														,
		Edgar_GivesWinchester_end											,
		Edgar_TankIntro_PreScene_pointing_loop								,
		Edgar_Redemption_DLG_004											,
		Edgar_Redemption_DLG_005											,
		Edgar_FallingAndCrawling											,
		EdgarGhost_Walk_01													,
		Edgar_Gambit_Edgar_pointing_loop_02									,
		EdgarGhost_Limp_Walk												,
		EdgarGhost_Limp_Walk_swamp2											,
		EdgarGhost_Walk														,
		Edgar_Sniper_Hip_AO_F												,
		Edgar_Sniper_IdlePose												,
		Edgar_Sniper_Hip_AO_FD												,
		Edgar_Sniper_Hip_AO_FU												,
		Edgar_Sniper_Hip_AO_L												,
		Edgar_Sniper_Hip_AO_LD												,
		Edgar_Sniper_Hip_AO_LU												,
		Edgar_Sniper_Hip_AO_R												,
		Edgar_Sniper_Hip_AO_RD												,
		Edgar_Sniper_Hip_AO_RU												,
		Edgar_Sniper_Hip_Fire												,
		Edgar_Sniper_Ironsights_AO_F										,
		Edgar_Sniper_Ironsights_AO_FD										,
		Edgar_Sniper_Ironsights_AO_FU										,
		Edgar_Sniper_Ironsights_AO_L										,
		Edgar_Sniper_Ironsights_AO_LD										,
		Edgar_Sniper_Ironsights_AO_LU										,
		Edgar_Sniper_Ironsights_AO_R										,
		Edgar_Sniper_Ironsights_AO_RD										,
		Edgar_Sniper_Ironsights_AO_RU										,
		Edgar_Sniper_Ironsights_Fire										,
		Edgar_Sniper_Ironsights_Reload										,
		Edgar_Sniper_Melee_01												,
		Edgar_Sniper_Melee_02												,
		Edgar_Sniper_Melee_03												,
		Edgar_Sniper_MeleeDynamic_01										,
		Edgar_Sniper_MeleeDynamic_02										,
		Edgar_Sniper_MeleeDynamic_03										,
		Edgar_Sniper_CoverHigh_Idle											,
		Edgar_Sniper_CoverHigh_Reload										,
		Edgar_Sniper_CoverHigh_Side_L_AimPose								,
		Edgar_Sniper_CoverHigh_Side_L_Start									,
		Edgar_Sniper_CoverHigh_Side_L_Stop									,
		Edgar_Sniper_CoverHigh_Side_R_AimPose								,
		Edgar_Sniper_CoverHigh_Side_R_Start									,
		Edgar_Sniper_CoverHigh_Side_R_Stop									,
		Edgar_Sniper_CoverLow_Idle											,
		Edgar_Sniper_CoverLow_Over_AimPose									,
		Edgar_Sniper_CoverLow_Over_Start									,
		Edgar_Sniper_CoverLow_Over_Stop										,
		Edgar_Sniper_CoverLow_Reload										,
		Edgar_Sniper_CoverLow_Side_L_AimPose								,
		Edgar_Sniper_CoverLow_Side_L_Start									,
		Edgar_Sniper_CoverLow_Side_L_Stop									,
		Edgar_Sniper_CoverLow_Side_R_AimPose								,
		Edgar_Sniper_CoverLow_Side_R_Start									,
		Edgar_Sniper_CoverLow_Side_R_Stop									,
		Edgar_Sniper_Crouch_Idle											,
		Edgar_Sniper_Crouch_Start											,
		Edgar_Sniper_Crouch_Stop											,
		Edgar_Sniper_Crouch_Turn_90_L										,
		Edgar_Sniper_Crouch_Turn_90_R										,
		Edgar_Sniper_Idle													,
		Edgar_Sniper_Run_B													,
		Edgar_Sniper_Run_BL													,
		Edgar_Sniper_Run_BR													,
		Edgar_Sniper_Run_F													,
		Edgar_Sniper_Run_FL													,
		Edgar_Sniper_Run_FR													,
		Edgar_Sniper_Sprint_F												,
		Edgar_Sniper_Taunt_01												,
		Edgar_Sniper_Taunt_02												,
		Edgar_Sniper_Taunt_03												,
		Edgar_Sniper_Turn_L180												,
		Edgar_Sniper_Turn_L90												,
		Edgar_Sniper_Turn_R180												,
		Edgar_Sniper_Turn_R90												,
		Edgar_Sniper_Walk_B													,
		Edgar_Sniper_Walk_BL												,
		Edgar_Sniper_Walk_BR												,
		Edgar_Sniper_Walk_F													,
		Edgar_Sniper_Walk_FL												,
		Edgar_Sniper_Walk_FR												,
		Edgar_Sniper_WalkAim_B												,
		Edgar_Sniper_WalkAim_BL												,
		Edgar_Sniper_WalkAim_BR												,
		Edgar_Sniper_WalkAim_F												,
		Edgar_Sniper_WalkAim_FL												,
		Edgar_Sniper_WalkAim_FR												,
		Edgar_Sniper_DeathLight_B_01										,
		Edgar_Sniper_DeathLight_F_01										,
		Edgar_Sniper_DeathNormal_B_01										,
		Edgar_Sniper_DeathNormal_F_01										,
		Edgar_Sniper_HitHeavy_B_01											,
		Edgar_Sniper_HitHeavy_BL_01											,
		Edgar_Sniper_HitHeavy_BR_01											,
		Edgar_Sniper_HitHeavy_F_01											,
		Edgar_Sniper_HitHeavy_FL_01											,
		Edgar_Sniper_HitHeavy_FR_01											,
		Edgar_Sniper_HitLight_F_01											,
		Edgar_Sniper_KnockFrontLight_B_01									,
		Edgar_Sniper_KnockFrontLight_F_01									,
		Edgar_Sniper_KnockFrontLight_F_02									,
		Edgar_Sniper_Weakspot_Death_01										,
		Edgar_Sniper_Stun_Loop												,
		Edgar_Sniper_Burn_Loop_TEMP											,
		Edgar_Sniper_CoverHigh_LeanLeft_HitHeavy_01_TEMP					,
		Edgar_Sniper_CoverHigh_LeanRight_HitHeavy_01_TEMP					,
		Edgar_Sniper_CoverLow_LeanLeft_HitHeavy_01_TEMP						,
		Edgar_Sniper_CoverLow_LeanOver_HitHeavy_01_TEMP						,
		Edgar_Sniper_CoverLow_LeanRight_HitHeavy_01_TEMP					,
		Edgar_Sniper_Electrocute_Loop_TEMP									,
		Edgar_SniperIntro													,
		Edgar_Sniper_Introduction_03										,
		Edgar_ThrowDynamite													,
		Edgar_Grenade_Throw_v2												,

	};

};

END

