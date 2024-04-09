#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CCamera;
class CNavigation;
END


BEGIN(Client)

class CPlayer;
class CMother;
class CSon;
class CCamera_Dynamic;
class CMasterCamera;
class CSky;
class CEnvironment_Interact;

class CData_Manager : public CBase
{
	DECLARE_SINGLETON(CData_Manager);

private:
	CData_Manager();
	virtual ~CData_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	
public: 
	void Set_Player(CPlayer* _pPlayer);
	CPlayer* Get_Player();
	void Reset_Player(LEVEL eLEVEL);

	void Set_Mother(CMother* _pMother);
	CMother* Get_Mother();

	void Set_Son(CGameObject* _pSon);
	CGameObject* Get_Son();
	void Set_Son2(CGameObject* _pSon);
	CGameObject* Get_Son2();


public:
	void Apply_Shake_And_Blur(Power ePower = Power::Light);

public:
	void Set_MasterCamera(CMasterCamera* _pMasterCamera);
	CMasterCamera* Get_MasterCamera();
	void Reset_MasterCamera(LEVEL eLEVEL);

public:
	CSky*	Get_pSkyBox() { return m_pSky; }
	void	Set_pSkybox(CSky* _pSkybox);

	_uint	Get_SkyTextureCount();
	_uint	Get_SkyCurrentTextureIndex();

	_uint   Get_CurrentSkyType();
	void	Set_SkyType(_uint iCastSkyType);

	void    Set_SkyTextureIndex(_uint iSkyTextureIndex);

	CEnvironment_Interact* Get_SnowMountainWagon() { return m_pSnowMountainWagon;}
	void				   Set_SnowMountainWagon(CEnvironment_Interact* pSnowWagon) { m_pSnowMountainWagon = pSnowWagon;}

public:
	CNavigation* Get_Navigation() { return m_pNavigation; }
	void		 Set_Navigation(CNavigation* pNavigation) { m_pNavigation = pNavigation; }

public:
	_bool Is_AdditionalSkill_Learned(Additional_Skill eAdditional_Skill) { return m_AdditionalSkills[ECast(eAdditional_Skill)]; }
	void Set_AdditionalSkill(Additional_Skill eAdditional_Skill, _bool bValue) { m_AdditionalSkills[ECast(eAdditional_Skill)] = bValue; };

	_float Get_Additional_ElectricDamage() { return m_fAdditional_ElectricDamage; }
	void Activate_Additional_ElectricDamage() { m_fAdditional_ElectricDamage = 5.f; };

	_float Get_Additional_MeleeDamage() { return m_fAdditional_MeleeDamage; }
	void Activate_Additional_MeleeDamage() { m_fAdditional_MeleeDamage = 3.f; };

	_float Get_HpRegen() { return m_fHpRegen; }
	void Activate_HpRegen() { m_fHpRegen = 150.f; };

	_float Get_Additional_MoveSpeed() { return m_fAdditional_MoveSpeed; }
	void Activate_Additional_MoveSpeed() { m_fAdditional_MoveSpeed = 1.f; };

	_bool Is_AdditionalWeapon_Acquired(Additional_Weapon eAdditional_Weapon) { return m_AdditionalWeapons[ECast(eAdditional_Weapon)]; }
	void Set_AdditionalWeapon(Additional_Weapon eAdditional_Weapon, _bool bValue) { m_AdditionalWeapons[ECast(eAdditional_Weapon)] = bValue; };

private:
	_bool   m_AdditionalSkills[ECast(Additional_Skill::Additional_Skill_End)];
	_bool   m_AdditionalWeapons[ECast(Additional_Weapon::Additional_Weapon_End)];

	_float   m_fAdditional_ElectricDamage = { 0.f };
	_float   m_fAdditional_MeleeDamage = { 0.f };

	_float  m_fHpRegen = { 100.f };
	_float   m_fAdditional_MoveSpeed = { 0.f };

// ! SH_Add
public:
	void	Set_MouseFix(_bool bFix) { m_bFix = bFix; }
	_bool	Get_MouseFix() { return m_bFix; }

	void	Set_MouseCenter(_bool bCenter) { m_bCenter = bCenter; }
	_bool	Get_MouseCenter() { return m_bCenter; }

private:
	_bool	m_bCenter = false;
	_bool	m_bFix = false;

private: /* _float */
	_float	m_fMaxHP = 100.f;
	_float	m_fCurHP = 100.f;

