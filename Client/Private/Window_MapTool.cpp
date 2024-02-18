#include "stdafx.h"
#include "Window_MapTool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

#include "Environment_Instance.h"
#include "Environment_Object.h"
#include "Field.h"

#include "LandObject.h"

#include "../Imgui/ImGuizmo/ImGuizmo.h"
#include "../Imgui/ImGuizmo/ImCurveEdit.h"
#include "../Imgui/ImGuizmo/GraphEditor.h"
#include "../Imgui/ImGuizmo/ImSequencer.h"
#include "../Imgui/ImGuizmo/ImZoomSlider.h"

static ImGuizmo::OPERATION InstanceCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE	   InstanceCurrentGizmoMode(ImGuizmo::WORLD);
static bool InstanceuseSnap(false);


CWindow_MapTool::CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_MapTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.

	FAILED_CHECK(__super::Initialize());


	//! Loader에서 푸시백 해놓은 Imgui_Manager의 모델태그 벡터를 받아오자.
	FAILED_CHECK(Ready_ModelTags());

	

// 	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, L"Layer_BackGround", TEXT("Prototype_GameObject_Terrain")));
// 	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, L"Layer_BackGround", TEXT("Prototype_GameObject_Sky")));

	
	
	

		//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Skydome"))))
		//	return E_FAIL;
	
	return S_OK;
}

void CWindow_MapTool::Tick(_float fTimeDelta)
{


	__super::Tick(fTimeDelta);

	__super::Begin();

	

	//TODO ImGuiTabBarFlags_
	//!	ImGuiTabBarFlags_None = 0,
	
	//!	ImGuiTabBarFlags_Reorderable = 1 << 0,   // 수동으로 탭을 끌어서 순서를 변경할 수 있음 + 새 탭이 목록 끝에 추가됨
	//!	ImGuiTabBarFlags_AutoSelectNewTabs = 1 << 1,   // 새 탭이 나타나면 자동으로 선택
	//!	ImGuiTabBarFlags_TabListPopupButton = 1 << 2,   // 탭 목록 팝업을 열려면 버튼을 비활성화하세요.
	//!	ImGuiTabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,   // 마우스 가운데 버튼을 사용하여 탭 닫기 동작(p_open != NULL로 제출됨)을 비활성화합니다. if (IsItemHovered() && IsMouseClicked(2)) *p_open = false를 사용하여 사용자 측에서 이 동작을 재현할 수 있습니다.
	//!	ImGuiTabBarFlags_NoTabListScrollingButtons = 1 << 4,   //스크롤 버튼 비활성화(피팅 정책이 ImGuiTabBarFlags_FittingPolicyScroll인 경우 적용)
	//!	ImGuiTabBarFlags_NoTooltip = 1 << 5,   //탭을 가리킬 때 도구 설명 비활성화
	//!	ImGuiTabBarFlags_FittingPolicyResizeDown = 1 << 6,   // 탭이 맞지 않는 경우 탭 크기 조정
	//!	ImGuiTabBarFlags_FittingPolicyScroll = 1 << 7,   // 탭이 맞지 않을 때 스크롤 버튼 추가
	//!	ImGuiTabBarFlags_FittingPolicyMask_ = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_FittingPolicyScroll,
	//!	ImGuiTabBarFlags_FittingPolicyDefault_ = ImGuiTabBarFlags_FittingPolicyResizeDown,

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;
	

	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 80), ImGuiChildFlags_Border, WindowFlag);
	
	ImGui::SeparatorText(u8"세이브 / 로드");
	{
		if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
	}ImGui::Separator(); 

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Create_RightChild", ImVec2(0, 80), ImGuiChildFlags_Border, WindowFlag);
	
	FieldWindowMenu(); //! 필드 창 보이기 감추기

	ImGui::EndChild();

	

	

	if (ImGui::BeginTabBar(u8"오브젝트 타입", tab_bar_flags))
	{

		if (ImGui::BeginTabItem(u8"그라운드"))
		{
			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_GROUND;
			GroundTab_Function();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"상호작용"))
		{
			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_INTERACT;
			InteractTab_Function();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"환경"))
		{
			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT;
			EnvironmentTab_Function();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		ShowDialog();
	}

	if (m_eModeType != CWindow_MapTool::MODE_TYPE::MODE_CREATE && nullptr != m_pPreviewObject)
	{
		m_pPreviewObject->Set_Dead(true);
		m_pPreviewObject = nullptr;
	}


	__super::End();


	
}

void CWindow_MapTool::Render()
{

}


