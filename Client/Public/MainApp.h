#pragma once
#include "stdafx.h"

#include "Client_Defines.h"
#include "Base.h"



BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final  : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	/* IDirect3DDevice9 == LPDIRECT3DDEVICE9 */
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

//#ifdef _DEBUG
private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = { 0.f };
	_uint					m_iNumRender = { 0 };

//#endif

private:
	HRESULT Open_Level(LEVEL eStartLevelID);
	HRESULT Ready_Prototype_Component_ForStaticLevel();

private:
	HRESULT Ready_Gara();

public:
	static CMainApp* Create();
	virtual void Free();
};

END