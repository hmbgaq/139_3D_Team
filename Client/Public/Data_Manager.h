#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CCamera;

END


BEGIN(Client)

class CPlayer;
class CCamera_Dynamic;
class CMasterCamera;

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



// ! SH_Add
private: /* Player */
	void	PlayerInfo_Setting();

	_float	m_fMaxHP = 100.f;
	_float	m_fCurHP = 100.f;

	_float	m_fMaxEXP = 100.f;
	_float	m_fCurEXP = 0.f;

	_float	m_fMaxSkillGuige = 100.f;
	_float	m_fCurSkillGuige = 0.f;

	_int	m_iCurLevel = 1;
	_bool	m_bLevelUp = false;
	_bool	m_bShowLevelBox = false;

	// ! SH_Add
public: /* Player Info Get/Set */
	void	Limit_HP();
	_float	Get_MaxHP() { return m_fMaxHP; }
	void	Set_MaxHP(_float fMaxHP) { m_fMaxHP = fMaxHP; }
	void	Add_MaxHP(_float fAddMaxHP) { m_fMaxHP += fAddMaxHP; }

	_float	Get_CurHP() { return m_fCurHP; }
	void	Set_CurHP(_float fCurHP) { m_fCurHP = fCurHP; }
	void	Add_CurHP(_float fAddCurHP) { m_fCurHP += fAddCurHP; }

	_bool	Limit_EXP();
	_float	Get_MaxEXP() { return m_fMaxEXP; }
	void	Set_MaxEXP(_float fMaxEXP) { m_fMaxEXP = fMaxEXP; }
	void	Add_MaxEXP(_float fAddMaxEXP) { m_fMaxEXP += fAddMaxEXP; }
	// LevelUpΩ√ MaxEXP ¡ı∞°
	void	NextLevl_MaxEXP()
	{
		m_fMaxEXP = m_fMaxEXP + (m_fMaxEXP / 3);
	}

	_float	Get_CurEXP() { return m_fCurEXP; }
	void	Set_CurEXP(_float fCurEXP) { m_fCurEXP = fCurEXP; }
	void	Add_CurEXP(_float fAddCurEXP) { m_fCurEXP += fAddCurEXP; }

	void	Limit_SkillGuige();
	_float	Get_MaxSkillGuige() { return m_fMaxSkillGuige; }
	void	Set_MaxSkillGuige(_float fMaxSkillGuige) { m_fMaxSkillGuige = fMaxSkillGuige; }
	void	Add_MaxSkillGuige(_float fAddMaxSkillGuige) { m_fMaxSkillGuige += fAddMaxSkillGuige; }

	_float	Get_CurSkillGuige() { return m_fCurSkillGuige; }
	void	Set_CurSkillGuige(_float fCurSkillGuige) { m_fCurSkillGuige = fCurSkillGuige; }
	void	Add_CurSkillGuige(_float fAddCurSkillGuige) { m_fCurSkillGuige += fAddCurSkillGuige; }

	void	Limit_Level();
	_int	Get_CurLevel() { return m_iCurLevel; }
	void	Set_CurLevel(_int iCurLevel) { m_iCurLevel = m_iCurLevel; }
	void	Add_CurLevel(_int fAddCurLevel) { m_iCurLevel += fAddCurLevel; }
	_bool	Get_ShowLevelBox() { return m_bShowLevelBox; }
	void	Set_ShowLevelBox(_float bShowLevelBox) { m_bShowLevelBox = bShowLevelBox; }

	void	Limit_Manager();
private:
	CMasterCamera* m_pMasterCamera = { nullptr };
	CPlayer* m_pPlayer = { nullptr };

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

