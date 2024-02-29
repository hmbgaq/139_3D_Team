#include "stdafx.h"
#include "Window_MapTool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

#include "Environment_Instance.h"
#include "Environment_Object.h"
#include "Field.h"

#include "LandObject.h"
#include "Player.h"
#include "Monster.h"

#include "../Imgui/ImGuizmo/ImGuizmo.h"
#include "../Imgui/ImGuizmo/ImCurveEdit.h"
#include "../Imgui/ImGuizmo/GraphEditor.h"
#include "../Imgui/ImGuizmo/ImSequencer.h"
#include "../Imgui/ImGuizmo/ImZoomSlider.h"
#include "Camera.h"
#include "SpringCamera.h"

static ImGuizmo::OPERATION InstanceCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE	   InstanceCurrentGizmoMode(ImGuizmo::WORLD);
static bool InstanceuseSnap(false);


CWindow_MapTool::CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_MapTool::Initialize()
{
	FAILED_CHECK(__super::Initialize());

	FAILED_CHECK(Ready_ModelTags());
	FAILED_CHECK(Ready_PrototypeTags());
	

	_int iEnvironModelTagSize = m_vecEnviroModelTag.size();
	for (int i = 0; i < iEnvironModelTagSize; ++i) //! 인스턴싱용 깡통맵
	{
		vector<CEnvironment_Object*> EmptyVector;

		m_mapPreviewInstance.emplace(m_vecEnviroModelTag[i], EmptyVector);
	}
	

	//m_mapPreviewInstance
	
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

	CameraWindow_Function();

	ImGui::SeparatorText(u8"오브젝트 타입");
	{

		static _int iObjectType = 0;
		const char* CharObjectType[2] = { u8"환경", u8"캐릭터" };

		for (_uint i = 0; i < IM_ARRAYSIZE(CharObjectType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			if (ImGui::RadioButton(CharObjectType[i], &iObjectType, i))
			{
				m_bChangeObjectMode = true;
				
				m_eObjectMode = ECast<OBJECTMODE_TYPE>(iObjectType);
				ObjectMode_Change_For_Reset();
			}
		}
		ObjectMode_Change_For_Reset();
	}
	ImGui::Separator();

	
	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		EnvironmentMode_Function();
	}
	else //! OBJECTMODE_CHARACTER
	{
		CharacterMode_Function();
	}

	

	if (m_eModeType != CWindow_MapTool::MODE_TYPE::MODE_CREATE && m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT && nullptr != m_pPreviewObject)
	{
		m_pPreviewObject->Set_Dead(true);
		m_pPreviewObject = nullptr;
	}
	else if (m_eModeType != CWindow_MapTool::MODE_TYPE::MODE_CREATE && m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER && nullptr != m_pPreviewCharacter)
	{
		m_pPreviewCharacter->Set_Dead(true);
		m_pPreviewCharacter = nullptr;
	}

	__super::End();

	if (false == m_bCreateCamera)
	{
		IsCreatePlayer_ReadyCamara();
	}

	
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

	for (auto& tag : m_vecCreateMonsterTag)
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
				_int iCreateObjectSize = (_int)m_vecCreateObject.size();

			

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
				_int iCreateInstanceObjectSize = (_int)m_vecCreateInstance.size();

				

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


					for (_uint j = 0; j < InstanceObjDesc.iNumInstance; ++j)
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

			json MonsterJson;

			if (false == m_vecCreateMonster.empty())
			{
				_int iCreateMonsterSize = (_int)m_vecCreateMonster.size();
			
			
			
				for (_int i = 0; i < iCreateMonsterSize; ++i)
				{
					CMonster::MONSTER_DESC Desc;
			
					Desc = *m_vecCreateMonster[i]->Get_MonsterDesc();
			
					string strProtoTag = m_pGameInstance->Wstring_To_UTF8(Desc.strProtoTypeTag);
					MonsterJson[i].emplace("PrototypeTag", strProtoTag);
					m_vecCreateMonster[i]->Write_Json(MonsterJson[i]);
				}
			}
			
			//todo 추후 작성 npc
			
			//json NPCJson;
			//
			//if (false == m_vecCreateNPC.empty())
			//{
			//	_int iCreateNPCSize = (_int)m_vecCreateNPC.size();
			//
			//	for (_int i = 0; i < iCreateNPCSize; ++i)
			//	{
			//		CNPC::NPC_DESC Desc;
			//
			//		Desc = *m_vecCreateNPC[i]->Get_NPCDesc();
			//
			//		string strProtoTag = m_pGameInstance->Wstring_To_UTF8(Desc.strProtoTypeTag);
			//		NPCJson[i].emplace("PrototypeTag", strProtoTag);
			//		m_vecCreateNPC[i]->Write_Json(NPCJson[i]);
			//	}
			//}




			SaveJson.emplace("Basic_Json", BasicJson);
			SaveJson.emplace("Interact_Json", InteractJson);
			SaveJson.emplace("Instance_Json", InstanceJson);
			SaveJson.emplace("Monster_Json", MonsterJson);
			//! 추후 작성 npc SaveJson.emplace("NPC_Json", NPCJson);

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
	_int iBasicJsonSize = (_int)BasicJson.size();

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
		m_iCreateObjectIndex++;
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
	_int InstanceJsonSize = (_int)InstanceJson.size();

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
		_uint InstanceInfoJsonSize = (_uint)InstanceInfoJson.size();
 
 		for (_uint j = 0; j < InstanceInfoJsonSize; ++j)
 		{
 			INSTANCE_INFO_DESC InstanceInfoDesc = {};
 
 			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Scale"], InstanceInfoDesc.vScale);
 			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Rotation"], InstanceInfoDesc.vRotation);
 			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Translation"], InstanceInfoDesc.vTranslation);
 			CJson_Utility::Load_Float3(InstanceInfoJson[j]["Instance_Center"], InstanceInfoDesc.vCenter);
 
 			InstanceDesc.vecInstanceInfoDesc.push_back(InstanceInfoDesc);
			m_iInstanceInfoTagIndex++;
			
 		}
 
 
 		CEnvironment_Instance* pInstanceObject = { nullptr };
 
 		pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));
 
 		m_vecCreateInstance.push_back(pInstanceObject);
		m_iCreateInstanceIndex++;
 	}

	json MonsterJson = LoadJson["Monster_Json"];
	_int iMonsterJsonSize = (_int)MonsterJson.size();

	for (_int i = 0; i < iMonsterJsonSize; ++i)
	{
		string pushMonsterTag = (string)MonsterJson[i]["PrototypeTag"] + "@" + to_string(i);

		m_vecCreateMonsterTag.push_back(pushMonsterTag);

		CMonster::MONSTER_DESC MonsterDesc;
		MonsterDesc.bPreview = false;


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

		CMonster* pMonster = { nullptr };

		wstring strProtoTypeTag;
		m_pGameInstance->String_To_WString((string)MonsterJson[i]["PrototypeTag"], strProtoTypeTag);

		pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strProtoTypeTag, &MonsterDesc));

		m_vecCreateMonster.push_back(pMonster);
		m_iCreateMonsterIndex++;
	}

	return S_OK;
}