HRESULT CWindow_MapTool::Save_Function(string strPath, string strFileName)
{
 	
 
 	string strNoExtFileName = filesystem::path(strFileName).stem().string();
	

 	string strBasic = "Basic";
 	string strInstance = "Instance";
 
 	for (auto& tag : m_vecCreateObjectTag)
 	{
 		// 문자열에서 '@' 문자 이후의 부분을 지움
 		size_t atIndex = tag.find('@');
 		if (atIndex != std::string::npos) {
 			tag.erase(atIndex); // '@' 이후의 문자열을 모두 제거
 		}
 	}
 
 	for (auto& tag : m_vecCreateInstanceTag)
 	{
 		// 문자열에서 '@' 문자 이후의 부분을 지움
 		size_t atIndex = tag.find('@');
 		if (atIndex != std::string::npos) {
 			tag.erase(atIndex); // '@' 이후의 문자열을 모두 제거
 		}
 	}
	
			json SaveJson = {};



			json BasicJson = {};
			

			if (false == m_vecCreateObject.empty())
			{
				_int iCreateObjectSize = m_vecCreateObject.size();

			

				for (_int i = 0; i < iCreateObjectSize; ++i)
				{
					CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;

					Desc = *m_vecCreateObject[i]->Get_EnvironmentDesc();

					string strModelTag;
					m_pGameInstance->WString_To_String(m_vecCreateObject[i]->Get_ModelTag(), strModelTag);

					BasicJson[i].emplace("Type", strBasic);
					BasicJson[i].emplace("Index", i);
					BasicJson[i].emplace("ObjectTag", m_vecCreateObjectTag[i]);
					BasicJson[i].emplace("LayerTag", L"Layer_BackGround");
					BasicJson[i].emplace("ModelTag", strModelTag);
					BasicJson[i].emplace("AnimType", Desc.bAnimModel);
					BasicJson[i].emplace("ShaderPassIndex", Desc.iShaderPassIndex);
					BasicJson[i].emplace("PlayAnimationIndex", Desc.iPlayAnimationIndex);

					m_vecCreateObject[i]->Write_Json(BasicJson[i]);
				}

			}

			json InteractJson = {};


			json InstanceJson = {};
 
 
			if (false == m_vecCreateInstance.empty())
			{
				_int iCreateInstanceObjectSize = m_vecCreateInstance.size();

				

				for (_int i = 0; i < iCreateInstanceObjectSize; ++i)
				{
					MAPTOOL_INSTANCE_DESC InstanceObjDesc = m_vecCreateInstance[i]->Get_InstanceDesc();

					string strModelTag;
					m_pGameInstance->WString_To_String(m_vecCreateInstance[i]->Get_ModelTag(), strModelTag);

					InstanceJson[i].emplace("Type", strInstance);
					InstanceJson[i].emplace("Index", i);
					InstanceJson[i].emplace("ObjectTag", m_vecCreateInstanceTag[i]);
					InstanceJson[i].emplace("ModelTag", strModelTag);
					InstanceJson[i].emplace("LayerTag", L"Layer_BackGround");
					InstanceJson[i].emplace("ShaderPassIndex", InstanceObjDesc.iShaderPassIndex);
					InstanceJson[i].emplace("InstanceCount", InstanceObjDesc.iNumInstance);

					json InstanceInfoJson = {};


					for (_int j = 0; j < InstanceObjDesc.iNumInstance; ++j)
					{
						INSTANCE_INFO_DESC InstanceInfoDesc = InstanceObjDesc.vecInstanceInfoDesc[j];

						InstanceInfoJson[j].emplace("Instance_Index", j);
						CJson_Utility::Write_Float3(InstanceInfoJson[j]["Instance_Scale"], XMLoadFloat3(&InstanceInfoDesc.vScale));
						CJson_Utility::Write_Float3(InstanceInfoJson[j]["Instance_Rotation"], XMLoadFloat3(&InstanceInfoDesc.vRotation));
						CJson_Utility::Write_Float3(InstanceInfoJson[j]["Instance_Translation"], XMLoadFloat3(&InstanceInfoDesc.vTranslation));
						CJson_Utility::Write_Float3(InstanceInfoJson[j]["Instance_Center"], XMLoadFloat3(&InstanceInfoDesc.vCenter));

					}

					InstanceJson[i].emplace("InstanceInfo_Json", InstanceInfoJson);

					m_vecCreateInstance[i]->Write_Json(InstanceJson[i]);
				}

				
			}


			SaveJson.emplace("Basic_Json", BasicJson);
			SaveJson.emplace("Interact_Json", InteractJson);
			SaveJson.emplace("Instance_Json", InstanceJson);

			string strSavePath = strPath + "/" + strNoExtFileName + "_MapData.json";
			if (FAILED(CJson_Utility::Save_Json(strSavePath.c_str(), SaveJson)))
			{
				MSG_BOX("맵툴 저장 실패");
			}
			else
			{
				MSG_BOX("맵툴 저장 성공");
			}

			return S_OK;
}

