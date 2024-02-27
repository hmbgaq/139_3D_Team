#include "stdafx.h"
#include "Window_ShaderTool.h"
#include "GameInstance.h"
#include "Environment_Object.h"
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

	return S_OK;
}

void CWindow_ShaderTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Imgui_Setting();

	__super::Begin();

	Top_Setting(); /* 최상위 셋팅 */

	Select_Level(); /* 스테이지 선택 및 불러오기 */

	if (ImGui::CollapsingHeader("Light Control"))
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

void CWindow_ShaderTool::Top_Setting()
{
	/* 최상위 셋팅 - 렌더타겟 끄고 켜기 */
	ImGui::SeparatorText("Priority Setting");

	if (ImGui::Checkbox(u8"RenderTarget Active", &bRenderTarget_Active))
	{
		m_pGameInstance->Set_RenderDebug(bRenderTarget_Active);
	}

	ImGui::SameLine();
	HelpMarker(u8"렌더타겟 끄고 켜기");

	ImGui::Spacing();
}

void CWindow_ShaderTool::Layer_Light_Control()
{
	if (ImGui::TreeNode("Directional Light "))
	{
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Spot Light"))
	{
		ImGui::TreePop();
	}
}

void CWindow_ShaderTool::Layer_Level_Shader_Control()
{
	ImGui::SeparatorText("Pre-Post");

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
	if (ImGui::TreeNode("Screen Setting"))
	{
		Compress_Screen_Setting();
		ImGui::TreePop();
	}
}

void CWindow_ShaderTool::Layer_Object_Shader_Control()
{
}

void CWindow_ShaderTool::Compress_HBAO_Plus_Setting()
{
	ImGui::Checkbox("HBAO+ Active", &m_eHBAO_Desc.bHBAO_Active);

	ImGui::SliderFloat("Radius", &m_eHBAO_Desc.fRadius, 0.1f, 5.0f, "Radius = %.3f");

	ImGui::SliderFloat("Bias", &m_eHBAO_Desc.fBias, 0.1f, 1.0f, "Bias = %.3f");

	ImGui::SliderFloat("Power Exponent", &m_eHBAO_Desc.fPowerExponent, 0.1f, 3.0f, "Power = %.3f");

	ImGui::SliderFloat("Blur Sharpness", &m_eHBAO_Desc.fBlur_Sharpness, 1.f, 20.0f, "Sharpness = %.3f");

	m_pGameInstance->Get_Renderer()->Set_SSAO(m_eHBAO_Desc.bHBAO_Active);

	m_pGameInstance->Get_Renderer()->Set_HBAO_Option(m_eHBAO_Desc);
}

void CWindow_ShaderTool::Compress_Fog_Setting()
{
	ImGui::Checkbox("Fog Active", &m_eFog_Desc.bFog_Active);

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		m_eFog_Desc.fFogStartDepth		= 55.f;
		m_eFog_Desc.fFogStartDistance	= 0.1f;
		m_eFog_Desc.fFogDistanceValue	= 30.f;
		m_eFog_Desc.fFogHeightValue		= 50.f;
		m_eFog_Desc.fFogDistanceDensity = 0.05f;
		m_eFog_Desc.fFogHeightDensity	= 0.05f;
	}

	ImGui::SliderFloat("FogStartDepth", &m_eFog_Desc.fFogStartDepth, 0.001f, 250.0f, "StartDepth = %.3f");

	ImGui::SliderFloat("FogStartDistance", &m_eFog_Desc.fFogStartDistance, 0.001f, 30.0f, "StartDistance = %.3f");

	ImGui::SliderFloat("FogDistanceValue", &m_eFog_Desc.fFogDistanceValue, 0.001f, 50.f, "FogDistanceValue = %.3f");

	ImGui::SliderFloat("FogHeightValue", &m_eFog_Desc.fFogHeightValue, 0.001f, 100.f, "HeightValue = %.3f");

	ImGui::SliderFloat("FogDistanceDensity", &m_eFog_Desc.fFogDistanceDensity, 0.001f, 1.0f, "FogDistanceValue = %.3f");

	ImGui::SliderFloat("FogHeightDensity", &m_eFog_Desc.fFogHeightDensity, 0.001f, 1.0f, "HeightDensity = %.3f");

	m_pGameInstance->Get_Renderer()->Set_Fog(m_eFog_Desc.bFog_Active);

	m_pGameInstance->Get_Renderer()->Set_Fog_Option(m_eFog_Desc);
}