void CWindow_MapTool::Reset_Function()
{
	m_pPickingObject = nullptr;

	if (m_pPreviewObject != nullptr)
		m_pPreviewObject->Set_Dead(true);
	
	if(m_pPreviewCharacter != nullptr)
		m_pPreviewCharacter->Set_Dead(true);


	m_pPreviewObject = nullptr;
	m_pPreviewCharacter = nullptr;
	m_pPickingInstanceInfo = nullptr;

		

	_int iCreateObjectSize = (_int)m_vecCreateObject.size();

	for (_int i = 0; i < iCreateObjectSize; ++i)
	{
		m_vecCreateObject[i]->Set_Dead(true);
	}

	m_iSelectObjectIndex = 0;
	m_iCreateObjectIndex = 0;
	m_vecCreateObject.clear();
	m_vecCreateObjectTag.clear();


	_int iCreateInstanceSize = (_int)m_vecCreateInstance.size();

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
	

	_int iPreviewInstanceSize = (_int)m_vecPreViewInstance.size();

	for (_int i = 0; i < iPreviewInstanceSize; ++i)
	{
		m_vecPreViewInstance[i]->Set_Dead(true);
	}

	m_vecPreViewInstance.clear();

	


	_int iCreateMonsterSize = (_int)m_vecCreateMonster.size();

	for (_int i = 0; i < iCreateMonsterSize; ++i)
	{
		m_vecCreateMonster[i]->Set_Dead(false);
	}

	m_iCreateMonsterIndex = 0;
	m_iSelectCharacterTag = 0; 
	m_vecCreateMonster.clear();
	m_vecCreateMonsterTag.clear();

	
	
	//!_int iCreateNPCSize = (_int)m_vecCreateNPC.size();
	//!
	//!for (_int i = 0; i < iCreateNPCSize; ++i)
	//!{
	//!	m_vecCreateNPC[i]->Set_Dead(true);
	//!}
	//!
	//!m_vecCreateNPC.clear();

}

