#pragma once
#include "stdafx.h"
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUI_Manager;
class CDevConsole;

class CMainApp final  : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);
	HRESULT Render();
	//CUDATEST
	int sum_int(int a, int b);
private:
	HRESULT Open_Level(LEVEL eStartLevelID);
	HRESULT Ready_Prototype_Component_ForStaticLevel();
	HRESULT Ready_Gara();
	HRESULT Ready_Font();

	// SH_Add
private: /* ===================== UI ===================== */
	HRESULT	Ready_UITexture();
	HRESULT	Ready_Mask_Noise();
	HRESULT UI_TargetTexture();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	CDevConsole*			m_pDevConsole = { nullptr };
	CUI_Manager*			m_pUIManager = { nullptr };

private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = { 0.f };
	_uint					m_iNumRender = { 0 };


public:
	static CMainApp* Create();
	virtual void Free();
};

END