HRESULT CWindow_MapTool::Load_Function(string strPath, string strFileName)
{

	json LoadJson;

	string strFullPath = strPath + "/" + strFileName;

	if (FAILED(CJson_Utility::Load_Json(strFullPath.c_str(), LoadJson)))
	{
		MSG_BOX("맵툴 불러오기 실패");
		return E_FAIL;
	}
	else
		Reset_Function();

	

	json BasicJson = LoadJson["Basic_Json"];
	_int iBasicJsonSize = BasicJson.size();

	for (_int i = 0; i < iBasicJsonSize; ++i)
	{
		string IndexTag = "@" + to_string(i);

		string pushObjectTag = (string)BasicJson[i]["ObjectTag"] + IndexTag;
		
		m_vecCreateObjectTag.push_back(pushObjectTag);

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

		CEnvironment_Object* pObject = { nullptr};

		pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

		m_vecCreateObject.push_back(pObject);
	}


	json InteractJson = LoadJson["Interact_Json"];
	_int InteractJsonSize = InteractJson.size();

	for(_int i = 0; i < InteractJsonSize; ++i)
	{
		string IndexTag = "@" + to_string(i);

		string pushObjectTag = string(InteractJson[i]["ObjectTag"]) + IndexTag;
		
		//TODO 추후 상호작용 오브젝트 클래스 작성  후 작업
		//! L"Layer_Event"
	}

	json InstanceJson = LoadJson["Instance_Json"];
	_int InstanceJsonSize = InstanceJson.size();

	for(_int i = 0; i < InstanceJsonSize; ++i)
	{
		string IndexTag = "@" + to_string(i);

		string pushObjectTag = string(InstanceJson[i]["ObjectTag"]) + IndexTag;
 			
 		m_vecCreateInstanceTag.push_back(pushObjectTag);
 
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
 
 		m_vecCreateInstance.push_back(pInstanceObject);
 	}

	return S_OK;
}

void CWindow_MapTool::Reset_Function()
{
	m_pPreviewObject = nullptr;
	m_pPickingObject = nullptr;
	m_pPickingInstanceInfo = nullptr;

	_int iCreateObjectSize = m_vecCreateObject.size();

	for (_int i = 0; i < iCreateObjectSize; ++i)
	{
		m_vecCreateObject[i]->Set_Dead(true);
	}

	m_iSelectObjectIndex = 0;
	m_iCreateObjectIndex = 0;
	m_vecCreateObject.clear();
	m_vecCreateObjectTag.clear();


	_int iCreateInstanceSize = m_vecCreateInstance.size();

	for (_int i = 0; i < iCreateInstanceSize; ++i)
	{
		m_vecCreateInstance[i]->Set_Dead(true);
	}

	m_iSelectInstanceIndex = 0;
	m_iSelectEnvironmentIndex = 0;
	m_vecCreateInstance.clear();
	m_vecCreateInstanceTag.clear();
	m_vecInstanceInfoTag.clear();
	m_iInstanceInfoTagIndex = 0;
	

	_int iPreviewInstanceSize = m_vecPreViewInstance.size();

	for (_int i = 0; i < iPreviewInstanceSize; ++i)
	{
		m_vecPreViewInstance[i]->Set_Dead(true);
	}

	m_vecPreViewInstance.clear();

	

	


}

HRESULT CWindow_MapTool::Ready_ModelTags()
{
	//! 애님태그
	
	map<wstring, CImgui_Manager::JSY_MODEL_TYPE> mapAnimTags = *CImgui_Manager::GetInstance()->Get_Anim_E_ModelTag();

	for (auto& PAIR : mapAnimTags)
	{
		string strAnimTag = {};

		m_pGameInstance->WString_To_String(PAIR.first, strAnimTag);

		m_mapAnimModelTag.emplace(strAnimTag, (MAP_KEY_TYPE)PAIR.second);

		switch ((MAP_KEY_TYPE)PAIR.second)
		{
			case MAP_KEY_TYPE::MODEL_ENVIRONMENT:
			{
				m_vecAnimEnviroModelTag.push_back(strAnimTag);
				break;
			}

			case MAP_KEY_TYPE::MODEL_INTERACT:
			{
				m_vecAnimInteractModelTag.push_back(strAnimTag);
				break;
			}
		}
	}

	//!논애님 태그
	map<wstring, CImgui_Manager::JSY_MODEL_TYPE> mapNonAnimTags = *CImgui_Manager::GetInstance()->Get_NonAnim_E_ModelTag();
	

	for (auto& PAIR : mapNonAnimTags)
	{
		string strNonAnimTag = {};

		m_pGameInstance->WString_To_String(PAIR.first, strNonAnimTag);
		

		m_mapNonAnimModelTag.emplace(strNonAnimTag, (MAP_KEY_TYPE)PAIR.second);

		switch ((MAP_KEY_TYPE)PAIR.second)
		{
			case MAP_KEY_TYPE::MODEL_GROUND:
			{
				m_vecGroundModelTag.push_back(strNonAnimTag);
				break;
			}

			case MAP_KEY_TYPE::MODEL_ENVIRONMENT:
			{
				m_vecEnviroModelTag.push_back(strNonAnimTag);
				break;
			}

			case MAP_KEY_TYPE::MODEL_INTERACT:
			{
				m_vecInteractModelTag.push_back(strNonAnimTag);
				break;
			}
		}
	}

	
	return S_OK;
}