	_float	m_fMaxEXP = 1000000000.f;
	_float	m_fCurEXP = 0.f;

	_float	m_fMaxEnergyGuige = 100.f;
	_float	m_fCurEnergyGuige = 0.f;

	GAME_STATE m_eGame_State = GAME_STATE::GAMEPLAY;

private:
	LEVEL	m_eSelectLevel = LEVEL::LEVEL_END;

public:
	void	Set_SelectLevel(LEVEL eSelectLevel) { m_eSelectLevel = eSelectLevel; }
	LEVEL	Get_SelectLevel() { return m_eSelectLevel; }

private: /* _int */
	_int	m_iCurLevel = 1;

private: /* _bool */
	_bool	m_bLevelUp = false;				// 레벨업		On/Off
	_bool	m_bShowLevelBox = false;		// 레벨 박스		On/Off
	_bool	m_bShowInterface = false;		// 인터페이스	On/Off
	_bool	m_bShowTutorial = false;		// 튜토리얼 박스 On/Off
	_bool	m_bShowQuestBox = false;		// 퀘스트 박스	On/Off
	_bool	m_bShowRewardBox = false;		// 리워드 박스	On/Off
	_bool	m_bShowCrosshair = false;		// 크로스헤어	On/Off
	_bool	m_bTriggerCrosshair = false;	// 크로스헤어	PlayAnim
	_bool	m_bHit = false;					// 피격받음		On/Off

// ! SH_Add
public: /* ========================== Player Info ========================== */
	void	PlayerInfo_Setting();	// Player Setting
	void	Limit_Manager();		// Limit_Manager
#pragma region =========> GameState <=========
	GAME_STATE	Get_GameState() { return m_eGame_State; }
	void		Set_GameState(GAME_STATE eGame_State) { m_eGame_State = eGame_State; }

#pragma region =========> Hit <=========
	_bool	Get_HitState() { return m_bHit; }
	void	Set_HitState(_bool bHit) { m_bHit = bHit; }

#pragma region =========> MAX_HP <=========
	// => HP (Limit)
	void	Limit_HP();
	// =>최대체력 Get함수
	_float	Get_MaxHP() { return m_fMaxHP; }
	// =>최대체력 Set함수
	void	Set_MaxHP(_float fMaxHP) { m_fMaxHP = fMaxHP; }
	// =>최대체력 Add함수
	void	Add_MaxHP(_float fAddMaxHP) { m_fMaxHP += fAddMaxHP; }

#pragma region =========> CUR_HP <=========
	// =>현재체력 Get함수
	_float	Get_CurHP() { return m_fCurHP; }
	// =>현재체력 Set함수
	void	Set_CurHP(_float fCurHP) { m_fCurHP = fCurHP; }
	// =>현재체력 Add함수
	void	Add_CurHP(_float fAddCurHP) { m_fCurHP += fAddCurHP; }

#pragma region =========> MAX_EXP <=========
	// =>Player_EXP(Limit)
	_bool	Limit_EXP();
	// =>최대경험치 Get함수
	_float	Get_MaxEXP() { return m_fMaxEXP; }
	// =>최대경험치 Set함수
	void	Set_MaxEXP(_float fMaxEXP) { m_fMaxEXP = fMaxEXP; }
	// =>최대경험치 Add함수
	void	Add_MaxEXP(_float fAddMaxEXP) { m_fMaxEXP += fAddMaxEXP; }
	// =>LevelUp시 비율로 최대경험치를 증가시키는 함수
	void	NextLevl_MaxEXP() { m_fMaxEXP = m_fMaxEXP + (m_fMaxEXP / 3); }

#pragma region =========> CUR_EXP <=========
	// =>현재 경험치 Get함수
	_float	Get_CurEXP() { return m_fCurEXP; }
	// =>현재 경험치 Set함수
	void	Set_CurEXP(_float fCurEXP) { m_fCurEXP = fCurEXP; }
	// =>현재 경험치 Add함수
	void	Add_CurEXP(_float fAddCurEXP) { m_fCurEXP += fAddCurEXP; }

#pragma region =========> MAX_ENERGYGUIGE <=========
	// =>SkillGuige (Limit)
	void	Limit_EnergyGuige();
	// =>맥스 스킬게이지 Get함수
	_float	Get_MaxEnergyGuige() { return m_fMaxEnergyGuige; }
	// =>맥스 스킬게이지 Set함수
	void	Set_MaxEnergyGuige(_float fMaxEnergyGuige) { m_fMaxEnergyGuige = fMaxEnergyGuige; }
	// =>맥스 스킬게이지 Add함수
	void	Add_MaxEnergyGuige(_float fAddMaxEnergyGuige) { m_fMaxEnergyGuige += fAddMaxEnergyGuige; }

#pragma region =========> CUR_SKILLGUIGE <=========
	// =>현재 스킬게이지 Get함수
	_float	Get_CurSkillGuige() { return m_fCurEnergyGuige; }
	// =>현재 스킬게이지 Set함수
	void	Set_CurSkillGuige(_float fCurEnergyGuige) { m_fCurEnergyGuige = fCurEnergyGuige; }
	// =>현재 스킬게이지 Add함수
	void	Add_CurSkillGuige(_float fAddCurEnergyGuige) { m_fCurEnergyGuige += fAddCurEnergyGuige; }

#pragma region =========> PLAYER_LEVEL <=========
	// =>LevelUP (Limit)
	void	Limit_Level();
	// =>현재 레벨 Get함수
	_int	Get_CurLevel() { return m_iCurLevel; }
	// =>현재 레벨 Set함수
	void	Set_CurLevel(_int iCurLevel) { m_iCurLevel = m_iCurLevel; }
	// =>현재 레벨 Add함수
	void	Add_CurLevel(_int fAddCurLevel) { m_iCurLevel += fAddCurLevel; }
	// =>LevelUP_UI를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowLevelBox() { return m_bShowLevelBox; }
	// =>LevelUP_UI를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowLevelBox(_float bShowLevelBox) { m_bShowLevelBox = bShowLevelBox; }

#pragma region =========> HUD(_Interface_) <=========
	// =>PlayerHUD를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowInterface() { return m_bShowInterface; };
	// =>PlayerHUD를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowInterface(_bool bShowInterface) { m_bShowInterface = bShowInterface; }
	/* !!! ShowInterface가 true일때 UI가 켜지며, LifeTime을 계속 갱신시킴 / false일때 시간이 지나면 꺼짐 !!! */

#pragma region =========> TutorialBox <=========
	// =>TutorialBox를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowTutorialBox() { return m_bShowTutorial; };
	// =>TutorialBox를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowTutorialBox(_bool bShowTutorial) { m_bShowTutorial = bShowTutorial; }

#pragma region =========> QuestBox <=========
	// =>QuestBox를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowQuestBox() { return m_bShowQuestBox; };
	// =>QuestBox를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowQuestBox(_bool bShowQuestBox) { m_bShowQuestBox = bShowQuestBox; }

#pragma region =========> RewardBox <=========
	// =>RewardBox를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowRewardBox() { return m_bShowRewardBox; };
	// =>RewardBox를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowRewardBox(_bool bShowRewardBox) { m_bShowRewardBox = bShowRewardBox; }

#pragma region =========> Crosshair <=========
	// =>Crosshair를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowCrosshair() { return m_bShowCrosshair; }
	// =>Crosshair를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowCrosshair(_bool bCrosshair) { m_bShowCrosshair = bCrosshair; }
	// =>총을 발사했을 때 Crosshair발사 애니메이션 재생을 세팅할 수 있는 Set함수
	_bool	Get_TriggerCrosshair() { return m_bTriggerCrosshair; }
	// =>총을 발사했을 때 Crosshair발사 애니메이션 활성화 여부를 결정하는 Get함수
	void	Set_TriggerCrosshair(_bool bTriggerCrosshair) { m_bTriggerCrosshair = bTriggerCrosshair; }

private:
	CMasterCamera* m_pMasterCamera = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	CMother* m_pMother = { nullptr };
	CGameObject* m_pSon = { nullptr };
	CGameObject* m_pSon2 = { nullptr };

	CSky* m_pSky = { nullptr };
	CNavigation* m_pNavigation = { nullptr };
	CEnvironment_Interact* m_pSnowMountainWagon = { nullptr };


private:
	CCamera* m_pCamera = { nullptr };

	
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };



public:
	virtual void	Free();
};

END

