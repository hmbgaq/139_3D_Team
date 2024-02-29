#pragma once

#include "Imgui_Window.h"

BEGIN(Client)


class CWindow_Style final : public CImgui_Window
{
public:
	typedef struct tagStyle_Desc
	{
		ImVec4 vBgColor = ImVec4(0.f, 0.f, 0.f, 1.f);

		_float fWindowRounding	= { 0.f };
		_float fFrameRounding	= { 0.f };
		_float fPopupRounding	= { 0.f };
		_float fGrabRounding	= { 0.f };
		_float fTabRounding		= { 0.f };

	}STYLE_DESC;

	struct Window_Style_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_Style(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_Style() = default;

public:
	virtual	HRESULT Initialize()			override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render()				override;

public:
	void	Load_CustomStyle_ForMember();

private:
	STYLE_DESC	m_tStyleDesc = {};

	_float		m_vBgColor[4] = { 0.f, 0.f, 0.f, 1.f };

public:
	static CWindow_Style* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

