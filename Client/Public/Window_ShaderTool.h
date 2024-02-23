#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CWindow_ShaderTool final : public CImgui_Window
{
protected:
	CWindow_ShaderTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_ShaderTool() = default;

public:
	virtual	HRESULT Initialize() override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render() override;

private:
	/* Top Setting */
	_bool bRenderTarget_Active = true;

	/* Level Load */
	_int		m_iCurrLevel_Index = {};
	LEVEL		m_eCurrLevel_Enum = {};
	wstring		m_wstrLayerTag = {};
	string		m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/Test1_MapData.json";

	/* Level Shader Struct */
	HBAO_PLUS_DESC	m_eHBAO_Desc = {};
	FOG_DESC		m_eFog_Desc = {};

	/* Level Shader Active Control */
	_bool		m_bHBAO_Plus_Active = { false };
	_bool		m_bFog_Active = { false };
	_bool		m_bRimLight_Active = { false };

	_bool		m_bHDR_Active = { false };
	_bool		m_bFXAA_Active = { false };


private:
	/* 최상위 셋팅 */
	void Imgui_Setting();
	void Top_Setting();

	/* 레벨 불러오기 */
	void Select_Level();
	HRESULT Load_Level(_int iLevel_Index);

	/* Collapsing */
	void Level_Shader_Control();
	void Object_Shader_Control();

public:
	static CWindow_ShaderTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

