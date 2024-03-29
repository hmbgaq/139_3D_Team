#include "Light.h"
#include "SMath.h"
#include "stdafx.h"
#include "Data_Manager.h"
#include "GameInstance.h"
#include "Light_Manager.h"
#include "Monster_Character.h"
#include "Window_ShaderTool.h"
#include "Environment_Object.h"
#include "ShaderParsed_Object.h"
#include "Environment_Instance.h"

CWindow_ShaderTool::CWindow_ShaderTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}

HRESULT CWindow_ShaderTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	FAILED_CHECK(__super::Initialize());

	Imgui_Setting();

	m_pGameInstance->Get_ModelTag(&m_vObjectModelTag);
	
	return S_OK;
}

void CWindow_ShaderTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Imgui_Setting();

	__super::Begin();

	Main_Window = Check_ImGui_Rect();

	/* ============== Main Control ============== */
	Top_Setting(); /* 렌더타겟, 디버그컴 컨트롤  */

	Choice_Level_N_Object(); /* 어느거 조정할지 선택 */
	
	if (ImGui::CollapsingHeader("Level Light Control"))
		Layer_Light_Control();

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Level Shader"))
		Layer_Level_Shader_Control();

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Object Shader"))
		Layer_Object_Shader_Control();

	__super::End();
}

void CWindow_ShaderTool::Render()
{
}


void CWindow_ShaderTool::Choice_Level_N_Object()
{
	ImGui::SeparatorText("Object Setting ");

	/* ======================================== */
	ImGui::PushID(1);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
	if (ImGui::Button("Create Object"))
	{
		m_bObject_Layer_Button = true;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();
	ImGui::SameLine();

	/* ======================================== */
	ImGui::PushID(2);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.f, 1.0f, 0.9f));
	if (ImGui::Button("Create Level"))
	{
		m_bCreate_Level_Button = true;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	if(m_bObject_Layer_Button)
		Create_Object();

	if (m_bCreate_Level_Button)
		Select_Level();
}

#pragma region Create Object

void CWindow_ShaderTool::Create_Object()
{
	ImGui::SetNextWindowSize(ImVec2(300, 500));

	ImGuiWindowFlags Flag = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |	ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin(u8"Shader_Object_Create", &m_bObjectCreateWindwo, Flag); /* imgui 시작 */

	Show_N_Create_ObjectList();

	ImGui::SeparatorText(" Modify Object ");

	ImGui::End();
}

