#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CGameObject;
END

class CUI_Interaction;
class CUI_EnemyHUD_Shard;

BEGIN(Client)
//class CUI;

class CUI_Manager : public CBase
{
	DECLARE_SINGLETON(CUI_Manager);

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void		Tick(_float fTimeDelta);

public:	// 생성 함수 작성
	template<typename T>
	T* Clone_Object(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr)
	{
		CGameObject* pClonedObject = m_pGameInstance->Add_CloneObject_And_Get(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
		T* pResult = dynamic_cast<T*>(pClonedObject);
		return pResult;
	};

	template<typename T>
	T* Clone_Object(const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr)
	{
		return Clone_Object(m_pGameInstance->Get_NextLevel(), strLayerTag, strPrototypeTag, pArg);
	};

	template<typename T>
	T* Clone_Component()
	{
	};

public: /* Ready_Preset */
	HRESULT Ready_Interface(_uint iLevelIndex);
	HRESULT Ready_Crosshair(_uint iLevelIndex);
	HRESULT Ready_Loading_Intro(_uint iLevelIndex);
	HRESULT Ready_Loading_IntroBoss(_uint iLevelIndex);
	HRESULT Ready_Loading_SnowMountain(_uint iLevelIndex);
	HRESULT Ready_Loading_SnowMountainBoss(_uint iLevelIndex);
	HRESULT Ready_Loading_ToolLevel(_uint iLevelIndex);
	HRESULT Ready_Loading_TestLevel(_uint iLevelIndex);
	HRESULT Ready_BossHUD_Bar(_uint iLevelIndex, CGameObject* pOwner = nullptr, const string& strBossName = "");
	HRESULT Ready_DiedScreen(_uint iLevelIndex);
	HRESULT Ready_Option(_uint iLevelIndex);
	HRESULT Ready_MainMenu(_uint iLevelIndex);
	HRESULT Ready_MouseCursor(_uint iLevelIndex);
	HRESULT Ready_HitUI(_uint iLevelIndex);

	/* EnemyShard */
	CUI_EnemyHUD_Shard* Ready_EnemyHUD_Shard(_uint iLevelIndex, CGameObject* pOwner = nullptr);

public:
	/* PlayerHUD */
	void			Active_PlayerHUD();
	void			NonActive_PlayerHUD();

	/* LeftHUD */
	HRESULT			Add_LeftHUD(_uint iLevelIndex, const wstring& strLayerTag);
	void			Active_LeftHUD();
	void			NonActive_LeftHUD();
	void			LeftSkillState(const string& strUIName, SKILLSTATE eState);
	void			LeftSkillUnlock(const string& strUIName, _bool bUnlock);
	vector<CUI*>	m_vecLeftHUD;

	/* RightHUD */
	HRESULT			Add_RightHUD(_uint iLevelIndex, const wstring& strLayerTag);
	void			Active_RightHUD();
	void			NonActive_RightHUD();
	void			RightSkillState(const string& strUIName, SKILLSTATE eState);
	void			RightSkillUnlock(const string& strUIName, _bool bUnlock);
	vector<CUI*>	m_vecRightHUD;

	/* LeftSkill */
	HRESULT			Add_LeftSkill(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_LeftSkill();
	void			NonActive_LeftSkill();
	CUI*			Get_LeftHUD(const string& strUIName);

#pragma region =========================== LeftCoolTime_Function ===========================
	// 현재 쿨타임 변경
	void			Change_LeftHUD_CurrentCoolTime(const string& strUIName, _float fCoolTime);
	// 현재 쿨타임 가져오기
	_float			Get_LeftHUD_CurrentCoolTime(const string& strUIName);
	// 최대 쿨타임 변경
	void			Change_LeftHUD_MaxCoolTime(const string& strUIName, _float fCoolTime);
	// 최대 쿨타임 가져오기
	_float			Get_LeftHUD_MaxCoolTime(const string& strUIName);
	// 스킬 해금
	void			Change_LeftHUD_SkillUnlock(const string& strUIName, _bool bUnlock);

	vector<CUI*>	m_vecLeftSkill;

	/* RightSkill */
	HRESULT			Add_RightSkill(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_RightSkill();
	void			NonActive_RightSkill();
	CUI*			Get_RightHUD(const string& strUIName);

#pragma region =========================== RightCoolTime_Function ===========================
	// 현재 쿨타임 변경
	void			Change_RightHUD_CurrentCoolTime(const string& strUIName, _float fCoolTime);
	// 현재 쿨타임 가져오기
	_float			Get_RightHUD_CurrentCoolTime(const string& strUIName);
	// 최대 쿨타임 변경
	void			Change_RightHUD_MaxCoolTime(const string& strUIName, _float fCoolTime);
	// 최대 쿨타임 가져오기
	_float			Get_RightHUD_MaxCoolTime(const string& strUIName);
	// 스킬 해금
	void			Change_RightHUD_SkillUnlock(const string& strUIName, _bool bUnlock);

	vector<CUI*>	m_vecRightSkill;

	/* TutorialBox */
	HRESULT Add_TutorialBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_TutorialBox();
	void	NonActive_TutorialBox();
	void	Change_TutorialText(TUTORIAL_TEXT eChangeText);

	vector<CUI*>	m_vecTutorialBox;
	/* LevelUp */
	HRESULT Add_LevelUp(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_LevelUp();
	void	NonActive_LevelUp();
	vector<CUI*>	m_vecLevelUP;
	/* RewardBox */
	HRESULT Add_RewardBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_RewardBox();
	void	NonActive_RewardBox();
	vector<CUI*>	m_vecRewardBox;

	/* QuestBox */
	HRESULT Add_QuestBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_QuestBox();
	vector<CUI*>	m_vecQuestBox;

	/* Distortion */
	HRESULT Add_Distortion(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Distortion();
	void	NonActive_Distortion();
	vector<CUI*>	m_vecDistortion;

#pragma region Loading
	/* IntroLoading */
	HRESULT Add_Loading_Intro(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_Intro(_bool bActive);
	void	NonActive_Loading_Intro();
	vector<CUI*>	m_vecLoading;

	/* BossLoading */
	HRESULT Add_Loading_IntroBoss(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_IntroBoss(_bool bActive);
	void	NonActive_Loading_IntroBoss();
	vector<CUI*>	m_vecLoadingIntroBoss;

	/* SnowMountainLoading */
	HRESULT Add_Loading_SnowMountain(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_SnowMountain(_bool bActive);
	void	NonActive_Loading_SnowMountain();
	vector<CUI*>	m_vecLoadingSnowMountain;

	/* SnowMountainBossLoading */
	HRESULT Add_Loading_SnowMountainBoss(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_SnowMountainBoss(_bool bActive);
	void	NonActive_Loading_SnowMountainBoss();
	vector<CUI*>	m_vecLoadingSnowMountainBoss;

	/* ToolLoading */
	HRESULT Add_Loading_ToolLevel(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_ToolLevel(_bool bActive);
	void	NonActive_Loading_ToolLevel();
	vector<CUI*>	m_vecLoadingTool;

	/* GamePlayLoading */
	HRESULT Add_Loading_TestLevel(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_TestLevel(_bool bActive);
	void	NonActive_Loading_TestLevel();
	vector<CUI*>	m_vecLoadingTest;
#pragma endregion

	/* Crosshair */
	HRESULT			Add_Crosshair(_uint iLevelIndex, const wstring& strLayerTag);
	void			Active_Crosshair(_bool bActive);
	void			NonActive_Crosshair();
	void			Trigger_Crosshair(_bool bPlayAnim);
	vector<CUI*>	m_vecCrosshair;

	/* BossHUD(Bar) */
	HRESULT			Add_BossHUD_Bar(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr, const string& strBossName = "");
	void			Active_BossHUD_Bar(_bool bActive);
	void			NonActive_BossHUD_Bar();
	vector<CUI*>	m_vecBossHUD_Bar;

	/* BossHUD(Shard) */
	HRESULT			Add_BossHUD_Shard(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_BossHUD_Shard(_bool bActive);
	void			NonActive_BossHUD_Shard();
	void			Dead_BossHUD_Shard(_bool bDeadOwner);
	vector<CUI*>	m_vecBossHUD_Shard;

	/* EnemyHUD(Shard) */
	CUI_EnemyHUD_Shard*		Add_EnemyHUD_Shard(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void					Active_EnemyHUD_Shard(_bool bActive);
	void					NonActive_EnemyHUD_Shard();
	void					Set_EnemyHUD_World(_matrix matWorld, _float3 vOffsetPos = { 0.f, 0.f, 0.f });
	void					Set_Offset(_float fOffsetX, _float fOffsetY);
	vector<CUI*>			m_vecEnemyHUD_Shard;

	/* DiedScreen */
	HRESULT			Add_DiedScreen(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_DiedScreen();
	void			NonActive_DiedScreen();
	vector<CUI*>	m_vecDiedScreen;

	/* Option */
	HRESULT			Add_Option(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_Option();
	void			NonActive_Option();
	vector<CUI*>	m_vecOption;

	/* MainMenu */
	HRESULT			Add_MainMenu(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_MainMenu();
	void			NonActive_MainMenu();
	// MainList
	HRESULT			Add_MainList(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_MainList();
	void			NonActive_MainList();
	// LevelList
	HRESULT			Add_LevelList(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_LevelList();
	void			NonActive_LevelList();

	// MainLogo
	HRESULT			Add_MainLogo(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_MainLogo();
	void			NonActive_MainLogo();

	/* MouseCusor */
	HRESULT			Add_MouseCursor(_uint iLevelIndex, const wstring& strLayerTag);
	void			Active_MouseCursor();
	void			NonActive_MouseCursor();
	CUI*			m_pMouseCursor = nullptr;

	/* HitUI */
	HRESULT			Add_HitUI(_uint iLevelIndex, const wstring& strLayerTag);
	void			Active_HitUI();
	void			NonActive_HitUI();
	CUI*			m_pHitUI = nullptr;

	/* Interaction */
	CUI_Interaction*	Add_Interaction(_uint iLevelIndex, const string& strUIName, const string& strProtoTag);


	void			Load_Json_BasicInfo(const json& Out_Json, CUI::UI_DESC* tUI_Info);
	void			Active_UI();
	void			NonActive_UI();

private:
	CUI*			m_pMainLogo = nullptr;
	vector<CUI*>	m_vecMainMenu;
	vector<CUI*>	m_vecMainList;
	vector<CUI*>	m_vecLevelList;


public:
	void			Check_UIPicking(_float fTimeDelta);

private:
	_bool				m_bMouseOver = false;
	_bool				m_bSelect = false;
	_bool				m_bSelectPressing = false;
	string				m_strSelectUI = "";
	string				m_strMouseOverUI = "";
	CUI*				m_pUI = nullptr;
	//CUI_Interaction*	m_pInteraction = nullptr;
	vector<CUI_Interaction*> m_vecInterraction;

public:
	void			Set_MouseOver(_bool bMouseOver) { m_bMouseOver = bMouseOver; }
	// UI 마우스 오버중
	_bool			Get_MouseOver() { return m_bMouseOver; }

	void			Set_Select(_bool bSelect) { m_bSelect = bSelect; }
	// UI 선택
	_bool			Get_Select() { return m_bSelect; }

	void			Set_SelectPressing(_bool bSelectPressing) { m_bSelectPressing = bSelectPressing; }
	// UI 선택(Pressing)중
	_bool			Get_SelectPressing() { return m_bSelectPressing; };

	// 선택한 UI 이름 반환
	string			Get_strSelectUI() { return m_strSelectUI; };
	// 마우스 오버중인 UI 이름 반환
	string			Get_strMouseOverUI() { return m_strMouseOverUI; };

	// 선택된 UI본인을 반환
	CUI* Get_CurrentUI() 
	{ 
		if (m_pUI != nullptr)
			return m_pUI;
		else
			return nullptr;
	}

	//CUI* Add_CloneUI(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);

#ifdef _DEBUG
	CUI*			Add_Tool(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	void			Active_Tool();
	vector<CUI*>	m_vecUI_Manager;
#endif // DEBUG

public:
	void Set_Active(UITYPE eType) { m_eType = eType; }

private:
	void	Check_Active(_float fTimeDelta);
	UITYPE	m_eType = UITYPE::NONE;

	HRESULT Load_Json(const string& strPath, const string& strFileName);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };



public:
	virtual void	Free();
};

END

