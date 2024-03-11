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

public: /* Interface */
	HRESULT Ready_Interface(_uint iLevelIndex);
	HRESULT Add_LeftHUD(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_LeftHUD();
	vector<CUI*>	m_vecLeftHUD;
	HRESULT Add_RightHUD(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_RightHUD();
	vector<CUI*>	m_vecRightHUD;
	HRESULT Add_TutorialBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_TutorialBox();
	vector<CUI*>	m_vecTutorialBox;
	HRESULT Add_LevelUp(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_LevelUp();
	vector<CUI*>	m_vecLevelUP;
	HRESULT Add_RewardBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_RewardBox();
	vector<CUI*>	m_vecRewardBox;
	HRESULT Add_QuestBox(_uint iLevelIndex, const wstring& strLayerTag);
	void	Active_QuestBox();
	vector<CUI*>	m_vecQuestBox;

	CUI* Add_CloneUI(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	vector<CUI*> m_vecUI_Manager;

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

