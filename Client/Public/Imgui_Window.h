#pragma once

#include "imgui_impl_win32.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"

#include "Client_Defines.h"
#include "Base.h"
BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CImgui_Window abstract : public CBase
{
public:
	struct ImGuiDESC
	{
		string				strName;
		ImGuiWindowFlags	eWindowFlags = ImGuiWindowFlags_None;
		ImVec2				vWindowSize = { 0.f, 0.f };
	};

protected:
	CImgui_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImgui_Window() = default;

public:
	_bool			Is_Enable() { return m_bEnable;}
	void			Set_Enable(_bool bEnable) { m_bEnable = bEnable; }
	HRESULT			SetUp_ImGuiDESC(string _strName, const ImVec2& _vWindowSize, ImGuiWindowFlags _eWindowFlags);
	HRESULT			Begin();
	HRESULT			End();

public:
	virtual	HRESULT Initialize();
	virtual	void	Tick(_float fTimeDelta);
	virtual void	Render() PURE;

protected:
	_bool				m_bEnable = false;
	_bool				m_bRenderTargetOnOff = true;
	_bool				m_bdialogCheck = false;

	ImGuiDESC			m_tImGuiDESC;
	ImVec4				m_BackgroundColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	_float				m_fWindowAlpha = 1.f;
	_float				m_fTimeDelta = 0.f;

	ID3D11Device*		m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END

