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
	_bool bRenderCom_Active = true;

	/* Level Load - string도 바꿔야함 */
	_int		m_iCurrLevel_Index = {};
	LEVEL		m_eCurrLevel_Enum = {};
	wstring		m_wstrLayerTag = {};
	//string		m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/IntroMap.json";
	string			m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/Test1Map_MapData.json";

	/* Level Shader Struct */
	HBAO_PLUS_DESC	m_eHBAO_Desc = {};
	FOG_DESC		m_eFog_Desc = {};
	BLOOMRIM_DESC	m_eScreen_Desc = {};

	RADIAL_DESC		m_eRadial_Desc = {};
	DOF_DESC		m_eDOF_Desc = {};
	HDR_DESC		m_eHDR_Desc = {};
	ANTI_DESC		m_eAnti_Desc = {};
	HSV_DESC		m_eHSV_Desc = {};
	LEVEL_SHADER_DESC m_eTotalShader_Desc = {};

private:
	/* 최상위 셋팅 */
	void Imgui_Setting();
	void Top_Setting();

	/* 레벨 불러오기 */
	void Select_Level();
	HRESULT Load_Level(_int iLevel_Index);

	/* Collapsing */
	void Layer_Light_Control();
	void Layer_Level_Shader_Control();
	void Layer_Object_Shader_Control();

	/* Compress */
	void Compress_HBAO_Plus_Setting();
	void Compress_Fog_Setting();
	void Compress_BloomRim_Setting();

	void Compress_Radial_Setting();
	void Compress_DOF_Setting();
	void Compress_HDR_Setting();
	void Compress_FXAA_Setting();
	void Compress_HSV_Setting();

public:
	static CWindow_ShaderTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

