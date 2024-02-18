#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)

class CImgui_Window;

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
		IMGUI_BINARY_WINDOW,
		IMGUI_WINDOW_END
	};

	enum class JSY_MODEL_TYPE
	{
		MODEL_GROUND, MODEL_ENVIRONMENT, MODEL_INTERACT, MODEL_END 
	};

private:
			 CImgui_Manager();
	virtual ~CImgui_Manager() = default;


public:
	//!승용 전용
	//! E는 Environment의 약자
	map<wstring, JSY_MODEL_TYPE>*	Get_NonAnim_E_ModelTag() { return &m_mapNonAnim_E_ModelTag; }
	map<wstring, JSY_MODEL_TYPE>*	Get_Anim_E_ModelTag() { return &m_mapAnim_E_ModelTag; }

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void			Tick(_float fTimeDelta);
		void			MenuTick(_float fTimeDelta);

	void			Render();

private:
	//!Intialize
	HRESULT			 Ready_Windows();
	//

private:
	CImgui_Window*	 Find_Window(IMGUI_WINDOW_TYPE eWindowType);
	_bool			 Check_ImGui_Rect(); //! 마우스가 ImGui 창안에 있는지 체크


private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;

	//! Render가 먼저 호출되서 첫프레임에 터지는 현상을 막기위해 Tick이 한번이상 호출 된 이후에 Render를 호출하기위한 변수
	_bool					m_bFirstTick = false;

private:
	map<IMGUI_WINDOW_TYPE, CImgui_Window*>	m_mapWindows;
	_bool									m_bMainMenu = true;
	_bool									m_bEnableTool[(_int)IMGUI_WINDOW_TYPE::IMGUI_WINDOW_END];
	

private:
	map<wstring, JSY_MODEL_TYPE>			m_mapNonAnim_E_ModelTag = {};
	map<wstring, JSY_MODEL_TYPE>			m_mapAnim_E_ModelTag = {};
	
public:
	virtual void	Free();
};

END