void CWindow_MapTool::ObjectMode_Change_For_Reset()
{
	if (m_bChangeObjectMode == true)
	{
		if (m_eObjectMode == OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
		{
			m_iSelectCharacterTag = 0;
			m_pPickingObject = nullptr;

			if (m_pPreviewCharacter != nullptr)
			{
				m_pPreviewCharacter->Set_Enable(false);
				m_pPreviewCharacter = nullptr;
			}
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
		{
			m_pPickingObject = nullptr;
			m_iSelectEnvironmentIndex = 0;
			m_iSelectInstanceIndex = 0;
			m_iSelectObjectIndex = 0;

			if(m_pPreviewObject != nullptr)
			{
				m_pPreviewObject->Set_Enable(false);
				m_pPreviewObject = nullptr;
			}
		}

		m_bChangeObjectMode = false;
	}

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

HRESULT CWindow_MapTool::Ready_PrototypeTags()
{

	m_vecMonsterTag.push_back("Prototype_GameObject_Monster");
	m_vecMonsterTag.push_back("Prototype_GameObject_Player");
	
	//!m_vecBossTag.push_back("Prototype_GameObject_~!~!~!~");
	//!m_vecNpcTag.push_back("Prototype_GameObject_NPC~!~!~");

	//TODO 지금은 테스트용으로 직접 때려넣지만, 로더에서 원형 객체 추가할 때 매개인자 이넘 타입을 넣어서 타입에따라 오브젝트매니저에서 툴에서 불러오기용 컨테이너의 태그값만 추가로 보관한다면 불러오기 편할 것 같긴 하다.
	//! 예시 m_mapMonsterProtoTag  // map<string, OBJECT_TYPE> 

	return S_OK;
}

void CWindow_MapTool::EnvironmentMode_Function()
{
	ImGui::SeparatorText(u8"애니메이션 타입");
	{
		static _int iAnimType = 0;
		const char* AnimTypeName[2] = { u8"논애님", u8"애님" };

		for (_uint i = 0; i < IM_ARRAYSIZE(AnimTypeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			if (ImGui::RadioButton(AnimTypeName[i], &iAnimType, i))
			{
				m_bChange = true;
				m_iSelectModelTag = 0;
				m_iSelectObjectIndex = 0;
				m_iSelectEnvironmentIndex = 0;
				m_iSelectInstanceIndex = 0;
				
			}
		}

		m_eAnimType = (ANIM_TYPE)iAnimType;
		m_bAnimType = (_bool)iAnimType;

	}ImGui::Separator(); ImGui::NewLine();
	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;

	if (ImGui::BeginTabBar(u8"환경 오브젝트 타입", tab_bar_flags))
	{

		if (ImGui::BeginTabItem(u8"그라운드"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_GROUND)
			{
				m_iSelectObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_GROUND;
			GroundTab_Function(); 

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"상호작용"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_INTERACT)
			{
				m_iSelectObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_INTERACT;
			InteractTab_Function();

			ImGui::EndTabItem();
		}

	
		if (ImGui::BeginTabItem(u8"환경"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT)
			{
				m_iSelectObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT;
			EnvironmentTab_Function();

			ImGui::EndTabItem();
		}
	
		

	

		ImGui::EndTabBar();

		ShowDialog();
	}
}

void CWindow_MapTool::CharacterMode_Function()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;

	if (ImGui::BeginTabBar(u8"캐릭터 오브젝트 타입", tab_bar_flags))
	{

		if (ImGui::BeginTabItem(u8"일반 몬스터"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER)
			{
				m_iSelectCharacterTag = 0;
				m_iSelectObjectIndex = 0;
			}
			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER;
			
			MonsterTab_Function();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"NPC"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_NPC)
			{
				m_iSelectCharacterTag = 0;
				m_iSelectObjectIndex = 0;
			}
			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_NPC;
			
			NPC_Tab_Function();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		ShowDialog();
	}
}

void CWindow_MapTool::GroundTab_Function()
{

	Select_ModeType(); //! 생성, 선택, 삭제 선택, 마우스 프레싱, 다운, 업 선택
	

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;
	
	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
			{
				Create_Tab(CWindow_MapTool::TAP_TYPE::TAB_GROUND);
				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
			{
				Basic_SelectFunction();
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
	Select_ModeType(); //! 생성, 선택, 삭제 선택, 마우스 프레싱, 다운, 업 선택

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
	case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
	{
		Create_Tab(CWindow_MapTool::TAP_TYPE::TAB_INTERACT);
		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
	{
		Basic_SelectFunction();
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
	Select_ModeType(); //! 생성, 선택, 삭제 선택, 마우스 프레싱, 다운, 업 선택

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
	case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
	{
		Create_Tab(CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT);

		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
	{

		if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
		{
			Instance_SelectFunction();
		}
		else
		{
			Basic_SelectFunction();
		}
		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
	{

		break;
	}
	}
}



void CWindow_MapTool::MonsterTab_Function()
{

	Select_CharacterModeType();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
		{
			Create_Tab(CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER);
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Character_SelectFunction();
			Guizmo_Tick(m_pPickingObject);
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{

			break;
		}
	}
}


void CWindow_MapTool::NPC_Tab_Function()
{
}

void CWindow_MapTool::CameraWindow_Function()
{
	ImGui::Begin(u8"카메라 탭");
	
	if (false == m_bCreateCamera)
	{
		ImGui::Text(u8" 플레이어가 생성되지 않았습니다. ");
		ImGui::Text(u8" 애니메이션툴에서 플레이어를 생성해주세요. ");
	}
	else
	{
		ImGui::SeparatorText(u8"카메라 타입");
		{

			static _int iCameraType = 0;
			const char* CameraType[2] = { u8"프리 카메라", u8"스프링 카메라" };

			for (_uint i = 0; i < IM_ARRAYSIZE(CameraType); ++i)
			{
				if (i > 0) { ImGui::SameLine(); }
				
				if (ImGui::RadioButton(CameraType[i], &iCameraType, i))
				{
					_int iCameraCount = m_vecCameras.size();

					for (_int i = 0; i < iCameraCount; ++i)
					{
						if (i == iCameraType)
							m_vecCameras[i]->Set_Enable(true);
						else
							m_vecCameras[i]->Set_Enable(false);
						
					}
					
				}
			}
			
			ImGui::Text(u8"키보드 PAGE UP 키누를시 모드 전환입니다.");
			
			if (m_pGameInstance->Key_Down(DIK_PGUP))
			{
				
					if(IM_ARRAYSIZE(CameraType) > iCameraType + 1)
						iCameraType = iCameraType + 1;
					else
						iCameraType = 0;

					_int iCameraCount = m_vecCameras.size();

					for (_int i = 0; i < iCameraCount; ++i)
					{
						if (i == iCameraType)
							m_vecCameras[i]->Set_Enable(true);
						else
							m_vecCameras[i]->Set_Enable(false);

					}
			}

		}ImGui::NewLine();
	}
	



	ImGui::End();
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
				if (true == m_vecCreateObject.empty())
				{
					ImGui::Text(u8"생성된 환경 오브젝트가 없습니다.");
					ImGui::End();
					return;
				}
				
					ImGui::SeparatorText(u8"메쉬 픽킹 인포");


					_int iCreateObjectSize = (_int)m_vecCreateObject.size();

					for (_int i = 0; i < iCreateObjectSize; ++i)
					{
						CEnvironment_Object* pTargetObject = m_vecCreateObject[i];

						if (false == pTargetObject->Get_EnvironmentDesc()->bAnimModel)
						{
							m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);

						#ifdef DEBUG
							if (pTargetObject->Picking_VerJSY(&m_tWorldRay, &m_fRayPos))
							{
								m_fMeshPos = m_fRayPos;
							}
						#endif // DEBUG


							//if (true == pTargetObject->Picking(&m_fRayPos))
							//{
							//
							//	XMStoreFloat3(&m_fMeshPos, XMVector3TransformCoord(XMLoadFloat3(&m_fRayPos), m_vecCreateObject[i]->Get_Transform()->Get_WorldMatrix()));
							//}
						}
						else
							continue;

						
					}	
					
					ImGui::Text(u8"마우스 X: %.2f", m_fMeshPos.x); ImGui::SameLine();	ImGui::Text(u8"마우스 Y: %.2f", m_fMeshPos.y); ImGui::SameLine();	ImGui::Text(u8"마우스 Z: %.2f", m_fMeshPos.z);

					ImGui::Separator();

					ImGui::NewLine();
			}

			else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE && true == ImGui_MouseInCheck())
			{

				if (true == m_vecCreateInstance.empty())
				{
					ImGui::Text(u8"생성된 인스턴스 개체가 없습니다.");
					ImGui::End();
					return;
				}
					_int iCreateInstanceSize = m_vecCreateInstance.size();
		
					for (_int i = 0; i < iCreateInstanceSize; ++i)
					{
						m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
					
						if (true == m_vecCreateInstance[i]->Picking_Instance(&m_tWorldRay, &m_fRayPos))
						{
							m_fInstanceMeshPos = m_fRayPos;
						}
						
						
					}
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

void CWindow_MapTool::IsCreatePlayer_ReadyCamara()
{
	if (nullptr != m_pGameInstance->Get_Player())
	{
		CCamera* pFreeCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObect_Last(LEVEL_TOOL, L"Layer_Camera"));

		m_vecCameras.push_back(pFreeCamera);

		CSpringCamera::SPRING_CAMERA_DESC SpringDesc = {};

		SpringDesc.fMouseSensor = 0.05f;
		SpringDesc.fFovy = XMConvertToRadians(60.f);
		SpringDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		SpringDesc.fNear = 0.1f;
		SpringDesc.fFar = m_pGameInstance->Get_CamFar();
		SpringDesc.fSpeedPerSec = 20.f;
		SpringDesc.fRotationPerSec = XMConvertToRadians(180.f);
		SpringDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
		SpringDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

		CCamera* pSpringCamera = dynamic_cast<CCamera*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Camera", L"Prototype_GameObject_Camera_Spring", &SpringDesc));
		pSpringCamera->Set_Enable(false);

		m_vecCameras.push_back(pSpringCamera);

		


		m_bCreateCamera = true;
	}

	return;
}

void CWindow_MapTool::Select_ModeType()
{
	ImGui::SeparatorText(u8"펑션 타입");
	{
		static _int iModeIndex = 0;
		const char* FunctionTypeName[3] = { u8"픽킹 / 생성", u8"픽킹 / 선택", u8"픽킹 / 삭제" };

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
}

void CWindow_MapTool::Select_PickingType()
{
	ImGui::SeparatorText(u8"픽킹 타입");
	{
		static _int iPickingType = 0;
		const char* PickingTypeName[4] = { u8"필드 픽킹", u8"메쉬 픽킹", u8"인스턴스 픽킹", u8"픽킹 정지"};

		for (_uint i = 0; i < IM_ARRAYSIZE(PickingTypeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(PickingTypeName[i], &iPickingType, i);
		}

		m_ePickingType = (PICKING_TYPE)iPickingType;

	}ImGui::Separator(); ImGui::NewLine();
}

void CWindow_MapTool::Select_CharacterModeType()
{

	ImGui::SeparatorText(u8"펑션 타입");
	{
		static _int iModeIndex = 0;
		const char* FunctionTypeName[3] = { u8"픽킹 / 생성", u8"픽킹 / 선택", u8"픽킹 / 삭제" };

		for (_uint i = 0; i < IM_ARRAYSIZE(FunctionTypeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(FunctionTypeName[i], &iModeIndex, i);
		}

		m_eModeType = (MODE_TYPE)iModeIndex;

		ImGui::NewLine();

		static _int iPickingMode = 0;
		const char* PickingModeName[2] = { u8"마우스 다운", u8"마우스 업" };

		for (_uint i = 0; i < IM_ARRAYSIZE(PickingModeName); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }
			ImGui::RadioButton(PickingModeName[i], &iPickingMode, i);
		}

		_int iAddNum = iPickingMode + 1;
		m_ePickingMode = (PICKING_MODE)iAddNum;

	}ImGui::Separator(); ImGui::NewLine();
}


void CWindow_MapTool::Create_Tab(TAP_TYPE eTabType)
{
	_uint iModelTagSize = 0;
	_uint iSelectTag = m_iSelectModelTag;
	vector<string> vecModelTag;
	string strListBoxName = u8"";

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;


	Select_PickingType();

	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		strListBoxName = u8"모델 태그 리스트";

		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_GROUND)
		{
			iModelTagSize = (_uint)m_vecGroundModelTag.size();
			vecModelTag = m_vecGroundModelTag;
		}
		else if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_INTERACT)
		{
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
			{
				iModelTagSize = (_uint)m_vecAnimInteractModelTag.size();
				vecModelTag = m_vecAnimInteractModelTag;
			}
			else
			{
				iModelTagSize = (_uint)m_vecInteractModelTag.size();
				vecModelTag = m_vecInteractModelTag;
			}
		}
		else if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT)
		{
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
			{
				iModelTagSize = (_uint)m_vecAnimEnviroModelTag.size();
				vecModelTag = m_vecAnimEnviroModelTag;
			}
			else
			{
				iModelTagSize = (_uint)m_vecEnviroModelTag.size();
				vecModelTag = m_vecEnviroModelTag;
			}
		}
	}
	else
	{

		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER)
		{
			strListBoxName = u8"몬스터 리스트";
			iModelTagSize = (_uint)m_vecMonsterTag.size();
			vecModelTag = m_vecMonsterTag;
		}

		else if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_NPC)
		{
			strListBoxName = u8"NPC 리스트";
			iModelTagSize = (_uint)m_vecNpcTag.size();
			vecModelTag = m_vecNpcTag;
		}
	}


		if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iModelTagSize; ++i)
			{
				const _bool isSelected = (iSelectTag == i);

				if (ImGui::Selectable(vecModelTag[i].c_str(), isSelected))
				{
					if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
						m_iSelectCharacterTag = i;
					else
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

		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT)
		{
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
			{
				ImGui::Separator();
				{
					if (ImGui::Button(u8"인스턴스 생성"))
					{
						Create_Instance();
					}
				}
			}
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


}


void CWindow_MapTool::Preview_Function()
{
	Change_PreViewObject(m_eTabType);

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_NONE || m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_END)
		return;

	if (m_bDeadComplete == true && nullptr != m_pPreviewObject || nullptr != m_pPreviewCharacter)
	{

		_vector vPos = {};

		if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
			vPos = { m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f};
		else if(m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
			vPos = { m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f };
		else if(m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE)
			vPos = { m_fInstanceMeshPos.x, m_fInstanceMeshPos.y, m_fInstanceMeshPos.z, 1.f };

		if(m_pPreviewCharacter == nullptr)
			m_pPreviewObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
		else
			m_pPreviewCharacter->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);

	}
}

void CWindow_MapTool::Change_PreViewObject(TAP_TYPE eTabType)
{	
	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
	{
		if (m_bChange == true && m_pPreviewCharacter != nullptr)
		{
			m_pPreviewCharacter->Set_Enable(false);

			m_bChange = false;
			m_pPreviewCharacter = nullptr;
		}

		if (nullptr == m_pPreviewCharacter)
		{

			wstring strPrototypeTag;
			//m_vecMonsterTag[m_iSelectCharacterTag];
			switch (m_eTabType)
			{
				case Client::CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER:
					m_pGameInstance->String_To_WString(m_vecMonsterTag[m_iSelectCharacterTag], strPrototypeTag);
					break;
				case Client::CWindow_MapTool::TAP_TYPE::TAB_BOSSMONSTER:
					m_pGameInstance->String_To_WString(m_vecBossTag[m_iSelectCharacterTag], strPrototypeTag);
					break;
				case Client::CWindow_MapTool::TAP_TYPE::TAB_NPC:
					m_pGameInstance->String_To_WString(m_vecNpcTag[m_iSelectCharacterTag], strPrototypeTag);
					break;
				default:
					break;
			}

			if (strPrototypeTag != L"")
			{
				m_pPreviewCharacter = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strPrototypeTag, nullptr);

				m_pPreviewCharacter->Get_Transform()->Set_Position(m_fRayPos);
			}
			
		}
	}
	else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
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
				{
					m_pGameInstance->String_To_WString(m_vecGroundModelTag[m_iSelectModelTag], Desc.strModelTag);

					break;
				}
				case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
				{
					if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
					{
						m_pGameInstance->String_To_WString(m_vecInteractModelTag[m_iSelectModelTag], Desc.strModelTag);
					}
					else
					{
						m_pGameInstance->String_To_WString(m_vecAnimInteractModelTag[m_iSelectModelTag], Desc.strModelTag);
					}
					break;
				}
				case Client::CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT:
				{
					if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
					{
						m_pGameInstance->String_To_WString(m_vecEnviroModelTag[m_iSelectModelTag], Desc.strModelTag);

					}
					else
					{
						if (m_bAnimType == true)
						{
							Desc.bAnimModel = true;
							Desc.iPlayAnimationIndex = 0;
						}
						m_pGameInstance->String_To_WString(m_vecAnimEnviroModelTag[m_iSelectModelTag], Desc.strModelTag);
					}
					break;
				}
			}

			m_pPreviewObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_Object", &Desc));

			m_pPreviewObject->Get_Transform()->Set_Position(m_fRayPos);
		}
	}
	
	
	
	
}

