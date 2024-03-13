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
class CCamera_Dynamic;
class CMasterCamera;
class CSky;

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

public:
	CNavigation* Get_Navigation() { return m_pNavigation; }
	void		 Set_Navigation(CNavigation* pNavigation) { m_pNavigation = pNavigation; }

// ! SH_Add
private: /* _float */
	_float	m_fMaxHP = 100.f;
	_float	m_fCurHP = 100.f;

	_float	m_fMaxEXP = 100.f;
	_float	m_fCurEXP = 0.f;

	_float	m_fMaxSkillGuige = 100.f;
	_float	m_fCurSkillGuige = 0.f;

private: /* _int */
	_int	m_iCurLevel = 1;

private: /* _bool */
	_bool	m_bLevelUp = false;
	_bool	m_bShowLevelBox = false;
	_bool	m_bShowInterface = false;
	_bool	m_bShowTutorial = false;
	_bool	m_bShowQuestBox = false;
	_bool	m_bShowRewardBox = false;


// ! SH_Add
public: /* ========================== Player Info ========================== */
	void	PlayerInfo_Setting();	// Player Setting
	void	Limit_Manager();		// Limit_Manager
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
#pragma region =========> MAX_SKILLGUIGE <=========
	// =>SkillGuige (Limit)
	void	Limit_SkillGuige();
	// =>맥스 스킬게이지 Get함수
	_float	Get_MaxSkillGuige() { return m_fMaxSkillGuige; }
	// =>맥스 스킬게이지 Set함수
	void	Set_MaxSkillGuige(_float fMaxSkillGuige) { m_fMaxSkillGuige = fMaxSkillGuige; }
	// =>맥스 스킬게이지 Add함수
	void	Add_MaxSkillGuige(_float fAddMaxSkillGuige) { m_fMaxSkillGuige += fAddMaxSkillGuige; }
#pragma region =========> CUR_SKILLGUIGE <=========
	// =>현재 스킬게이지 Get함수
	_float	Get_CurSkillGuige() { return m_fCurSkillGuige; }
	// =>현재 스킬게이지 Set함수
	void	Set_CurSkillGuige(_float fCurSkillGuige) { m_fCurSkillGuige = fCurSkillGuige; }
	// =>현재 스킬게이지 Add함수
	void	Add_CurSkillGuige(_float fAddCurSkillGuige) { m_fCurSkillGuige += fAddCurSkillGuige; }
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
#pragma region =========> HUD(Interface) <=========
	// =>PlayerHUD를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowInterface() { return m_bShowInterface; };
	// =>PlayerHUD를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowInterface(_bool bShowInterface) { m_bShowInterface = bShowInterface; };
#pragma region =========> TutorialBox <=========
	// =>TutorialBox를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowTutorialBox() { return m_bShowTutorial; };
	// =>TutorialBox를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowTutorialBox(_bool bShowTutorial) { m_bShowTutorial = bShowTutorial; };
#pragma region =========> QuestBox <=========
	// =>QuestBox를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowQuestBox() { return m_bShowQuestBox; };
	// =>QuestBox를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowQuestBox(_bool bShowQuestBox) { m_bShowQuestBox = bShowQuestBox; };
#pragma region =========> RewardBox <=========
	// =>RewardBox를 보여줄지 말지 결정하는 Get함수
	_bool	Get_ShowRewardBox() { return m_bShowRewardBox; };
	// =>RewardBox를 보여줄지 말지 세팅할 수 있는 Set함수
	void	Set_ShowRewardBox(_bool bShowRewardBox) { m_bShowRewardBox = bShowRewardBox; };

private:
	CMasterCamera* m_pMasterCamera = { nullptr };
	CPlayer* m_pPlayer = { nullptr };

	CSky* m_pSky = { nullptr };
	CNavigation* m_pNavigation = { nullptr };

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

