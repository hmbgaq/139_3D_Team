#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CWindow_MapTool final : public CImgui_Window
{
public:
	struct WINDOW_MAPTOOL_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_MapTool() = default;


public:
	virtual	HRESULT Initialize() override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render() override;

public:
	static CWindow_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