void CWindow_MapTool::Picking_Function()
{


	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
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
					if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
					{
						Preview_Environment_CreateFunction();
					}
					else if(m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
					{
						Anim_Environment_CreateFunction();
					}

					break;
				}
			}

		}
	}
	else //! OBJECTMODE_CHARACTER
	{
		if (nullptr != m_pPreviewCharacter && true == ImGui_MouseInCheck())
		{
				Character_CreateFunction();
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
		
		_int iCreateObjectSize = (_int)m_vecCreateObject.size();

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

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE)
	{
		if (m_vecCreateInstance.empty())
			return;
		
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

		string strModelTag;
		m_pGameInstance->WString_To_String(Desc.strModelTag, strModelTag);

		auto iter = m_mapPreviewInstance.find(strModelTag);

		if (iter == m_mapPreviewInstance.end())
		{
			MSG_BOX("인스턴스 추가 실패유");
		}
		else
		{
			iter->second.push_back(pObject);
		}

		//

		//!m_vecPreViewInstance.push_back(pObject);
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{
		if (m_vecCreateObject.empty())
			return;

		_int iCreateObjectSize = (_int)m_vecCreateObject.size();

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

			string strModelTag;
			m_pGameInstance->WString_To_String(Desc.strModelTag, strModelTag);

			auto iter = m_mapPreviewInstance.find(strModelTag);

			if (iter == m_mapPreviewInstance.end())
			{
				MSG_BOX("인스턴스 추가 실패유");
			}
			else
			{
				iter->second.push_back(pObject);
			}
			
		}

	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE)
	{

			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
			Desc.bAnimModel = m_bAnimType;
			Desc.iShaderPassIndex = m_iShaderPassIndex;
			Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
			Desc.bPreview = false;
			Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldMatrix();

			CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

			string strModelTag;
			m_pGameInstance->WString_To_String(Desc.strModelTag, strModelTag);

			auto iter = m_mapPreviewInstance.find(strModelTag);

			if (iter == m_mapPreviewInstance.end())
			{
				MSG_BOX("인스턴스 추가 실패유");
			}
			else
			{
				iter->second.push_back(pObject);
			}
	}
}