void CWindow_ShaderTool::Compress_HDR_Setting()
{
	ImGui::Checkbox("HDR Active", &m_eHDR_Desc.bHDR_Active);

	ImGui::SliderFloat("HDR_White", &m_eHDR_Desc.fmax_white, 0.0f, 1.0f, "HDR_White = %.3f");

	m_pGameInstance->Get_Renderer()->Set_HDR(m_eHDR_Desc.bHDR_Active);

	m_pGameInstance->Get_Renderer()->Set_HDR_Option(m_eHDR_Desc);
}

void CWindow_ShaderTool::Compress_FXAA_Setting()
{
	ImGui::Checkbox("FXAA Active", &m_eScreen_Desc.bFXAA_Active);

	m_pGameInstance->Get_Renderer()->Set_FXAA(m_eScreen_Desc.bFXAA_Active);
	m_pGameInstance->Get_Renderer()->Set_Screen_Option(m_eScreen_Desc);
}

void CWindow_ShaderTool::Compress_Screen_Setting()
{
	ImGui::SliderFloat("Brightness", &m_eScreen_Desc.fFinal_Brightness, 0.0f, 2.0f, "Brightness = %.3f");

	ImGui::SliderFloat("Saturation", &m_eScreen_Desc.fFinal_Saturation, 0.0f, 2.0f, "Saturation = %.3f");

	m_pGameInstance->Get_Renderer()->Set_Screen_Option(m_eScreen_Desc);
}


#pragma region Level 불러오기 

void CWindow_ShaderTool::Select_Level()
{
	/* 레벨셋팅 - 어떤레벨에 대한 셋팅을할것인지 지정 */
	ImGui::SeparatorText("Level Setting ");

	const char* items[] = { "None", "Test", "SnowMountain", "Larva" };
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
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		if (m_iCurrLevel_Index > 0)
			Load_Level(m_iCurrLevel_Index); /* 인덱스에 해당하는 레벨 불러오기 */
	}

	ImGui::Separator();
}

HRESULT CWindow_ShaderTool::Load_Level(_int iLevel_Index)
{
	switch (iLevel_Index)
	{
	case 0:  /* None */
		ImGui::Text("wrong choice");
		break;
	case 1: /* Test */
		m_eCurrLevel_Enum = LEVEL::LEVEL_GAMEPLAY;
		break;
	case 2: /* SnowMountain */
		m_eCurrLevel_Enum = LEVEL::LEVEL_SNOWMOUNTAIN;
		break;
	case 3: /* Larva */
		m_eCurrLevel_Enum = LEVEL::LEVEL_LAVA;
		break;
	}

	if (0 == iLevel_Index)
		return S_OK;

	m_wstrLayerTag = TEXT("Layer_BackGround");

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, m_wstrLayerTag, TEXT("Prototype_GameObject_Sky")));

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
	_int InteractJsonSize = InteractJson.size();

	for (_int i = 0; i < InteractJsonSize; ++i)
	{

		//TODO 추후 상호작용 오브젝트 클래스 작성  후 작업
		//! L"Layer_Event"
	}

	json InstanceJson = Stage1MapJson["Instance_Json"];
	_int InstanceJsonSize = InstanceJson.size();

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
		_uint InstanceInfoJsonSize = InstanceInfoJson.size();

		for (_int j = 0; j < InstanceInfoJsonSize; ++j)
		{
			INSTANCE_INFO_DESC InstanceInfoDesc = {};

			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Scale"], InstanceInfoDesc.vScale);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Rotation"], InstanceInfoDesc.vRotation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Translation"], InstanceInfoDesc.vTranslation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Center"], InstanceInfoDesc.vCenter);

			InstanceDesc.vecInstanceInfoDesc.push_back(InstanceInfoDesc);
		}


		CEnvironment_Instance* pInstanceObject = { nullptr };

		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));

	}

	return S_OK;
}

#pragma endregion

void CWindow_ShaderTool::Imgui_Setting()
{
	/* 아임구이 셋팅 */
	SetUp_ImGuiDESC("Shader", ImVec2{ 400.f, 300.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.8f));

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