void CWindow_ShaderTool::Show_N_Create_ObjectList()
{
	ImGui::SeparatorText(" Select Object ");

	/* 오브젝트 리스트 보이기 */
	_int iModelCnt = (_int)m_vObjectModelTag.size();
	static _int item_current_idx = 0;
	if (ImGui::BeginListBox(" "))
	{
		for (_int i = 0; i < iModelCnt; i++)
		{
			string displayString = m_vObjectModelTag[i]; // 원본 문자열을 복사
			// "Prototype_Component_" 부분을 삭제
			size_t found = displayString.find("Prototype_Component_Model_");
			if (found != string::npos)
			{
				displayString.erase(found, std::string("Prototype_Component_Model_").length());
			}

			const _bool is_selected = (item_current_idx == i);
			if (ImGui::Selectable(displayString.c_str(), is_selected))
			{
				item_current_idx = i;
				m_strCurrentObjectTag = m_vObjectModelTag[i];
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	/* 리스트에서 선택한 오브젝트 만들기 */
	if (ImGui::Button("Show"))
	{
		m_bCreateObject_Button = true;
	}

	if (m_bCreateObject_Button)
	{
		Create_DummyObject(m_strCurrentObjectTag);
	}
	
}

void CWindow_ShaderTool::Create_DummyObject(string ObjectTag)
{
	// m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Bandit_Sniper"));
	// 더미라 아무대나 넣음 
	wstring Temp;
	m_pGameInstance->String_To_WString(ObjectTag, Temp);

	CShaderParsed_Object::CREATE_DESC desc = {};
	desc.strModelProtoTag = ObjectTag;
	//desc.strShaderProtoTag = Prototype_Component_Shader_Monster
	m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, LAYER_MONSTER, TEXT("Prototype_GameObject_ShaderParsed_Object"), &desc);
	
}

#pragma endregion

#pragma region [LAYER] : Level Light Control

void CWindow_ShaderTool::Layer_Light_Control()
{
	/* 모든 빛 다 가져오기 */
	//m_pGameInstance->Get_AllLight(&m_listLight);
	//
	//Show_N_Controll_Light();

	if (ImGui::TreeNode(" Save / Losad "))
	{
		Save_Load_Light();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Directional Light "))
	{
		Compress_Directional_Light();

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Spot Light"))
	{
		Compress_SpotLight();

		ImGui::TreePop();
	}
}

void CWindow_ShaderTool::Save_Load_Light()
{
	const char* items2[] = { "GamePlay", "Intro", "IntroBoss", "SnowMountain", "Lava" };
	static int item_current_idx2 = 0; // Here we store our selection data as an index.
	const char* combo_preview_value2 = items2[item_current_idx2];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("1", combo_preview_value2))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items2); n++)
		{
			const bool is_selected = (item_current_idx2 == n);
			if (ImGui::Selectable(items2[n], is_selected))
			{
				item_current_idx2 = n;
				strPath = "../Bin/DataFiles/Data_Shader/Level/";
				switch (n)
				{
				case 0: /* LEVEL_GAMEPLAY */
					strFileName = "GamePlay_DirLight";
					break;
				case 1: /* LEVEL_INTRO */
					strFileName = "Intro_DirLight";
					break;
				case 2: /* LEVEL_INTRO_BOSS */
					strFileName = "IntroBoss_DirLight";
					break;
				case 3: /* LEVEL_SNOWMOUNTAIN */
					strFileName = "SnowMountain_DirLight";
					break;
				case 4: /* LEVEL_LAVA */
					strFileName = "Lava_DirLight";
					break;
				}
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Save Light"))
		m_bSave = true;

	if (ImGui::Button("Load Light"))
		m_bLoad = true;

	if (m_bSave)
		Save_Function(strPath, strFileName);

	if(m_bLoad)
		Load_Function(strPath, strFileName);
}

void CWindow_ShaderTool::Show_N_Controll_Light()
{
	const char* items[128];

	for (auto& Lights : m_listLight)
	{
		_int iIndex = Lights->Get_LightIndex();
		switch (Lights->Get_LightDesc().eType)
		{
		case LIGHT_DESC::TYPE::TYPE_DIRECTIONAL:
			break;
		case LIGHT_DESC::TYPE::TYPE_POINT:
			break;
		case LIGHT_DESC::TYPE::TYPE_SPOTLIGHT:
			break;
		}
	}

	//const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
	static int item_current_idx = 0; // Here we store our selection data as an index.
	if (ImGui::BeginListBox("listbox 1"))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
}

void CWindow_ShaderTool::Compress_Directional_Light()
{
	/* 현재 레벨의 전역빛 가져오기 */
	LIGHT_DESC NewDesc = {};
	NewDesc.bEnable = true;
	NewDesc.iLightIndex = 0;

	static int LightType = 0;
	ImGui::RadioButton("Directional", &LightType, 1); ImGui::SameLine();
	ImGui::RadioButton("PointLight", &LightType, 2); ImGui::SameLine();
	ImGui::RadioButton("SpotLight", &LightType, 3);

	if (1 <= LightType) /* Common */
	{
		static float Diffuse[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
		ImGui::ColorEdit4("Diffuse", Diffuse);

		static float Ambient[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
		ImGui::ColorEdit4("Ambient", Ambient);

		static float Specular[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
		ImGui::ColorEdit4("Specular", Specular);

		NewDesc.vDiffuse = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] };
		NewDesc.vAmbient = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] };
		NewDesc.vSpecular = { Specular[0],Specular[1],Specular[2],Specular[3] };
	}

	if (1 == LightType) /* Directional */
	{
		NewDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;

		static float Direction[4] = { 0.10f, 0.20f, 0.30f, 0.0f };
		ImGui::SliderFloat4("Direction", Direction, -1.0f, 1.0f);
		NewDesc.vDirection = { Direction[0],Direction[1],Direction[2],Direction[3] };
	}
	else if (2 == LightType) /* PointLight*/
	{

		NewDesc.eType = LIGHT_DESC::TYPE_POINT;
	}
	else if (3 == LightType) /* SpotLight*/
	{

		NewDesc.eType = LIGHT_DESC::TYPE_SPOTLIGHT;
	}

	if (ImGui::Button(u8"적용하기") || (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_B)))
	{
		_int TempIndex = {};
		m_pGameInstance->Remove_Light(0);
		m_pGameInstance->Add_Light(NewDesc, TempIndex);
	}
}

void CWindow_ShaderTool::Compress_SpotLight()
{
	if (ImGui::Button(u8"예시 만들기"))
	{
		_uint Temp = {};

		LIGHT_DESC LightDesc = {};
		LightDesc.bEnable = true;
		LightDesc.eType = LIGHT_DESC::TYPE::TYPE_SPOTLIGHT;
		LightDesc.vPosition = _float4(60.5f, 0.f, 26.f, 0.f);
		LightDesc.fRange = 10.f;
		LightDesc.fCutOff = 0.5f;
		LightDesc.fOuterCutOff = 0.7f;
		LightDesc.vDiffuse = { 1.f, 0.f, 0.f, 1.f };
		LightDesc.vAmbient = { 1.f, 0.f, 0.f, 1.f };
		LightDesc.vSpecular = { 1.f, 0.f, 0.f, 1.f };
		LightDesc.fVolumetricStrength = 10.f;

		CLight* pLight = m_pGameInstance->Add_Light_AndGet(LightDesc, Temp);
	}
}

void CWindow_ShaderTool::Compress_PointLight()
{
}


#pragma endregion

#pragma region [LAYER] : Level Shader Control

void CWindow_ShaderTool::Layer_Level_Shader_Control()
{
	ImGui::SeparatorText("Pre-Post");

	if (ImGui::TreeNode("PBR"))
	{
		Compress_PBR_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Bloom / Rim Setting"))
	{
		Compress_BloomRim_Setting();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("HBAO+ Setting"))
	{
		Compress_HBAO_Plus_Setting();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Fog Setting"))
	{
		Compress_Fog_Setting();
		ImGui::TreePop();
	}

	ImGui::SeparatorText("Post");

	if (ImGui::TreeNode("SSR Setting"))
	{
		Compress_SSR_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Radial Blur Setting"))
	{
		Compress_Radial_Setting();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("DOF Setting"))
	{
		Compress_DOF_Setting();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("HDR Setting"))
	{
		Compress_HDR_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("FXAA Setting"))
	{
		Compress_FXAA_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("HSV Setting"))
	{
		Compress_HSV_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Vignette Setting"))
	{
		Compress_Vignette_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Chroma Setting"))
	{
		Compress_Chroma_Setting();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Screen Effect"))
	{
		Compress_ScreenEffect_Setting();
		ImGui::TreePop();
	}
	ImGui::PushID(3);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Shader Save"))
	{
		m_bShaderSave = true;
	}
	ImGui::PopStyleColor(3); //PushStyleColor를 3번 호출해서 3번 지우는거
	ImGui::PopID();

	ImGui::SameLine();

	ImGui::PushID(4);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Shader Load"))
	{
		m_eDialogType = DIALOG_TYPE::LOAD_DIALOG;
		m_strDialogPath = "../Bin/DataFiles/Data_Shader/";
		OpenDialog(CImgui_Window::IMGUI_SHADER_WINDOW);
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	if (m_bShaderSave)
		Save_Shader();

	//if (m_bShaderLoad)
	//{
	//	Load_Shader();
	//}
}

void CWindow_ShaderTool::Compress_PBR_Setting()
{
	ImGui::Checkbox("PBR Active", &m_ePBR_Desc.bPBR_ACTIVE);

	ImGui::InputInt("PBR Texture", &m_iPBRTextureNumber);

	if (m_iPBRTextureNumber < 0)
		m_iPBRTextureNumber = 0;
	if (m_iPBRTextureNumber >= 10)
		m_iPBRTextureNumber = 10;

	m_pGameInstance->Set_ToolPBRTexture_InsteadLevel(m_iPBRTextureNumber);
	m_pGameInstance->Get_Renderer()->Set_PBR_Option(m_ePBR_Desc);

}

void CWindow_ShaderTool::Compress_HBAO_Plus_Setting()
{
	ImGui::Checkbox("HBAO+ Active", &m_eHBAO_Desc.bHBAO_Active);

	ImGui::SliderFloat("Radius", &m_eHBAO_Desc.fRadius, 0.1f, 5.0f, "Radius = %.3f");

	ImGui::SliderFloat("Bias", &m_eHBAO_Desc.fBias, 0.1f, 1.0f, "Bias = %.3f");

	ImGui::SliderFloat("Power Exponent", &m_eHBAO_Desc.fPowerExponent, 0.1f, 3.0f, "Power = %.3f");

	ImGui::SliderFloat("Blur Sharpness", &m_eHBAO_Desc.fBlur_Sharpness, 1.f, 20.0f, "Sharpness = %.3f");

	m_pGameInstance->Get_Renderer()->Set_HBAO_Active(m_eHBAO_Desc.bHBAO_Active);

	m_pGameInstance->Get_Renderer()->Set_HBAO_Option(m_eHBAO_Desc);
}

void CWindow_ShaderTool::Compress_Fog_Setting()
{
	ImGui::Checkbox("Fog Active", &m_eFog_Desc.bFog_Active);

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		m_eFog_Desc.fFogStartDepth = 55.f;
		m_eFog_Desc.fFogStartDistance = 0.1f;
		m_eFog_Desc.fFogDistanceValue = 30.f;
		m_eFog_Desc.fFogHeightValue = 50.f;
		m_eFog_Desc.fFogDistanceDensity = 0.05f;
		m_eFog_Desc.fFogHeightDensity = 0.05f;
		m_eFog_Desc.vFogColor = { 0.5f, 0.5f, 0.5f, 0.2f };
	}

	ImGui::SliderFloat("FogStartDepth", &m_eFog_Desc.fFogStartDepth, 0.001f, 250.0f, "StartDepth = %.3f");

	ImGui::SliderFloat("FogStartDistance", &m_eFog_Desc.fFogStartDistance, 0.001f, 30.0f, "StartDistance = %.3f");

	ImGui::SliderFloat("FogDistanceValue", &m_eFog_Desc.fFogDistanceValue, 0.001f, 50.f, "FogDistanceValue = %.3f");

	ImGui::SliderFloat("FogHeightValue", &m_eFog_Desc.fFogHeightValue, 0.001f, 100.f, "HeightValue = %.3f");

	ImGui::SliderFloat("FogDistanceDensity", &m_eFog_Desc.fFogDistanceDensity, 0.001f, 1.0f, "FogDistanceValue = %.3f");

	ImGui::SliderFloat("FogHeightDensity", &m_eFog_Desc.fFogHeightDensity, 0.001f, 1.0f, "HeightDensity = %.3f");

	static float vFogColor[4] = { 1.f, 0.0f, 0.0f, 1.f };
	ImGui::ColorEdit4("Fog Color", vFogColor);
	m_eFog_Desc.vFogColor = { vFogColor[0], vFogColor[1], vFogColor[2], vFogColor[3] };

	m_pGameInstance->Get_Renderer()->Set_Fog_Active(m_eFog_Desc.bFog_Active);

	m_pGameInstance->Get_Renderer()->Set_Fog_Option(m_eFog_Desc);
}

void CWindow_ShaderTool::Compress_BloomRim_Setting()
{
	ImGui::Checkbox("BloomRim Blur", &m_eDeferred_Desc.bRimBloom_Blur_Active);

	ImGui::Checkbox("Shadow", &m_eDeferred_Desc.bShadow_Active);

	m_pGameInstance->Get_Renderer()->Set_Deferred_Option(m_eDeferred_Desc);
}

void CWindow_ShaderTool::Compress_Radial_Setting()
{
	ImGui::Checkbox("Radial Active", &m_eRadial_Desc.bRadial_Active);

	ImGui::SliderFloat("Power", &m_eRadial_Desc.fRadial_Power, 0.0f, 1.0f, "Power = %.3f");
	ImGui::SliderFloat("Quality", &m_eRadial_Desc.fRadial_Quality, 0.0f, 20.0f, "Quality = %.3f");

	m_pGameInstance->Get_Renderer()->Set_Radial_Blur_Active(m_eRadial_Desc.bRadial_Active);

	m_pGameInstance->Get_Renderer()->Set_RadialBlur_Option(m_eRadial_Desc);
}

void CWindow_ShaderTool::Compress_DOF_Setting()
{
	static float Params[4] = { 0.10f, 0.20f, 0.30f, 0.4f };
	ImGui::Checkbox("DOF Active", &m_eDOF_Desc.bDOF_Active);
	ImGui::SliderFloat4("DOF Parameters", reinterpret_cast<float*>(&m_eDOF_Desc.DOFParams), -1.0f, 5.0f, "Param %0.3f");

	//ImGui::SliderFloat4("DOFParams", &m_eDOF_Desc.DOFParams, -1.0f, 1.0f, "DOFParams = %.3f");
	//ImGui::SliderFloat("Distance", &m_eDOF_Desc.fFocusDistance, 0.0f, 100.0f, "Distance = %.3f");
	//ImGui::SliderFloat("Range", &m_eDOF_Desc.fFocusRange, 0.0f, 100.0f, "Range = %.3f");
	
	m_pGameInstance->Get_Renderer()->Set_DOF_Option(m_eDOF_Desc);
}

void CWindow_ShaderTool::Compress_HDR_Setting()
{
	ImGui::Checkbox("HDR Active", &m_eHDR_Desc.bHDR_Active);

	ImGui::SliderFloat("HDR_White", &m_eHDR_Desc.fmax_white, 0.0f, 1.0f, "HDR_White = %.3f");

	m_pGameInstance->Get_Renderer()->Set_HDR_Active(m_eHDR_Desc.bHDR_Active);

	m_pGameInstance->Get_Renderer()->Set_HDR_Option(m_eHDR_Desc);
}

void CWindow_ShaderTool::Compress_FXAA_Setting()
{
	ImGui::Checkbox("FXAA Active", &m_eAnti_Desc.bFXAA_Active);

	m_pGameInstance->Get_Renderer()->Set_FXAA_Active(m_eAnti_Desc.bFXAA_Active);
	m_pGameInstance->Get_Renderer()->Set_FXAA_Option(m_eAnti_Desc);
}

void CWindow_ShaderTool::Compress_HSV_Setting()
{
	ImGui::Checkbox("HSV Active", &m_eHSV_Desc.bScreen_Active);

	ImGui::SliderFloat("Brightness", &m_eHSV_Desc.fFinal_Brightness, 0.0f, 2.0f, "Brightness = %.3f");

	ImGui::SliderFloat("Saturation", &m_eHSV_Desc.fFinal_Saturation, 0.0f, 2.0f, "Saturation = %.3f");

	m_pGameInstance->Get_Renderer()->Set_HSV_Option(m_eHSV_Desc);
}

void CWindow_ShaderTool::Compress_Vignette_Setting()
{
	ImGui::Checkbox("Vignette Active", &m_eVignette_Desc.bVignette_Active);

	ImGui::SliderFloat("fVignetteRatio", &m_eVignette_Desc.fVignetteRatio, 0.0f, 6.0f, "Ratio = %.3f");

	ImGui::SliderFloat("fVignetteRadius", &m_eVignette_Desc.fVignetteRadius, -1.0f, 3.0f, "Radius = %.3f");

	ImGui::SliderFloat("fVignetteAmount", &m_eVignette_Desc.fVignetteAmount, -2.0f, 1.0f, "Amount = %.3f");
	
	ImGui::SliderFloat("fVignetteSlope", &m_eVignette_Desc.fVignetteSlope, 0.0f, 16.0f, "Slope = %.3f");

	ImGui::SliderFloat("fVignetteCenter_X", &m_eVignette_Desc.fVignetteCenter_X, 0.0f, 1.0f, "Center_X = %.3f");

	ImGui::SliderFloat("fVignetteCenter_Y", &m_eVignette_Desc.fVignetteCenter_Y, 0.0f, 1.0f, "Center_Y = %.3f");

	m_pGameInstance->Get_Renderer()->Set_Vignette_Option(m_eVignette_Desc);
}

void CWindow_ShaderTool::Compress_ScreenEffect_Setting()
{
	ImGui::Checkbox("Gray Active", &m_eScreenDEffect_Desc.bGrayScale_Active);
	ImGui::Checkbox("Sephia Active", &m_eScreenDEffect_Desc.bSephia_Active);

	ImGui::SliderFloat("Grey Power", &m_eScreenDEffect_Desc.GreyPower, 0.0f, 1.0f, "GreyPower = %.3f");
	ImGui::SliderFloat("Sephia Power", &m_eScreenDEffect_Desc.SepiaPower, 0.0f, 1.0f, "SepiaPower = %.3f");

	m_pGameInstance->Get_Renderer()->Set_ScreenEffect_Option(m_eScreenDEffect_Desc);
	
}

void CWindow_ShaderTool::Compress_SSR_Setting()
{
	ImGui::Checkbox("SSR Active", &m_eSSR_Desc.bSSR_Active);

	ImGui::SliderFloat("RayStep", &m_eSSR_Desc.fRayStep, 0.0f, 3.0f, "RayStep = %.3f");
	ImGui::SliderFloat("RayHitThreshold", &m_eSSR_Desc.fStepCnt	, 0.0f, 150.f, "RayHitThreshold = %.3f");

	m_pGameInstance->Get_Renderer()->Set_SSR_Option(m_eSSR_Desc);
}

void CWindow_ShaderTool::Compress_Chroma_Setting()
{
	ImGui::Checkbox("Chroma Active", &m_eChroma_Desc.bChroma_Active);
	ImGui::SliderFloat("Intensity", &m_eChroma_Desc.fChromaticIntensity, 0.0f, 50.0f, "Intensity = %.3f");

	m_pGameInstance->Get_Renderer()->Set_Chroma_Option(m_eChroma_Desc);
}

void CWindow_ShaderTool::Save_Shader()
{
	string path = "../Bin/DataFiles/Data_Shader/Level/";

	string LevelString = SMath::capitalizeString(m_eCurrLevel_String);
	path += "_Shader.json";

	json Out_Json;

	Out_Json["HBAO"]["bHBAO_Active"] = m_eHBAO_Desc.bHBAO_Active;
	Out_Json["HBAO"]["fBias"] = m_eHBAO_Desc.fBias;
	Out_Json["HBAO"]["fBlur_Sharpness"] = m_eHBAO_Desc.fBlur_Sharpness;
	Out_Json["HBAO"]["fPowerExponent"] = m_eHBAO_Desc.fPowerExponent;
	Out_Json["HBAO"]["fRadius"] = m_eHBAO_Desc.fRadius;

	Out_Json["Deferred"]["bRimBloom_Blur_Active"] = m_eDeferred_Desc.bRimBloom_Blur_Active;
	Out_Json["Deferred"]["bShadow_Active"] = m_eDeferred_Desc.bShadow_Active;

	Out_Json["Fog"]["bFog_Active"] = m_eFog_Desc.bFog_Active;
	Out_Json["Fog"]["fFogStartDepth"] = m_eFog_Desc.fFogStartDepth;
	Out_Json["Fog"]["fFogStartDistance"] = m_eFog_Desc.fFogStartDistance;
	Out_Json["Fog"]["fFogDistanceValue"] = m_eFog_Desc.fFogDistanceValue;
	Out_Json["Fog"]["fFogHeightValue"] = m_eFog_Desc.fFogHeightValue;
	Out_Json["Fog"]["fFogDistanceDensity"] = m_eFog_Desc.fFogDistanceDensity;
	Out_Json["Fog"]["fFogHeightDensity"] = m_eFog_Desc.fFogHeightDensity;
	Out_Json["Fog"]["vFogColor_x"] = m_eFog_Desc.vFogColor.x;
	Out_Json["Fog"]["vFogColor_y"] = m_eFog_Desc.vFogColor.y;
	Out_Json["Fog"]["vFogColor_z"] = m_eFog_Desc.vFogColor.z;
	Out_Json["Fog"]["vFogColor_w"] = m_eFog_Desc.vFogColor.w;

	Out_Json["HDR"]["bHDR_Active"] = m_eHDR_Desc.bHDR_Active;
	Out_Json["HDR"]["fmax_white"] = m_eHDR_Desc.fmax_white;

	Out_Json["Anti"]["bFXAA_Active"] = m_eAnti_Desc.bFXAA_Active;

	Out_Json["HSV"]["bScreen_Active"] = m_eHSV_Desc.bScreen_Active;
	Out_Json["HSV"]["fFinal_Saturation"] = m_eHSV_Desc.fFinal_Saturation;
	Out_Json["HSV"]["fFinal_Brightness"] = m_eHSV_Desc.fFinal_Brightness;

	Out_Json["Radial"]["bRadial_Active"] = m_eRadial_Desc.bRadial_Active;
	Out_Json["Radial"]["fRadial_Quality"] = m_eRadial_Desc.fRadial_Quality;
	Out_Json["Radial"]["fRadial_Power"] = m_eRadial_Desc.fRadial_Power;

	Out_Json["DOF"]["bDOF_Active"] = m_eDOF_Desc.bDOF_Active;
	//Out_Json["DOF"]["fFocusDistance"] = m_eDOF_Desc.fFocusDistance;
	//Out_Json["DOF"]["fFocusRange"] = m_eDOF_Desc.fFocusRange;

	CJson_Utility::Save_Json(path.c_str(), Out_Json);

}

#pragma endregion

#pragma region [LAYER] : Object Shader

void CWindow_ShaderTool::Layer_Object_Shader_Control()
{
}

#pragma endregion

#pragma region [LOAD] : Level 불러오기 

void CWindow_ShaderTool::Select_Level()
{
	ImGui::SeparatorText(" Select Level ");
	/* 레벨셋팅 - 어떤레벨에 대한 셋팅을할것인지 지정 */
	const char* items[] = { "None", "GamePlay", "Intro", "Intro Boss", "SnowMountain", "Lava" };
	static int item_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo(" ", combo_preview_value))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_current_idx = n;
				m_iCurrLevel_Index = n;
				switch (m_iCurrLevel_Index)
				{
				case 0: // None
					break;
				case 1: // GamePlay
					m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/Stage1Final_MapData.json";
					m_eCurrLevel_Enum = LEVEL::LEVEL_GAMEPLAY;
					m_eCurrLevel_String = "LEVEL_GAMEPLAY";
					break;
				case 2: // Intro
					m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/Stage1Final_MapData.json";
					m_eCurrLevel_Enum = LEVEL::LEVEL_INTRO;
					m_eCurrLevel_String = "LEVEL_INTRO";
					break;
				case 3: //Intro Boss 
					m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/Stage1BossAddLight_MapData.json";
					m_eCurrLevel_Enum = LEVEL::LEVEL_INTRO_BOSS;
					m_eCurrLevel_String = "LEVEL_INTRO_BOSS";
					break;
				case 4:
					m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/SnowMountainNormalMapping_MapData.json";
					m_eCurrLevel_Enum = LEVEL::LEVEL_SNOWMOUNTAIN;
					m_eCurrLevel_String = "LEVEL_SNOWMOUNTAIN";
					break;
				case 5:
					m_strStage1MapLoadPath = "../Bin/DataFiles/Data_Map/Stage2Boss_TestMap_MapData.json";
					m_eCurrLevel_Enum = LEVEL::LEVEL_LAVA;
					m_eCurrLevel_String = "LEVEL_LAVA";
					break;
				}
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Load Model"))
	{
		if (m_iCurrLevel_Index > 0)
			Load_Level(m_iCurrLevel_Index); /* 인덱스에 해당하는 레벨 불러오기 */
	}

	ImGui::SameLine();

	if (ImGui::Button("Load Level Shader"))
	{
		Load_LevelShader(m_iCurrLevel_Index);
	}

	ImGui::Separator();
}

HRESULT CWindow_ShaderTool::Load_Level(_int iLevel_Index)
{
	if (0 == iLevel_Index)
	{
		ImGui::Text("wrong choice");
		return S_OK;
	}
	
	m_wstrLayerTag = TEXT("Layer_BackGround");

	/* 기존 스카이박스 있어서 해제함 */
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky")));

	json Stage1MapJson = {};

	if (FAILED(CJson_Utility::Load_Json(m_strStage1MapLoadPath.c_str(), Stage1MapJson)))
	{
		MSG_BOX("맵 불러오기 실패");
		return E_FAIL;
	}

	json BasicJson = Stage1MapJson["Basic_Json"];
	_int iBasicJsonSize = (_int)BasicJson.size();

	for (_int i = 0; i < iBasicJsonSize; ++i)
	{
		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;

		Desc.bAnimModel = BasicJson[i]["AnimType"];

		wstring strLoadModelTag;
		string strJsonModelTag = BasicJson[i]["ModelTag"];

		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		Desc.strModelTag = strLoadModelTag;

		Desc.iShaderPassIndex = BasicJson[i]["ShaderPassIndex"];
		Desc.iPlayAnimationIndex = BasicJson[i]["PlayAnimationIndex"];
		Desc.bPreview = false;

		const json& TransformJson = BasicJson[i]["Component"]["Transform"];
		_float4x4 WorldMatrix;

		for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
		{
			for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
			{
				WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
			}
		}

		XMStoreFloat4(&Desc.vPos, XMLoadFloat4x4(&WorldMatrix).r[3]);
		Desc.WorldMatrix = WorldMatrix;

		CEnvironment_Object* pObject = { nullptr };

		pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
	}

	json InteractJson = Stage1MapJson["Interact_Json"];
	_int InteractJsonSize = (_int)InteractJson.size();

	for (_int i = 0; i < InteractJsonSize; ++i)
	{

		//TODO 추후 상호작용 오브젝트 클래스 작성  후 작업
		//! L"Layer_Event"
	}

	json InstanceJson = Stage1MapJson["Instance_Json"];
	_int InstanceJsonSize = (_int)InstanceJson.size();

	for (_int i = 0; i < InstanceJsonSize; ++i)
	{
		MAPTOOL_INSTANCE_DESC InstanceDesc;

		InstanceDesc.iNumInstance = InstanceJson[i]["InstanceCount"];


		wstring strLoadModelTag;
		string strJsonModelTag = InstanceJson[i]["ModelTag"];

		m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
		InstanceDesc.strModelTag = strLoadModelTag;

		InstanceDesc.iShaderPassIndex = InstanceJson[i]["ShaderPassIndex"];

		json InstanceInfoJson = InstanceJson[i]["InstanceInfo_Json"];
		_uint InstanceInfoJsonSize = (_uint)InstanceInfoJson.size();

		for (_uint j = 0; j < InstanceInfoJsonSize; ++j)
		{
			INSTANCE_INFO_DESC InstanceInfoDesc = {};

			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Scale"], InstanceInfoDesc.vScale);
			CJson_Utility::Load_Float4(InstanceInfoJson[j]["Instance_Rotation"], InstanceInfoDesc.vRotation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Translation"], InstanceInfoDesc.vTranslation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Center"], InstanceInfoDesc.vCenter);

			InstanceDesc.vecInstanceInfoDesc.push_back(InstanceInfoDesc);
		}

		CEnvironment_Instance* pInstanceObject = { nullptr };

		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));
	}

	json MonsterJson = Stage1MapJson["Monster_Json"];
	_int iMonsterJsonSize = (_int)MonsterJson.size();

	for (_int i = 0; i < iMonsterJsonSize; ++i)
	{
		CMonster_Character::MONSTER_DESC MonsterDesc = {};

		string LoadMonsterTag = (string(MonsterJson[i]["PrototypeTag"]));

		m_pGameInstance->String_To_WString(LoadMonsterTag, MonsterDesc.strProtoTypeTag);
		MonsterDesc.bPreview = false;
		MonsterDesc.eDescType = CGameObject::MONSTER_DESC;

		const json& TransformJson = MonsterJson[i]["Component"]["Transform"];
		_float4x4 WorldMatrix;

		for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
		{
			for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
			{
				WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
			}
		}

		MonsterDesc.WorldMatrix = WorldMatrix;

		FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, L"Layer_Monster", MonsterDesc.strProtoTypeTag, &MonsterDesc));
	}
	return S_OK;

}

HRESULT CWindow_ShaderTool::Load_LevelShader(_int iLevel_Index)
{
	return S_OK;
}

#pragma endregion

#pragma region Initialize

void CWindow_ShaderTool::Top_Setting()
{
	/* 최상위 셋팅 - 렌더타겟 끄고 켜기 */
	ImGui::SeparatorText("Priority Setting");

#ifdef _DEBUG
	if (ImGui::Checkbox(u8"RenderTarget", &bRenderTarget_Active))
		m_pGameInstance->Set_RenderDebugTarget(bRenderTarget_Active);

	ImGui::SameLine();

	if (ImGui::Checkbox(u8"DebugCom", &bRenderCom_Active))
		m_pGameInstance->Set_RenderDebugCom(bRenderCom_Active);

#endif // _DEBUG

	ImGui::SameLine();
	HelpMarker(u8"렌더타겟 끄고 켜기");

	ImGui::Spacing();
}


void CWindow_ShaderTool::Imgui_Setting()
{
	/* 아임구이 셋팅 */
	ImGuiWindowFlags Flag = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
		/*	ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |*/
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

	SetUp_ImGuiDESC("Shader", ImVec2{ 400.f, 300.f }, Flag, ImVec4(0.f, 0.f, 0.f, 0.8f));
	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	style.FrameRounding = 8.0f; /* 값이 클수록 모서리가 더 둥글게된다. */
	/* 투명하게 */
	const ImVec4 bgColor = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_TitleBg] = bgColor;
}