void CWindow_MapTool::Create_Instance()
{
	
	//_int iCreateInstanceSize = m_mapPreviewInstance.size();

	for (auto& Pair : m_mapPreviewInstance)
	{
		vector<CEnvironment_Object*> pVecEnvironment = Pair.second;

		wstring strModelTag;
		m_pGameInstance->String_To_WString(Pair.first, strModelTag);

		MAPTOOL_INSTANCE_DESC InstanceDesc;

		InstanceDesc.iNumInstance = (_uint)pVecEnvironment.size();
		InstanceDesc.iShaderPassIndex = (_uint)m_iShaderPassIndex;
		InstanceDesc.strModelTag = strModelTag;

		for (_int i = 0; i < (_int)InstanceDesc.iNumInstance; ++i)
		{
			INSTANCE_INFO_DESC InfoDesc;

			CTransform* pTransform = pVecEnvironment[i]->Get_Transform();

			InfoDesc.vRotation = pTransform->Get_Rotated();
			InfoDesc.vScale = pTransform->Get_Scaled();
			InfoDesc.vTranslation = pTransform->Get_State(CTransform::STATE_POSITION);
			InfoDesc.Bake_CenterWithMatrix();

			InstanceDesc.vecInstanceInfoDesc.push_back(InfoDesc);

			pVecEnvironment[i]->Set_Dead(true); //! 미리보기 오브젝트는 삭제시켜주자
		}

		
		
		if(InstanceDesc.iNumInstance == 0)
			continue;

		CEnvironment_Instance* pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));


		//	InstanceDesc.vecInstanceInfoDesc.clear();
		m_vecCreateInstance.push_back(pInstanceObject);

		wstring strCreateInstanceTag = m_pGameInstance->SliceObjectTag(pInstanceObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateInstanceIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateInstanceTag, strConvertTag);
		m_vecCreateInstanceTag.push_back(strConvertTag);
		m_iCreateInstanceIndex++;
		pVecEnvironment.clear();

		
	}

	m_mapPreviewInstance.clear();

	_int iEnvironModelTagSize = m_vecEnviroModelTag.size();
	for (int i = 0; i < iEnvironModelTagSize; ++i) //! 인스턴싱용 깡통맵
	{
		vector<CEnvironment_Object*> EmptyVector;

		m_mapPreviewInstance.emplace(m_vecEnviroModelTag[i], EmptyVector);
	}

	//MAPTOOL_INSTANCE_DESC InstanceDesc;
	//
	//InstanceDesc.iNumInstance		= (_uint)m_vecPreViewInstance.size();
	//InstanceDesc.iShaderPassIndex	= (_uint)m_iShaderPassIndex;
	//InstanceDesc.strModelTag		= m_pPreviewObject->Get_ModelTag();
	
	//for (_int i = 0; i < (_int)InstanceDesc.iNumInstance; ++i)
	//{
	//	INSTANCE_INFO_DESC InfoDesc;

	//	CTransform* pTransform = m_vecPreViewInstance[i]->Get_Transform();
	//	
	//	InfoDesc.vRotation = pTransform->Get_Rotated();
	//	InfoDesc.vScale = pTransform->Get_Scaled();
	//	InfoDesc.vTranslation = pTransform->Get_State(CTransform::STATE_POSITION);
	//
	//	InstanceDesc.vecInstanceInfoDesc.push_back(InfoDesc);

	//	m_vecPreViewInstance[i]->Set_Dead(true); //! 미리보기 오브젝트는 삭제시켜주자

	//}

		
