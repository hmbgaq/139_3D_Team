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

	return S_OK;
}


void CWindow_ShaderTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Imgui_Setting();

	__super::Begin();

	Top_Setting(); /* 최상위 셋팅 */

	Select_Level(); /* 스테이지 선택 및 불러오기 */

	if (ImGui::CollapsingHeader("Level Shader"))
		Level_Shader_Control();

	ImGui::Spacing(); /* 여백 */

	if (ImGui::CollapsingHeader("Object Shader"))
		Object_Shader_Control();

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

void CWindow_ShaderTool::Level_Shader_Control()
{
	ImGui::SeparatorText("Shader Active ");

	ImGui::Text("Pre-Post");
	{
		if (ImGui::Checkbox(u8"HBAO+ Active", &m_bHBAO_Plus_Active))
			m_pGameInstance->Get_Renderer()->Set_SSAO(m_bHBAO_Plus_Active);

		ImGui::SameLine();

		if (ImGui::Checkbox(u8"RimLight Active", &m_bRimLight_Active))
			int a = 0;
			//m_pGameInstance->Get_Renderer()->Set_SSAO(m_bRimLight_Active);
	}

	ImGui::Text("Post");
	{
		if (ImGui::Checkbox(u8"HDR Active", &m_bHDR_Active))
			m_pGameInstance->Get_Renderer()->Set_HDR(m_bHDR_Active);

		ImGui::SameLine();

		if (ImGui::Checkbox(u8"FXAA Active", &m_bFXAA_Active))
			m_pGameInstance->Get_Renderer()->Set_FXAA(m_bFXAA_Active);

	}

}

void CWindow_ShaderTool::Object_Shader_Control()
{
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

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(m_eCurrLevel_Enum, m_wstrLayerTag, TEXT("Prototype_GameObject_Sky")));

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

		pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
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
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Rotation"], InstanceInfoDesc.vRotation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Translation"], InstanceInfoDesc.vTranslation);
			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Center"], InstanceInfoDesc.vCenter);

			InstanceDesc.vecInstanceInfoDesc.push_back(InstanceInfoDesc);
		}


		CEnvironment_Instance* pInstanceObject = { nullptr };

		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(m_eCurrLevel_Enum, m_wstrLayerTag, L"Prototype_GameObject_Environment_Instance", &InstanceDesc));
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
