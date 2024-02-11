#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)

class CImgui_Manager : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

public:
	enum class IMGUI_WINDOW_TYPE
	{
		IMGUI_MAPTOOL_WINDOW,
		IMGUI_ANIMATIONTOOL_WINDOW,
		IMGUI_EFFECTTOOL_WINDOW,
		IMGUI_UITOOL_WINDOW,
		IMGUI_WINDOW_END
	};

private:
			 CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void			 ImGuizmo_Initialize();
	void			 ImGuiFileDialog_Initialize();

	void			Tick(_float fTimeDelta);
	void			Render();


private:
	weak_ptr<ID3D11Device*>			m_pDevice;
	weak_ptr<ID3D11DeviceContext*>	m_pContext;

public:
	virtual void	Free();
};

END