//	CEnvironment_Instance* pInstanceObject = dynamic_cast<CEnvironment_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Instance", &InstanceDesc));
//
//	
////	InstanceDesc.vecInstanceInfoDesc.clear();
//	m_vecCreateInstance.push_back(pInstanceObject);
//
//	wstring strCreateInstanceTag = m_pGameInstance->SliceObjectTag(pInstanceObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateInstanceIndex));
//	string strConvertTag;
//	m_pGameInstance->WString_To_String(strCreateInstanceTag, strConvertTag);
//	m_vecCreateInstanceTag.push_back(strConvertTag);
//	m_iCreateInstanceIndex++;
//	m_vecPreViewInstance.clear();
	
	
}

void CWindow_MapTool::Anim_Environment_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (nullptr != m_pField && true == m_pField->MouseOnTerrain())
		{
			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
			Desc.bAnimModel = m_bAnimType; //! 0이면 논애님 , 1이면 애님
			Desc.iPlayAnimationIndex = 0;
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
		if (m_vecCreateObject.empty())
			return;

		_int iCreateObjectSize = (_int)m_vecCreateObject.size();

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

void CWindow_MapTool::Character_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	//TODO 추후 보스, NPC 분기 시켜서 추가 현재는 몬스터로만 테스트

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (nullptr != m_pField && true == m_pField->MouseOnTerrain())
		{
			
			switch (m_eTabType)
			{
				case Client::CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER:
				{
					Monster_CreateFunction();
					break;
				}
				case Client::CWindow_MapTool::TAP_TYPE::TAB_BOSSMONSTER:
				{
					Boss_CreateFunction();
					break;
				}
				case Client::CWindow_MapTool::TAP_TYPE::TAB_NPC:
				{
					NPC_CreateFunction();
					break;
				}
				default:
					break;
			}

		
			
		}
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{
		if (m_vecCreateObject.empty())
			return;

		_int iCreateObjectSize = (_int)m_vecCreateObject.size();

		switch (m_eTabType)
		{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER:
		{
			Monster_CreateFunction();
			break;
		}
		case Client::CWindow_MapTool::TAP_TYPE::TAB_BOSSMONSTER:
		{
			Boss_CreateFunction();
			break;
		}
		case Client::CWindow_MapTool::TAP_TYPE::TAB_NPC:
		{
			NPC_CreateFunction();
			break;
		}
		default:
			break;
		}

	}
}

