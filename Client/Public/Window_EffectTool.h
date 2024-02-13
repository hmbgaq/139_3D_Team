#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CWindow_EffectTool final : public CImgui_Window
{
public:
	struct Window_EffectTool_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_EffectTool() = default;


public:
	virtual	HRESULT Initialize()			override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render()				override;

public:
	static CWindow_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