CWindow_ShaderTool* CWindow_ShaderTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_ShaderTool* pInstance = new CWindow_ShaderTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_ShaderTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_ShaderTool::Free()
{
	__super::Free();
}

_bool CWindow_ShaderTool::Check_ImGui_Rect()
{
	/* Imgui창 내에 닿는지 확인용도 - 모든창마다 해야한다. */
	POINT m_tMouse_Screen;
	GetCursorPos(&m_tMouse_Screen);
	ScreenToClient(g_hWnd, &m_tMouse_Screen);

	ImVec2 windowPos = ImGui::GetWindowPos(); //왼쪽상단모서리점
	ImVec2 windowSize = ImGui::GetWindowSize();

	if (m_tMouse_Screen.x >= windowPos.x && m_tMouse_Screen.x <= windowPos.x + windowSize.x &&
		m_tMouse_Screen.y >= windowPos.y && m_tMouse_Screen.y <= windowPos.y + windowSize.y)
	{
		return true; //ImGui 영역 내
	}

	return false; //ImGui 영역이랑 안 겹침!
}
#pragma endregion

#pragma region Save / Load

HRESULT CWindow_ShaderTool::Save_Function(string strPath, string strFileName)
{
	json Out_Json;
	Write_Json(Out_Json);
	CJson_Utility::Save_Json(strPath.c_str(), Out_Json);

	return S_OK;
}

HRESULT CWindow_ShaderTool::Load_Function(string strPath, string strFileName)
{
	__super::Load_Function(strPath, strFileName);

	return S_OK;
}

_bool CWindow_ShaderTool::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	/* Light Type */
	//Out_Json["Shader"]["Light_Type"] = ;

	return _bool();
}


#pragma endregion