void CWindow_MapTool::Monster_CreateFunction()
{
	CMonster::MONSTER_DESC Desc;
	Desc.bPreview = false;
	Desc.WorldMatrix = m_pPreviewCharacter->Get_Transform()->Get_WorldMatrix();
	

	wstring strProtoTag;
	m_pGameInstance->String_To_WString(m_vecMonsterTag[m_iSelectCharacterTag], strProtoTag);

	Desc.strProtoTypeTag = strProtoTag;

	CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strProtoTag, &Desc));

	m_vecCreateMonster.push_back(pMonster);


	string strCreateMonsterTag = m_vecMonsterTag[m_iSelectCharacterTag] + "@" + to_string(m_iCreateMonsterIndex);

	m_vecCreateMonsterTag.push_back(strCreateMonsterTag);

	m_iCreateMonsterIndex++;
}

void CWindow_MapTool::Boss_CreateFunction()
{
}

void CWindow_MapTool::NPC_CreateFunction()
{
	//TODO 추후 npc 추가시

	//!CNPC::NPC_DESC Desc;
	//!Desc.bPreview = true;
	//!Desc.WorldMatrix = m_pPreviewCharacter->Get_Transform()->Get_WorldMatrix();
	//!
	//!wstring strProtoTag;
	//!m_pGameInstance->String_To_WString(m_vecNpcTag[m_iSelectCharacterTag], strProtoTag);
	//!
	//!CNPC* pNPC = dynamic_cast<CMonster*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", strProtoTag, &Desc));
	//!
	//!m_vecCreateNPC.push_back(pNPC);
	//!
	//!
	//!string strCreateNPCTag = m_vecNpcTag[m_iSelectCharacterTag] + "@" + to_string(m_iCreateNPCIndex);
	//!
	//!m_vecCreateNPCTag.push_back(strCreateNPCTag);
	//!
	//!m_iCreateNPCIndex++;
}

