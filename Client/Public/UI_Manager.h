#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"
#include "GameInstance.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)
class CUI;

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
	HRESULT Ready_BossHUD_Bar(_uint iLevelIndex, CGameObject* pOwner = nullptr);
	HRESULT Ready_EnemyHUD_Shard(_uint iLevelIndex, CGameObject* pOwner = nullptr);

public:
	/* LeftHUD */
	HRESULT Add_LeftHUD(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_LeftHUD();
	void	NonActive_LeftHUD();
	vector<CUI*>	m_vecLeftHUD;
	/* RightHUD */
	HRESULT Add_RightHUD(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_RightHUD();
	void	NonActive_RightHUD();
	vector<CUI*>	m_vecRightHUD;
	/* TutorialBox */
	HRESULT Add_TutorialBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_TutorialBox();
	vector<CUI*>	m_vecTutorialBox;
	/* LevelUp */
	HRESULT Add_LevelUp(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_LevelUp();
	vector<CUI*>	m_vecLevelUP;
	/* RewardBox */
	HRESULT Add_RewardBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_RewardBox();
	vector<CUI*>	m_vecRewardBox;
	/* QuestBox */
	HRESULT Add_QuestBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_QuestBox();
	vector<CUI*>	m_vecQuestBox;

	/* Distortion */
	HRESULT Add_Distortion(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Distortion();
	vector<CUI*>	m_vecDistortion;

	/* IntroLoading */
	HRESULT Add_Loading_Intro(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_Intro(_bool bActive);
	vector<CUI*>	m_vecLoading;

	/* BossLoading */
	HRESULT Add_Loading_IntroBoss(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_Loading_IntroBoss(_bool bActive);
	vector<CUI*>	m_vecLoadingIntroBoss;

	/* Crosshair */
	HRESULT			Add_Crosshair(_uint iLevelIndex, const wstring& strLayerTag);
	void			Active_Crosshair(_bool bActive);
	void			Trigger_Crosshair(_bool bPlayAnim);
	vector<CUI*>	m_vecCrosshair;

	/* BossHUD(Bar) */
	HRESULT			Add_BossHUD_Bar(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_BossHUD_Bar(_bool bActive);
	vector<CUI*>	m_vecBossHUD_Bar;

	/* BossHUD(Shard) */
	HRESULT			Add_BossHUD_Shard(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_BossHUD_Shard(_bool bActive);
	vector<CUI*>	m_vecBossHUD_Shard;

	/* EnemyHUD(Shard) */
	HRESULT			Add_EnemyHUD_Shard(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pOwner = nullptr);
	void			Active_EnemyHUD_Shard(_bool bActive);
	void			Set_EnemyHUD_World(_matrix matWorld);
	void			Set_Offset(_float fOffsetX, _float fOffsetY);

	vector<CUI*>	m_vecEnemyHUD_Shard;




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