void CWindow_MapTool::GroundTab_Function()
{


	ImGui::SeparatorText(u8"펑션 타입");
	{
		static _int iModeIndex = 0;
		const char* FunctionTypeName[3] = { u8"픽킹 / 생성", u8"픽킹 / 선택", u8"픽킹 / 삭제"};
		
		for (_uint i = 0; i < IM_ARRAYSIZE(FunctionTypeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(FunctionTypeName[i], &iModeIndex, i);
		}

		m_eModeType = (MODE_TYPE)iModeIndex;

		ImGui::NewLine();

		static _int iPickingMode = 0;
		const char* PickingModeName[3] = { u8"마우스 프레싱", u8"마우스 다운", u8"마우스 업" };

		for (_uint i = 0; i < IM_ARRAYSIZE(PickingModeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(PickingModeName[i], &iPickingMode, i);
		}

		m_ePickingMode = (PICKING_MODE)iPickingMode;

	}ImGui::Separator(); ImGui::NewLine();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;
	
	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
			{
				ImGui::SeparatorText(u8"픽킹 타입");
				{
					static _int iPickingType = 0;
					const char* PickingTypeName[3] = { u8"필드 픽킹", u8"메쉬 픽킹", u8"픽킹 정지"};

					for (_uint i = 0; i < IM_ARRAYSIZE(PickingTypeName); ++i)
					{
						if (i > 0) { ImGui::SameLine(); }
						ImGui::RadioButton(PickingTypeName[i], &iPickingType, i);
					}
					
					m_ePickingType = (PICKING_TYPE)iPickingType;

				}ImGui::Separator(); ImGui::NewLine();

				ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);
					
				_uint iModelTagSize = m_vecGroundModelTag.size();

				if (ImGui::BeginListBox(u8"모델 태그 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (_uint i = 0; i < iModelTagSize; ++i)
					{
						const _bool isSelected = (m_iSelectModelTag == i);

						if (ImGui::Selectable(m_vecGroundModelTag[i].c_str(), isSelected))
						{
							m_iSelectModelTag = i;
							m_bChange = true;

							
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
					}
					ImGui::EndListBox();
				}
				 
				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);
				
				ImGui::Text(u8"테스트2");

				ImGui::EndChild();

				Preview_Function();

				switch (m_ePickingMode)
				{
					case Client::CWindow_MapTool::PICKING_MODE::MOUSE_PRESSING:
					{
						if (true == m_pGameInstance->Mouse_Pressing(DIM_LB))
							Picking_Function();
						break;
					}
					case Client::CWindow_MapTool::PICKING_MODE::MOUSE_DOWN:
					{
						if (true == m_pGameInstance->Mouse_Down(DIM_LB))
							Picking_Function();
						break;
					}
					case Client::CWindow_MapTool::PICKING_MODE::MOUSE_UP:
					{
						if (true == m_pGameInstance->Mouse_Up(DIM_LB))
							Picking_Function();
						break;
					}
				}

				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
			{
				Basic_SelectFunction();
				Guizmo_Tick(m_pPickingObject);
				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
			{
				
				break;
			}
	}
	
}

void CWindow_MapTool::InteractTab_Function()
{
	ImGui::SeparatorText(u8"펑션 타입");
	{
		static _int iModeIndex = 0;
		const char* FunctionTypeName[3] = { u8"픽킹 / 생성_상호작용", u8"픽킹 / 선택_상호작용", u8"픽킹 / 삭제_상호작용" };

		for (_uint i = 0; i < IM_ARRAYSIZE(FunctionTypeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(FunctionTypeName[i], &iModeIndex, i);
		}

		m_eModeType = (MODE_TYPE)iModeIndex;

		ImGui::NewLine();


		static _int iPickingMode = 0;
		const char* PickingModeName[3] = { u8"마우스 프레싱", u8"마우스 다운", u8"마우스 업" };

		for (_uint i = 0; i < IM_ARRAYSIZE(PickingModeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(PickingModeName[i], &iPickingMode, i);
		}

		m_ePickingMode = (PICKING_MODE)iPickingMode;
	}ImGui::Separator(); ImGui::NewLine();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
	case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
	{
		ImGui::SeparatorText(u8"픽킹 타입");
		{
			static _int iPickingType = 0;
			const char* PickingTypeName[3] = { u8"필드 픽킹", u8"메쉬 픽킹", u8"픽킹 정지" };

			for (_uint i = 0; i < IM_ARRAYSIZE(PickingTypeName); ++i)
			{
				if (i > 0) { ImGui::SameLine(); }
				ImGui::RadioButton(PickingTypeName[i], &iPickingType, i);
			}

			m_ePickingType = (PICKING_TYPE)iPickingType;

		}ImGui::Separator(); ImGui::NewLine();

		
		ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

		_uint iModelTagSize = m_vecInteractModelTag.size();

		if (ImGui::BeginListBox(u8"모델 태그 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iModelTagSize; ++i)
			{
				const _bool isSelected = (m_iSelectModelTag == i);

				if (ImGui::Selectable(m_vecInteractModelTag[i].c_str(), isSelected))
				{
					m_iSelectModelTag = i;
					m_bChange = true;
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);

		ImGui::Text(u8"테스트2");

		ImGui::EndChild();

		Preview_Function();

		switch (m_ePickingMode)
		{
			case Client::CWindow_MapTool::PICKING_MODE::MOUSE_PRESSING:
			{
				if (true == m_pGameInstance->Mouse_Pressing(DIM_LB))
					Picking_Function();
				break;
			}
			case Client::CWindow_MapTool::PICKING_MODE::MOUSE_DOWN:
			{
				if (true == m_pGameInstance->Mouse_Down(DIM_LB))
					Picking_Function();
				break;
			}
			case Client::CWindow_MapTool::PICKING_MODE::MOUSE_UP:
			{
				if (true == m_pGameInstance->Mouse_Up(DIM_LB))
					Picking_Function();
				break;
			}
		}
		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
	{
		Basic_SelectFunction();
		Guizmo_Tick(m_pPickingObject);
		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
	{

		break;
	}
	}
}

void CWindow_MapTool::EnvironmentTab_Function()
{
	ImGui::SeparatorText(u8"펑션 타입");
	{
		static _int iModeIndex = 0;
		const char* FunctionTypeName[3] = { u8"픽킹 / 생성_인스턴싱", u8"픽킹 / 선택_인스턴싱", u8"픽킹 / 삭제_인스턴싱" };

		for (_uint i = 0; i < IM_ARRAYSIZE(FunctionTypeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(FunctionTypeName[i], &iModeIndex, i);
		}

		m_eModeType = (MODE_TYPE)iModeIndex;

		ImGui::NewLine();

		static _int iPickingMode = 0;
		const char* PickingModeName[3] = { u8"마우스 프레싱", u8"마우스 다운", u8"마우스 업" };

		for (_uint i = 0; i < IM_ARRAYSIZE(PickingModeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(PickingModeName[i], &iPickingMode, i);
		}

		m_ePickingMode = (PICKING_MODE)iPickingMode;

	}ImGui::Separator(); ImGui::NewLine();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
	case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
	{
		ImGui::SeparatorText(u8"픽킹 타입");
		{
			static _int iPickingType = 0;
			const char* PickingTypeName[3] = { u8"필드 픽킹", u8"메쉬 픽킹", u8"픽킹 정지" };

			for (_uint i = 0; i < IM_ARRAYSIZE(PickingTypeName); ++i)
			{
				if (i > 0) { ImGui::SameLine(); }
				ImGui::RadioButton(PickingTypeName[i], &iPickingType, i);
			}

			m_ePickingType = (PICKING_TYPE)iPickingType;

		}ImGui::Separator(); ImGui::NewLine();

		_uint iModelTagSize = m_vecEnviroModelTag.size();

		if (ImGui::BeginListBox(u8"모델 태그 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iModelTagSize; ++i)
			{
				const _bool isSelected = (m_iSelectModelTag == i);

				if (ImGui::Selectable(m_vecEnviroModelTag[i].c_str(), isSelected))
				{
					m_iSelectModelTag = i;
					m_bChange = true;

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					
					}
				}
			}
			ImGui::EndListBox();
		}

			ImGui::Separator();
			{
				if (ImGui::Button(u8"인스턴스 생성"))
				{
					Create_Instance();
				}
			}

		Preview_Function();


		switch (m_ePickingMode)
		{
			case Client::CWindow_MapTool::PICKING_MODE::MOUSE_PRESSING:
			{
				if (true == m_pGameInstance->Mouse_Pressing(DIM_LB))
					Picking_Function();
				break;
			}
			case Client::CWindow_MapTool::PICKING_MODE::MOUSE_DOWN:
			{
				if (true == m_pGameInstance->Mouse_Down(DIM_LB))
					Picking_Function();
				break;
			}
			case Client::CWindow_MapTool::PICKING_MODE::MOUSE_UP:
			{
				if (true == m_pGameInstance->Mouse_Up(DIM_LB))
					Picking_Function();
				break;
			}
		}
		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
	{
		m_pPickingObject = nullptr;

		if (true == m_vecCreateInstance.empty())
		{
			ImGui::Text(u8"인스턴스할 환경 객체를 생성하지 않았습니다.");
			break;
		}

		ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

		_int iCreateInstanceSize = m_vecCreateInstance.size();

		if (ImGui::BeginListBox(u8"환경 객체 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iCreateInstanceSize; ++i)
			{
				const _bool isSelected = (m_iSelectEnvironmentIndex == i);

				if (ImGui::Selectable(m_vecCreateInstanceTag[i].c_str(), isSelected))
				{
					m_iSelectEnvironmentIndex = i;

					m_vecInstanceInfoTag.clear();
					m_iInstanceInfoTagIndex = 0;

					vector<INSTANCE_INFO_DESC> Desc = *m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfoDesc();

					_int iInstanceInfoSize = Desc.size();

					for (_int i = 0; i < iInstanceInfoSize; ++i)
					{
						string strInstanceTag = "Instance" + to_string(m_iInstanceInfoTagIndex);

						m_vecInstanceInfoTag.push_back(strInstanceTag);

						m_iInstanceInfoTagIndex++;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		if (false == m_vecInstanceInfoTag.empty())
		{
			ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);


			if (ImGui::BeginListBox(u8"인스턴스 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{


				
				vector<INSTANCE_INFO_DESC> Desc = *m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfoDesc();

				_int iNumInstance = Desc.size();


				for (_uint i = 0; i < iNumInstance; ++i)
				{
					const _bool isSelected = (m_iSelectInstanceIndex == i);

					if (ImGui::Selectable(m_vecInstanceInfoTag[i].c_str(), isSelected))
					{
						m_iSelectInstanceIndex = i;
						m_pPickingInstanceInfo = m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfo(m_iSelectInstanceIndex);

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndListBox();
			}

			ImGui::EndChild();

			Instance_GuizmoTick(m_iSelectEnvironmentIndex, m_pPickingInstanceInfo);

		}
		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
	{

		break;
	}
	}
}



void CWindow_MapTool::MouseInfo_Window(_float fTimeDelta)
{
	
	if (true == m_bShowCreateField)
	{
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysHorizontalScrollbar;

		ImGui::Begin(u8"셋팅", nullptr, WindowFlags);

			if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD && true == ImGui_MouseInCheck())
			{
				if (nullptr != m_pField)
				{
					ImGui::SeparatorText(u8"필드 픽킹 인포");

					m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
					m_fRayPos = m_pField->GetMousePos(m_tWorldRay);

				
					ImGui::Text(u8"마우스 X: %.2f", m_fRayPos.x); ImGui::SameLine();	ImGui::Text(u8"마우스 Y: %.2f", m_fRayPos.y); ImGui::SameLine();	ImGui::Text(u8"마우스 Z: %.2f", m_fRayPos.z);

					ImGui::Separator();

					ImGui::NewLine();
				}
			}

			else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH && true == ImGui_MouseInCheck())
			{
				if(true == m_vecCreateObject.empty())
					return;
				
					ImGui::SeparatorText(u8"메쉬 픽킹 인포");

					m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
					

					_int iCreateObjectSize = m_vecCreateObject.size();

					for (_int i = 0; i < iCreateObjectSize; ++i)
					{
						

						if(true == m_vecCreateObject[i]->Picking(&m_fRayPos))
						{
							
							XMStoreFloat3(&m_fMeshPos, XMVector3TransformCoord(XMLoadFloat3(&m_fRayPos), m_vecCreateObject[i]->Get_Transform()->Get_WorldMatrix()));
						}
					}	
					
					

					ImGui::Text(u8"마우스 X: %.2f", m_fMeshPos.x); ImGui::SameLine();	ImGui::Text(u8"마우스 Y: %.2f", m_fMeshPos.y); ImGui::SameLine();	ImGui::Text(u8"마우스 Z: %.2f", m_fMeshPos.z);

					ImGui::Separator();

					ImGui::NewLine();
			}
		
		
		
			ImGui::SeparatorText(u8"필드");

			ImGui::InputFloat(u8"X 사이즈", &m_fFieldSizeX);
			ImGui::InputFloat(u8"Z 사이즈", &m_fFieldSizeZ);

			if (ImGui::Button(u8"픽킹용 필드 생성"))
			{
				VTXFIELD FieldInfo;

				FieldInfo.vPosition.x = m_fFieldSizeX;
				FieldInfo.vPosition.y = 1.f;
				FieldInfo.vPosition.z = m_fFieldSizeZ;

				m_pField = dynamic_cast<CField*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Field", &FieldInfo));
			}

			ImGui::Separator();
		
		ImGui::End();
	}
}

void CWindow_MapTool::FieldWindowMenu()
{
	ImGui::SeparatorText(u8"필드");
	{

		static _int iFieldShowType = 0;
		const char* FieldShowMode[2] = { u8"필드 에디터 보기", u8"필드 에디터 감추기" };

		for (_uint i = 0; i < IM_ARRAYSIZE(FieldShowMode); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(FieldShowMode[i], &iFieldShowType, i);
		}

		m_bShowCreateField = !(_bool)iFieldShowType;

	}ImGui::NewLine();

	MouseInfo_Window(m_fTimeDelta);

}


void CWindow_MapTool::Preview_Function()
{
	Change_PreViewObject(m_eTabType);

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_NONE || m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_END)
		return;

	if (m_bDeadComplete == true && nullptr != m_pPreviewObject)
	{
		_vector vPos = {};

		if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
			vPos = { m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f};
		else if(m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
			vPos = { m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f };


		
		m_pPreviewObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

void CWindow_MapTool::Change_PreViewObject(TAP_TYPE eTabType)
{	
	if (m_bChange == true && m_pPreviewObject != nullptr)
	{
		m_pPreviewObject->Set_Dead(true);
		//! Dead 처리되는 걸 잠시 기다려주자.
		
		m_bChange = false;
		m_pPreviewObject = nullptr;
	}
	
	if (nullptr == m_pPreviewObject)
	{
		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;

		switch (eTabType)
		{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_GROUND:
			m_pGameInstance->String_To_WString(m_vecGroundModelTag[m_iSelectModelTag], Desc.strModelTag);
			break;
		case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
			m_pGameInstance->String_To_WString(m_vecInteractModelTag[m_iSelectModelTag], Desc.strModelTag);
			break;
		case Client::CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT:
			m_pGameInstance->String_To_WString(m_vecEnviroModelTag[m_iSelectModelTag], Desc.strModelTag);
			break;
		}

		m_pPreviewObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_Object", &Desc));

		m_pPreviewObject->Get_Transform()->Set_Position(m_fRayPos);
	}
}

void CWindow_MapTool::Picking_Function()
{

	if (nullptr != m_pPreviewObject && true == ImGui_MouseInCheck())
	{
		
		switch (m_eTabType)
		{
			case Client::CWindow_MapTool::TAP_TYPE::TAB_GROUND:
			{
				Ground_CreateFunction();
				break;
			}
			case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
			{
				Interact_CreateFunction();
				break;
			}
			case Client::CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT:
			{
				Preview_Environment_CreateFunction();
				break;
			}
		}

	}

}

void CWindow_MapTool::Ground_CreateFunction()
{
	if(m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (nullptr != m_pField && true == m_pField->MouseOnTerrain())
		{
			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
			Desc.bAnimModel = m_bAnimType;
			Desc.iShaderPassIndex = m_iShaderPassIndex;
			Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
			Desc.bPreview = false;
			Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldMatrix();
		
			CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));
			
			m_vecCreateObject.push_back(pObject);
			
			
			wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateObjectIndex));
			string strConvertTag;
			m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
			m_vecCreateObjectTag.push_back(strConvertTag);

			m_iCreateObjectIndex++;
			//m_vecCreateObjectTag
		}
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{
		if(m_vecCreateObject.empty())
			return;
		
		_int iCreateObjectSize = m_vecCreateObject.size();

		for (_int i = 0; i < iCreateObjectSize; ++i)
		{
			//TODO 작성중. 메쉬 픽킹
// 			_float3 vPickedPos = {};
// 
// 			if (true == m_vecCreateObject[i]->Picking(&vPickedPos))
// 			{
				CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
				Desc.bAnimModel = m_bAnimType;
				Desc.iShaderPassIndex = m_iShaderPassIndex;
				Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
				Desc.bPreview = false;
				Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldMatrix();

				CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

				m_vecCreateObject.push_back(pObject);

				wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateObjectIndex));
				string strConvertTag;
				m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
				m_vecCreateObjectTag.push_back(strConvertTag);

				m_iCreateObjectIndex++;
//			}
			
		}

	}

		
}

void CWindow_MapTool::Interact_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;
}

void CWindow_MapTool::Preview_Environment_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if(nullptr == m_pField)
			return;

		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
		Desc.bAnimModel = m_bAnimType;
		Desc.iShaderPassIndex = m_iShaderPassIndex;
		Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
		Desc.bPreview = false;
		Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldMatrix();

		CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

		m_vecPreViewInstance.push_back(pObject);
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{
		if (m_vecCreateObject.empty())
			return;

		_int iCreateObjectSize = m_vecCreateObject.size();

		for (_int i = 0; i < iCreateObjectSize; ++i)
		{
			//TODO 작성중. 메쉬 픽킹
// 			_float3 vPickedPos = {};
// 
// 			if (true == m_vecCreateObject[i]->Picking(&vPickedPos))
// 			{
			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
			Desc.bAnimModel = m_bAnimType;
			Desc.iShaderPassIndex = m_iShaderPassIndex;
			Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
			Desc.bPreview = false;
			Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldMatrix();

			CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

			m_vecPreViewInstance.push_back(pObject);
			//			}
			
		}

	}
}

void CWindow_MapTool::Create_Instance()
{
	
	MAPTOOL_INSTANCE_DESC InstanceDesc;

	InstanceDesc.iNumInstance = m_vecPreViewInstance.size();
	InstanceDesc.iShaderPassIndex = m_iShaderPassIndex;
	InstanceDesc.strModelTag = m_pPreviewObject->Get_ModelTag();
	
	for (_int i = 0; i < InstanceDesc.iNumInstance; ++i)
	{
		INSTANCE_INFO_DESC InfoDesc;

		CTransform* pTransform = m_vecPreViewInstance[i]->Get_Transform();
		
		InfoDesc.vRotation = pTransform->Get_Rotated();
		InfoDesc.vScale = pTransform->Get_Scaled();
		InfoDesc.vTranslation = pTransform->Get_State(CTransform::STATE_POSITION);
	
		InstanceDesc.vecInstanceInfoDesc.push_back(InfoDesc);

		m_vecPreViewInstance[i]->Set_Dead(true); //! 미리보기 오브젝트는 삭제시켜주자

	}

		
	CEnvironment_Instance* pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));

	
//	InstanceDesc.vecInstanceInfoDesc.clear();
	m_vecCreateInstance.push_back(pInstanceObject);

	wstring strCreateInstanceTag = m_pGameInstance->SliceObjectTag(pInstanceObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateInstanceIndex));
	string strConvertTag;
	m_pGameInstance->WString_To_String(strCreateInstanceTag, strConvertTag);
	m_vecCreateInstanceTag.push_back(strConvertTag);
	m_iCreateInstanceIndex++;
	m_vecPreViewInstance.clear();
	
	
}

void CWindow_MapTool::Basic_SelectFunction()
{
	_uint iObjectTagSize = m_vecCreateObject.size();

	if (true == m_vecCreateObject.empty())
	{
		ImGui::Text(u8"생성한 객체가 없습니다. ");
	}
	else
	{
		if (ImGui::BeginListBox(u8"그라운드 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iObjectTagSize; ++i)
			{
				const _bool isSelected = (m_iSelectObjectIndex == i);

				if (ImGui::Selectable(m_vecCreateObjectTag[i].c_str(), isSelected))
				{
					m_iSelectObjectIndex = i;

					m_pPickingObject = m_vecCreateObject[m_iSelectObjectIndex];

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}
	}

	
}

void CWindow_MapTool::Guizmo_Tick(CGameObject* pPickingObject)
{
	if(nullptr == pPickingObject)
		return;

	Set_GuizmoCamView();
	Set_GuizmoCamProj();
	Set_Guizmo(pPickingObject);
}


//! 이벤트매니저가
//!	플레이어 위치
//!	트리거
//!     이거 각자이벤트에호출

//! 상호작용 오브젝트가 이벤트 끝났다고 불변수
	//!	 각자 자기가 해야할 함수 들 다갖고있어. 자기가 수행해야할 함수를 알고있어
	  //!		내가 불러
		//! 씬 넘어가면 

//! 일정거리갔을때 무너져.. 그 이후로는 계속 랜덤으로 파편생성

//! 수행해야할 함수와
//!  수행한상태인지 끝난 상태인지
//!   애니메이션 인덱스
//!	이벤트 발생 조건.
//!	   이넘을



void CWindow_MapTool::Instance_GuizmoTick(_int iIndex, INSTANCE_INFO_DESC* pInstance)
{

	if (true == m_vecCreateInstance.empty() || nullptr == pInstance )
		return;

		m_pPickingObject = nullptr;

		Set_GuizmoCamView();
		Set_GuizmoCamProj();

			
		/*==== Set ImGuizmo ====*/
		ImGuizmo::SetOrthographic(false);
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


		if (ImGui::IsKeyPressed(ImGuiKey_T))
			InstanceCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R))
			InstanceCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			InstanceCurrentGizmoOperation = ImGuizmo::SCALE;

		if (ImGui::RadioButton("Translate", InstanceCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			InstanceCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", InstanceCurrentGizmoOperation == ImGuizmo::ROTATE))
			InstanceCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", InstanceCurrentGizmoOperation == ImGuizmo::SCALE))
			InstanceCurrentGizmoOperation = ImGuizmo::SCALE;

		_float* arrView = m_arrView;
		_float* arrProj = m_arrProj;

		_matrix matWorld = pInstance->Get_Matrix();
		float arrWorld[16];
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(arrWorld), matWorld);

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
		ImGui::DragFloat3("Tr", matrixTranslation);
		ImGui::DragFloat3("Rt", matrixRotation);
		ImGui::DragFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);


		ImGui::Checkbox("UseSnap", &InstanceuseSnap);
		ImGui::SameLine();

		switch (InstanceCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::DragFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::DragFloat3("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::DragFloat3("Scale Snap", &snap[0]);
			break;
		}

		pInstance->vTranslation = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
		pInstance->vRotation = { matrixRotation[0], matrixRotation[1], matrixRotation[2] };
		pInstance->vScale = { matrixScale[0], matrixScale[1], matrixScale[2] };

		
		ImGuizmo::Manipulate(m_arrView, m_arrProj, InstanceCurrentGizmoOperation, InstanceCurrentGizmoMode, arrWorld, NULL, InstanceuseSnap ? &snap[0] : NULL);
		m_vecCreateInstance[iIndex]->Update(*pInstance, m_iSelectInstanceIndex);
}






CWindow_MapTool* CWindow_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_MapTool* pInstance = new CWindow_MapTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_MapTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_MapTool::Free()
{
	__super::Free();


	if(m_pField != nullptr)
		Safe_Release(m_pField);
}