void CWindow_MapTool::Basic_SelectFunction()
{
	_uint iObjectTagSize = (_uint)m_vecCreateObject.size();


	if (true == m_vecCreateObject.empty())
	{
		ImGui::Text(u8"생성한 객체가 없습니다. ");
	}
	else
	{
		if (ImGui::BeginListBox(u8"생성 객체 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
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

	Guizmo_Tick(m_pPickingObject);
}

void CWindow_MapTool::Instance_SelectFunction()
{
	if (true == m_vecCreateInstance.empty())
	{
		ImGui::Text(u8"인스턴스할 환경 객체를 생성하지 않았습니다.");
		return;
	}
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

	_int iCreateInstanceSize = (_int)m_vecCreateInstance.size();

	if (ImGui::BeginListBox(u8"환경 객체 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < (_uint)iCreateInstanceSize; ++i)
		{
			const _bool isSelected = (m_iSelectEnvironmentIndex == i);

			if (ImGui::Selectable(m_vecCreateInstanceTag[i].c_str(), isSelected))
			{
				m_iSelectEnvironmentIndex = i;

				m_vecInstanceInfoTag.clear();
				m_iInstanceInfoTagIndex = 0;

				vector<INSTANCE_INFO_DESC> Desc = *m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfoDesc();

				_int iInstanceInfoSize = (_int)Desc.size();

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

			_int iNumInstance = (_int)Desc.size();


			for (_uint i = 0; i < (_uint)iNumInstance; ++i)
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

		_float4x4 matTemp;
		XMStoreFloat4x4(&matTemp, matWorld);
		
		_float arrWorld[16] = {
		matTemp._11, matTemp._12, matTemp._13, matTemp._14,
		matTemp._21, matTemp._22, matTemp._23, matTemp._24,
		matTemp._31, matTemp._32, matTemp._33, matTemp._34,
		matTemp._41, matTemp._42, matTemp._43, matTemp._44
		};

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

		//pInstance->vTranslation = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
		//pInstance->vRotation = { matrixRotation[0], matrixRotation[1], matrixRotation[2] };
		//pInstance->vScale = { matrixScale[0], matrixScale[1], matrixScale[2] };

		

		
		
		
		ImGuizmo::Manipulate(m_arrView, m_arrProj, InstanceCurrentGizmoOperation, InstanceCurrentGizmoMode, arrWorld, NULL, InstanceuseSnap ? &snap[0] : NULL);
		
		_float m11 = arrWorld[0], m12 = arrWorld[1], m13 = arrWorld[2], m21 = arrWorld[4], m22 = arrWorld[5], m23 = arrWorld[6], m31 = arrWorld[8], m32 = arrWorld[9], m33 = arrWorld[10], m41 = arrWorld[12], m42 = arrWorld[13], m43 = arrWorld[14];


		//!_float arrWorld[16] = {
		//![0] matTemp._11, [1] matTemp._12, [2] matTemp._13, [3]matTemp._14,
		//![4] matTemp._21, [5] matTemp._22, [6] matTemp._23, [7] matTemp._24,
		//![8] matTemp._31, [9] matTemp._32, [10] matTemp._33, [11] matTemp._34,
		//![12] matTemp._41, [13] matTemp._42, [14] matTemp._43, [15] matTemp._44
		//!};
		//! 
		//! _float3 Get_Rotated() //! 승용 추가 수정할거면 물어봐주셈
		//!{
		//!	_float3 vRotation;
		//!
		//!	_float4x4 WorldMatrix = Get_WorldFloat4x4();
		//!	_matrix rotationMatrix = XMMatrixIdentity();
		//!
		//!	vRotation.x = asin(WorldMatrix._32); // 피치
		//!	vRotation.y = atan2(-WorldMatrix._31, WorldMatrix._33); // 요
		//!	vRotation.z = atan2(-WorldMatrix._12, WorldMatrix._22); // 롤
		//!
		//!	return vRotation;
		//!}
		
		
		_float3 vRotation;
		vRotation.x = asinf(-m32);

		if (cosf(vRotation.x) > 0.0001f) 
		{
			vRotation.y = atan2f(m31, m33); 
			vRotation.z = atan2f(m12, m22); 
		}
		else
		{
			vRotation.y = 0.0f; 
			vRotation.z = atan2f(-m21, m11); 
		}
		vRotation.y = atan2(-m31, m33);
		vRotation.z = atan2(-m12, m22);
		
		pInstance->vTranslation = { arrWorld[12], arrWorld[13], arrWorld[14] };
		pInstance->vRotation = { vRotation.x, vRotation.y, vRotation.z };
		pInstance->vScale = { m11, m22, m33 };
		//pInstance->Bake_CenterWithMatrix();
		m_vecCreateInstance[iIndex]->Update(*pInstance, m_iSelectInstanceIndex);

		if (ImGuizmo::IsOver())
		{
			int a = 0;
		}
}

void CWindow_MapTool::Character_SelectFunction()
{
	switch (m_eTabType)
	{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER:
		{
			Monster_SelectFunction();
			break;
		}
		case Client::CWindow_MapTool::TAP_TYPE::TAB_BOSSMONSTER:
		{
			Boss_SelectFunction();
			break;
		}
		case Client::CWindow_MapTool::TAP_TYPE::TAB_NPC:
		{
			NPC_SelectFunction();
			break;
		}

		default:
			break;
	}
	
}

void CWindow_MapTool::Monster_SelectFunction()
{
	_uint iCreateMonsterTagSize = (_uint)m_vecCreateMonsterTag.size();

	if (true == m_vecCreateMonster.empty())
	{
		ImGui::Text(u8"생성한 몬스터가 없습니다. ");
	}
	else
	{
		if (ImGui::BeginListBox(u8"몬스터 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iCreateMonsterTagSize; ++i)
			{
				const _bool isSelected = (m_iSelectCharacterTag == i);

				if (ImGui::Selectable(m_vecCreateMonsterTag[i].c_str(), isSelected))
				{
					m_iSelectCharacterTag = i;

					m_pPickingObject = m_vecCreateMonster[m_iSelectCharacterTag];
					
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

void CWindow_MapTool::Boss_SelectFunction()
{
}

void CWindow_MapTool::NPC_SelectFunction()
{
	//TODO 추후 NPC 추가 되면 작성

		//vector<CNPC*>					m_vecCreateNPC = { nullptr };
		//vector<string>						m_vecCreateNPCTag = {};

	//!_uint iCreateNpcTagSize = (_uint)m_vecCreateNPC.size();
	//!
	//!if (true == m_vecCreateNPC.empty())
	//!{
	//!	ImGui::Text(u8"생성한 NPC가 없습니다. ");
	//!}
	//!else
	//!{
	//!	if (ImGui::BeginListBox(u8"NPC 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	//!	{
	//!		for (_uint i = 0; i < iCreateNpcTagSize; ++i)
	//!		{
	//!			const _bool isSelected = (m_iSelectCharacterTag == i);
	//!
	//!			if (ImGui::Selectable(m_vecCreateNPCTag[i].c_str(), isSelected))
	//!			{
	//!				m_iSelectCharacterTag = i;
	//!
	//!				m_pPickingObject = m_vecCreateNPC[m_iSelectCharacterTag];
	//!
	//!				if (isSelected)
	//!				{
	//!					ImGui::SetItemDefaultFocus();
	//!				}
	//!			}
	//!		}
	//!		ImGui::EndListBox();
	//!	}
	//!}
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

	if(m_pPlayer != nullptr)
		Safe_Release(m_pPlayer);

	if (m_pPreviewCharacter != nullptr)
		Safe_Release(m_pPreviewCharacter);

	if(m_pPreviewObject != nullptr)
		Safe_Release(m_pPreviewObject);
		
}