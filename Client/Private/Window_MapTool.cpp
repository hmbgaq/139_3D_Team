#include "stdafx.h"
#include "Window_MapTool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

#include "Environment_Object.h"
#include "Environment_Interact.h"
#include "Environment_Instance.h"
#include "Environment_LightObject.h"
#include "Light.h"
#include "Effect.h"

#include "Field.h"

#include "LandObject.h"
#include "Player.h"
#include "Monster_Character.h"

#include "../Imgui/ImGuizmo/ImGuizmo.h"
#include "../Imgui/ImGuizmo/ImCurveEdit.h"
#include "../Imgui/ImGuizmo/GraphEditor.h"
#include "../Imgui/ImGuizmo/ImSequencer.h"
#include "../Imgui/ImGuizmo/ImZoomSlider.h"
#include "Camera_Dynamic.h"
#include "SpringCamera.h"
#include "Camera.h"
#include "Sky.h"
#include "Data_Manager.h"
#include "MasterCamera.h"

#include "Navigation.h"
#include "Cell.h"
#include "../../Reference/Public/Delaunator/delaunator.hpp"
#include "DebugDraw.h"


static ImGuizmo::OPERATION InstanceCurrentGizmoOperation;
static ImGuizmo::MODE	   InstanceCurrentGizmoMode;
static bool InstanceuseSnap(false);
static bool InstanceuseSnapUI(false);


CWindow_MapTool::CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
	
}


HRESULT CWindow_MapTool::Initialize()
{
	FAILED_CHECK(__super::Initialize());

	FAILED_CHECK(Ready_ModelTags());
	FAILED_CHECK(Ready_PrototypeTags());
	

	_int iEnvironModelTagSize = (_int)m_vecEnviroModelTag.size();
	for (int i = 0; i < iEnvironModelTagSize; ++i) //! 인스턴싱용 깡통맵
	{
		vector<CEnvironment_Object*> EmptyVector;

		m_mapPreviewInstance.emplace(m_vecEnviroModelTag[i], EmptyVector);
	}
	
	XMStoreFloat4x4(&m_matInstanceMatrix, XMMatrixIdentity());
	
	m_pToolCamera = CData_Manager::GetInstance()->Get_MasterCamera();

	if(m_pToolCamera == nullptr)
		return E_FAIL;

	m_pSkybox = CData_Manager::GetInstance()->Get_pSkyBox();

	if(m_pSkybox == nullptr)
		return E_FAIL;
	//m_mapPreviewInstance

	m_pNavigation = CData_Manager::GetInstance()->Get_Navigation();

	if(m_pNavigation == nullptr)
		return E_FAIL;

	/* Navi */
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t   iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	FAILED_CHECK(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayOut));


	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	CLight* pLight = m_pGameInstance->Add_Light_AndGet(LightDesc, LightDesc.iLightIndex);

	if (pLight == nullptr)
		return E_FAIL;

	m_vecCreateLight.push_back(pLight);
	m_vecCreateLightTag.push_back("Light" + pLight->Get_LightIndex());
	
	
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
		
		if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG; m_strDialogPath = "../Bin/DafaFiles/Data_Map/"; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DafaFiles/Data_Map/";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
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
		const char* CharObjectType[3] = { u8"환경", u8"캐릭터", u8"네비게이션"};

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
//		ObjectMode_Change_For_Reset();
	}
	ImGui::Separator();

	
	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		EnvironmentMode_Function();
	}
	else if(m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER) //! OBJECTMODE_CHARACTER
	{
		CharacterMode_Function();
	}
	else
	{
		NavigationMode_Function();
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

	if(m_bCreateCamera == false)
		IsCreatePlayer_ReadyCamara();
	

	
}

void CWindow_MapTool::Render()
{

}


HRESULT CWindow_MapTool::Save_Function(string strPath, string strFileName)
{
 	if(m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_NAVIGATION)
	{
		SaveNavi(strPath + "/" + strFileName);
	}
	else
	{
		string strNoExtFileName = filesystem::path(strFileName).stem().string();


		string strBasic = "Basic";
		string strInstance = "Instance";
		string strInteract = "Interact";

		for (auto& tag : m_vecCreateObjectTag)
		{
			// 문자열에서 '@' 문자 이후의 부분을 지움
			size_t atIndex = tag.find('@');
			if (atIndex != std::string::npos) {
				tag.erase(atIndex); // '@' 이후의 문자열을 모두 제거
			}
		}

		for (auto& tag : m_vecCreateInteractObjectTag)
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

		if (false == m_vecCreateInteractObject.empty())
		{
			_int iCreateInteractObjectSize = (_int)m_vecCreateInteractObject.size();

			for (_int i = 0; i < iCreateInteractObjectSize; ++i)
			{
				CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC Desc = {};

				Desc = *m_vecCreateInteractObject[i]->Get_EnvironmentDesc();
				
				string strModelTag;
				m_pGameInstance->WString_To_String(m_vecCreateInteractObject[i]->Get_ModelTag(), strModelTag);

				InteractJson[i].emplace("Type", strInteract);
				InteractJson[i].emplace("Index", i);
				InteractJson[i].emplace("ObjectTag", m_vecCreateInteractObjectTag[i]);
				InteractJson[i].emplace("LayerTag", L"Layer_BackGround");
				InteractJson[i].emplace("ModelTag", strModelTag);
				InteractJson[i].emplace("AnimType", Desc.bAnimModel);
				InteractJson[i].emplace("ShaderPassIndex", Desc.iShaderPassIndex);
				InteractJson[i].emplace("PlayAnimationIndex", Desc.iPlayAnimationIndex);
				InteractJson[i].emplace("InteractState", Desc.eInteractState);
				InteractJson[i].emplace("InteractType", Desc.eInteractType);
				InteractJson[i].emplace("LevelChange", Desc.bLevelChange);
				InteractJson[i].emplace("InteractLevel", Desc.eChangeLevel);
				

				CJson_Utility::Write_Float3(InteractJson[i]["ColliderSize"], Desc.vColliderSize);
				CJson_Utility::Write_Float3(InteractJson[i]["ColliderCenter"], Desc.vColliderCenter);
				m_vecCreateInteractObject[i]->Write_Json(InteractJson[i]);
			}

		}


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
					CJson_Utility::Write_Float4(InstanceInfoJson[j]["Instance_Rotation"], XMLoadFloat4(&InstanceInfoDesc.vRotation));
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
				CMonster_Character::MONSTER_DESC Desc;

				Desc = *m_vecCreateMonster[i]->Get_MonsterDesc();
				Desc.eDescType = CGameObject::MONSTER_DESC;

				string strProtoTag;

				m_pGameInstance->WString_To_String(Desc.strProtoTypeTag, strProtoTag);

				 /*= m_pGameInstance->Wstring_To_UTF8(Desc.strProtoTypeTag);*/
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

		

		json LightJson;

		if (false == m_vecCreateLight.empty())
		{
			_int iCreateLightSize = (_int)m_vecCreateLight.size();

			for (_int i = 0; i < iCreateLightSize; ++i)
			{
				LIGHT_DESC LightDesc = m_vecCreateLight[i]->Get_LightDesc();
				
				LightJson[i].emplace("LightIndex", LightDesc.iLightIndex);
				LightJson[i].emplace("LightEnable", LightDesc.bEnable);
				LightJson[i].emplace("CutOff", LightDesc.fCutOff);
				LightJson[i].emplace("OuterCutOff", LightDesc.fOuterCutOff);
				
				LightJson[i].emplace("Type", LightDesc.eType);
				CJson_Utility::Write_Float4(LightJson[i]["Direction"], LightDesc.vDirection);
				LightJson[i].emplace("Range", LightDesc.fRange);
				CJson_Utility::Write_Float4(LightJson[i]["Position"], LightDesc.vPosition);
				CJson_Utility::Write_Float4(LightJson[i]["Diffuse"], LightDesc.vDiffuse);
				CJson_Utility::Write_Float4(LightJson[i]["Ambient"], LightDesc.vAmbient);
				CJson_Utility::Write_Float4(LightJson[i]["Specular"], LightDesc.vSpecular);
			}
		}

		json LightObjectJson;

		if (false == m_vecCreateLightObject.empty())
		{
			_int iCreateLightObjectSize = (_int)m_vecCreateLightObject.size();

			for (_int i = 0; i < iCreateLightObjectSize; ++i)
			{
				CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc = *m_vecCreateLightObject[i]->Get_EnvironmentDesc();

				{
					LightObjectJson[i].emplace("AnimType", LightObjectDesc.bAnimModel);
					LightObjectJson[i].emplace("Effect", LightObjectDesc.bEffect);
					LightObjectJson[i].emplace("EffectType", LightObjectDesc.eLightEffect);
					LightObjectJson[i].emplace("PlayAnimationIndex", LightObjectDesc.iPlayAnimationIndex);
					LightObjectJson[i].emplace("ShaderPassIndex", LightObjectDesc.iShaderPassIndex);

					string strModelTag;
					m_pGameInstance->WString_To_String(m_vecCreateLightObject[i]->Get_ModelTag(), strModelTag);
					LightObjectJson[i].emplace("ModelTag", strModelTag);

					m_vecCreateLightObject[i]->Write_Json(LightObjectJson[i]);
				}
				
				{
					LIGHT_DESC LightDesc = LightObjectDesc.LightDesc;

					LightObjectJson[i].emplace("LightIndex", LightDesc.iLightIndex);
					LightObjectJson[i].emplace("LightEnable", LightDesc.bEnable);
					LightObjectJson[i].emplace("CutOff", LightDesc.fCutOff);
					LightObjectJson[i].emplace("OuterCutOff", LightDesc.fOuterCutOff);

					LightObjectJson[i].emplace("LightType", LightDesc.eType);
					CJson_Utility::Write_Float4(LightObjectJson[i]["Direction"], LightDesc.vDirection);
					LightObjectJson[i].emplace("Range", LightDesc.fRange);
					CJson_Utility::Write_Float4(LightObjectJson[i]["Position"], LightDesc.vPosition);
					CJson_Utility::Write_Float4(LightObjectJson[i]["Diffuse"], LightDesc.vDiffuse);
					CJson_Utility::Write_Float4(LightObjectJson[i]["Ambient"], LightDesc.vAmbient);
					CJson_Utility::Write_Float4(LightObjectJson[i]["Specular"], LightDesc.vSpecular);
				}
			}
		}


		SaveJson.emplace("Basic_Json", BasicJson);
		SaveJson.emplace("Interact_Json", InteractJson);
		SaveJson.emplace("Instance_Json", InstanceJson);
		SaveJson.emplace("Monster_Json", MonsterJson);
		SaveJson.emplace("Light_Json", LightJson);
		SaveJson.emplace("LightObject_Json", LightObjectJson);


		string strSavePath = strPath + "/" + strNoExtFileName + "_MapData.json";
		if (FAILED(CJson_Utility::Save_Json(strSavePath.c_str(), SaveJson)))
		{
			MSG_BOX("맵툴 저장 실패");
		}
		else
		{
			MSG_BOX("맵툴 저장 성공");
		}
	}
 	

			return S_OK;

}

HRESULT CWindow_MapTool::Load_Function(string strPath, string strFileName)
{
	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_NAVIGATION)
	{
		LoadNavi(strPath + "/" + strFileName);
	}
	else
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

			CEnvironment_Object* pObject = { nullptr };

			pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

			m_vecCreateObject.push_back(pObject);
			m_iCreateObjectIndex++;
		}


		json InteractJson = LoadJson["Interact_Json"];
		_int InteractJsonSize = (_int)InteractJson.size();

		for (_int i = 0; i < InteractJsonSize; ++i)
		{
			string IndexTag = "@" + to_string(i);

			string pushObjectTag = string(InteractJson[i]["ObjectTag"]) + IndexTag;

			m_vecCreateInteractObjectTag.push_back(pushObjectTag);

			CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC Desc = {};

			Desc.bAnimModel = InteractJson[i]["AnimType"];

			wstring strLoadModelTag;
			string strJsonModelTag = InteractJson[i]["ModelTag"];
			m_pGameInstance->String_To_WString(strJsonModelTag, strLoadModelTag);
			Desc.strModelTag = strLoadModelTag;
			Desc.bPreview = false;
			Desc.iPlayAnimationIndex = InteractJson[i]["PlayAnimationIndex"];
			Desc.iShaderPassIndex = InteractJson[i]["ShaderPassIndex"];
			Desc.eInteractState = InteractJson[i]["InteractState"];
			Desc.eInteractType = InteractJson[i]["InteractType"];
			Desc.bLevelChange = InteractJson[i]["LevelChange"];
			//Desc.bLevelChange = false;
			Desc.eChangeLevel = (LEVEL)InteractJson[i]["InteractLevel"];

			CJson_Utility::Load_Float3(InteractJson[i]["ColliderSize"], Desc.vColliderSize);
			CJson_Utility::Load_Float3(InteractJson[i]["ColliderCenter"], Desc.vColliderCenter);
				
			const json& TransformJson = InteractJson[i]["Component"]["Transform"];
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

			CEnvironment_Interact* pObject = { nullptr };

			pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

			m_vecCreateInteractObject.push_back(pObject);
			m_vecCreateInteractIndex++;
		}

		json InstanceJson = LoadJson["Instance_Json"];
		_int InstanceJsonSize = (_int)InstanceJson.size();

		for (_int i = 0; i < InstanceJsonSize; ++i)
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
				CJson_Utility::Load_Float4(InstanceInfoJson[j]["Instance_Rotation"], InstanceInfoDesc.vRotation);
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
			string IndexTag = "@" + to_string(i);
			
			string pushMonsterTag = (string)MonsterJson[i]["PrototypeTag"] + IndexTag;
			m_vecCreateMonsterTag.push_back(pushMonsterTag);

			CMonster_Character::MONSTER_DESC MonsterDesc;
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

			CMonster_Character* pMonster = { nullptr };

			wstring strProtoTypeTag;
			m_pGameInstance->String_To_WString((string)MonsterJson[i]["PrototypeTag"], strProtoTypeTag);
			MonsterDesc.strProtoTypeTag = strProtoTypeTag;

			pMonster = dynamic_cast<CMonster_Character*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strProtoTypeTag, &MonsterDesc));

			m_vecCreateMonster.push_back(pMonster);
			m_iCreateMonsterIndex++;
		}


		json LightJson = LoadJson["Light_Json"];
		_int iLightJsonSize = (_int)LightJson.size();

		for (_int i = 0; i < iLightJsonSize; ++i)
		{
			

			LIGHT_DESC LightDesc = {};

			LightDesc.iLightIndex = LightJson[i]["LightIndex"];
			LightDesc.bEnable = LightJson[i]["LightEnable"];
			LightDesc.fCutOff = LightJson[i]["CutOff"];
			LightDesc.fOuterCutOff = LightJson[i]["OuterCutOff"];
			
			LightDesc.eType = LightJson[i]["Type"];
			CJson_Utility::Load_Float4(LightJson[i]["Direction"], LightDesc.vDirection);
			LightDesc.fRange = LightJson[i]["Range"];
			CJson_Utility::Load_Float4(LightJson[i]["Position"], LightDesc.vPosition);
			CJson_Utility::Load_Float4(LightJson[i]["Diffuse"], LightDesc.vDiffuse);
			CJson_Utility::Load_Float4(LightJson[i]["Specular"], LightDesc.vSpecular);
			CJson_Utility::Load_Float4(LightJson[i]["Ambient"], LightDesc.vAmbient);
			

			if (LightDesc.eType == tagLightDesc::TYPE_DIRECTIONAL)
			{
				CLight* pDirectionLight = m_pGameInstance->Get_DirectionLight();

				if (pDirectionLight != nullptr)
				{
					m_pGameInstance->Remove_Light(pDirectionLight->Get_LightIndex());
					
				}
			}

			CLight* pLight = m_pGameInstance->Add_Light_AndGet(LightDesc, LightDesc.iLightIndex);

			if (pLight == nullptr)
			{
				MSG_BOX("라이트 불러오기 실패");
			}
			else
			{
				m_vecCreateLight.push_back(pLight);

				string strLightTag = "Light" + to_string(pLight->Get_LightIndex());
				m_vecCreateLightTag.push_back(strLightTag);
			}
		}

		json LightObjectJson = LoadJson["LightObject_Json"];
		_int iLightObjectJsonSize = (_int)LightObjectJson.size();

		for (_int i = 0; i < iLightObjectJsonSize; ++i)
		{
			CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc = {};

			LightObjectDesc.bAnimModel = LightObjectJson[i]["AnimType"];
			LightObjectDesc.bEffect = LightObjectJson[i]["Effect"];
			LightObjectDesc.eLightEffect = LightObjectJson[i]["EffectType"];
			LightObjectDesc.iPlayAnimationIndex = LightObjectJson[i]["PlayAnimationIndex"];
			LightObjectDesc.iShaderPassIndex = LightObjectJson[i]["ShaderPassIndex"];
			LightObjectDesc.bPreview = false;
			
			m_pGameInstance->String_To_WString((string)LightObjectJson[i]["ModelTag"], LightObjectDesc.strModelTag);
				
			const json& TransformJson = LightObjectJson[i]["Component"]["Transform"];
			_float4x4 WorldMatrix;

			for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
			{
				for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
				{
					WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
				}
			}

			LightObjectDesc.WorldMatrix = WorldMatrix;



			LIGHT_DESC LightDesc = {};

			LightDesc.iLightIndex = LightObjectJson[i]["LightIndex"];
			LightDesc.bEnable = LightObjectJson[i]["LightEnable"];
			LightDesc.fCutOff = LightObjectJson[i]["CutOff"];
			LightDesc.fOuterCutOff = LightObjectJson[i]["OuterCutOff"];

			LightDesc.eType = LightObjectJson[i]["LightType"];
			CJson_Utility::Load_Float4(LightObjectJson[i]["Direction"], LightDesc.vDirection);
			LightDesc.fRange = LightObjectJson[i]["Range"];
			CJson_Utility::Load_Float4(LightObjectJson[i]["Position"], LightDesc.vPosition);
			CJson_Utility::Load_Float4(LightObjectJson[i]["Diffuse"], LightDesc.vDiffuse);
			CJson_Utility::Load_Float4(LightObjectJson[i]["Ambient"], LightDesc.vAmbient);
			CJson_Utility::Load_Float4(LightObjectJson[i]["Specular"], LightDesc.vSpecular);

			
			LightObjectDesc.LightDesc = LightDesc;

			CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));

			if (pLightObject == nullptr)
			{
				MSG_BOX("라이트오브젝트 생성실패");
			}

			m_vecCreateLightObject.push_back(pLightObject);

			wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pLightObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateLightObjectIndex));
			string strConvertTag;
			m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
			m_vecCreateLightObjectTag.push_back(strConvertTag);

			m_iCreateLightObjectIndex++;
			
		}
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

	if(m_pPreviewInteract != nullptr)
		m_pPreviewInteract->Set_Dead(true);

	if(m_pPreviewLightObject != nullptr)
		m_pPreviewLightObject->Set_Dead(true);

	m_pPreviewObject = nullptr;
	m_pPreviewCharacter = nullptr;
	m_pPreviewInteract = nullptr;
	m_pPickingInstanceInfo = nullptr;
	m_pPreviewLightObject = nullptr;

		

	_int iCreateObjectSize = (_int)m_vecCreateObject.size();

	for (_int i = 0; i < iCreateObjectSize; ++i)
	{
		m_vecCreateObject[i]->Set_Dead(true);
	}

	m_iSelectObjectIndex = 0;
	m_iCreateObjectIndex = 0;
	m_vecCreateObject.clear();
	m_vecCreateObjectTag.clear();


	_int iCreateInteractSize = (_int)m_vecCreateInteractObject.size();

	for (_int i = 0; i < iCreateInteractSize; ++i)
	{
		m_vecCreateInteractObject[i]->Set_Dead(true);
	}

	m_iSelectObjectIndex = 0;
	m_vecCreateInteractIndex = 0;
	m_vecCreateInteractObject.clear();
	m_vecCreateInteractObjectTag.clear();


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
	m_iSelectPreviewIndex = 0;
	m_iCreatePreviewIndex = 0;

	


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

	

	_int iCreateLightObjectSize = (_int)m_vecCreateLightObject.size();

	for (_int i = 0; i < iCreateLightObjectSize; ++i)
	{
		m_vecCreateLightObject[i]->Set_Dead(false);
	}

	m_iCreateLightObjectIndex = 0;
	m_iSelectLightObjectIndex = 0;
	m_vecCreateLightObject.clear();
	m_vecCreateLightObjectTag.clear();

	_int iCreateLightSize = (_int)m_vecCreateLight.size();

	for (_int i = 0; i < iCreateLightSize; ++i)
	{
		if (false == m_pGameInstance->Remove_Light(m_vecCreateLight[i]->Get_LightIndex()))
		{
			MSG_BOX("라이트 삭제실패");
			return;
		}

	}

	m_iSelectLightIndex = 0;
	m_vecCreateLight.clear();
	m_vecCreateLightTag.clear();
	
	

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
			case MAP_KEY_TYPE::MODEL_SINGLE:
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
			case MAP_KEY_TYPE::MODEL_SINGLE:
			{
				m_vecSingleModelTag.push_back(strNonAnimTag);
				m_vecEnviroModelTag.push_back(strNonAnimTag);
				break;
			}

			case MAP_KEY_TYPE::MODEL_INSTANCE:
			{
				m_vecEnviroModelTag.push_back(strNonAnimTag);
				m_vecSingleModelTag.push_back(strNonAnimTag);
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
	m_vecMonsterTag.push_back("Prototype_GameObject_Infected_A");
	m_vecMonsterTag.push_back("Prototype_GameObject_Infected_B");
	m_vecMonsterTag.push_back("Prototype_GameObject_Infected_C");
	//m_vecMonsterTag.push_back("Prototype_GameObject_Assassin");
	m_vecMonsterTag.push_back("Prototype_GameObject_Bandit_Heavy");
	m_vecMonsterTag.push_back("Prototype_GameObject_Bandit_Sniper");
	//m_vecMonsterTag.push_back("Prototype_GameObject_Screamer");

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

	if (ImGui::Button(u8"스테이지1 불러오기"))
	{
		string strFilePath = "..\\Bin\\DataFiles\\Data_Map";
		string strFileName = "Stage1Final_MonsterInclude_Decrease.json";
		Load_Function(strFilePath, strFileName);
		LoadNavi("..\\Bin\\DataFiles\\Navigation\\Stage1NavigationFinal.dat");
	}

	if (ImGui::Button(u8"인트로보스맵 불러오기"))
	{
		string strFilePath = "..\\Bin\\DataFiles\\Data_Map";
		string strFileName = "Stage1Boss_MapData_MapData.json";
		Load_Function(strFilePath, strFileName);
		LoadNavi("..\\Bin\\DataFiles\\Navigation\\IntroBossNaviFinal.dat");
	}

	ImGui::NewLine();
	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;

	if (ImGui::BeginTabBar(u8"환경 오브젝트 타입", tab_bar_flags))
	{

		if (ImGui::BeginTabItem(u8"단일 환경"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_SINGLE)
			{
				m_iSelectObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_SINGLE;
			GroundTab_Function(); 

			ImGui::EndTabItem();
		}

		if(ImGui::BeginTabItem(u8"라이트"))
		{
			if(m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_LIGHT)
			{
				m_iSelectLightObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_LIGHT;
			LightTab_Function();

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

	
		if (ImGui::BeginTabItem(u8"인스턴스 환경"))
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

void CWindow_MapTool::NavigationMode_Function()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;

	ImGui::SeparatorText(u8"네비게이션 세이브 / 로드");
	{
		if (m_bHaveNaviSave == true)
		{
			if (ImGui::Button(u8"저장하기")) { SaveNavi(m_strNaviFinalSavePath + to_string(m_iSaveNaviIndex));}

			ImGui::SameLine();

			if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DafaFiles/Data_Map/Navigation";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
		}
		else
		{
			if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG;  m_strDialogPath = "../Bin/DafaFiles/Data_Map/Navigation"; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DafaFiles/Data_Map/Navigation";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
		}

		if (m_bCreateCamera == true && m_pPlayer != nullptr)
		{
			if (ImGui::Button(u8"플레이어 네비게이션"))
			{
				m_pPlayer->Set_Navigation(m_pNavigation);
			}

			ImGui::SameLine();

			static _bool bPlayerMove = false;

			ImGui::Checkbox(u8"플레이어 이동", &bPlayerMove);

			if(true == bPlayerMove)
			{
				Guizmo_Tick(m_pPlayer);
			}
		}

	}ImGui::Separator();

	if (ImGui::BeginTabBar(u8"네비게이션 모드 타입", tab_bar_flags))
	{

		if (ImGui::BeginTabItem(u8"네비게이션 생성"))
		{
			 Navigation_CreateTab();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"네비게이션 선택"))
		{
			Navigation_SelectTab();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"네비게이션 삭제"))
		{
			Navigation_DeleteTab();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		//m_pNavigation->Update(XMMatrixIdentity());
		m_pGameInstance->Add_DebugRender(m_pNavigation);

		
		

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
				Create_Tab(CWindow_MapTool::TAP_TYPE::TAB_SINGLE);
				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
			{
				Basic_SelectFunction();
				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
			{
				Delete_Tab(CWindow_MapTool::TAP_TYPE::TAB_SINGLE);

				break;
			}
	}
	
}

void CWindow_MapTool::LightTab_Function()
{
	Select_ModeType(); //! 생성, 선택, 삭제 선택, 마우스 프레싱, 다운, 업 선택

	m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_LIGHT;

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
		{
			Light_CreateTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Light_SelectTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{
			Light_DeleteTab();
			break;
		}
	}
}

void CWindow_MapTool::Light_CreateTab()
{
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	Select_PickingType();

	static _int iCreateLightType = 0;

	const char* CreateLightType[2] = { u8"라이트 설치", u8"라이트 오브젝트 설치" };


	for (_uint i = 0; i < IM_ARRAYSIZE(CreateLightType); ++i)
	{
		if(i > 0) { ImGui::SameLine(); }

		if (ImGui::RadioButton(CreateLightType[i], &iCreateLightType, i))
		{
			iCreateLightType = i;
			m_eLightCreateMode = (CWindow_MapTool::LIGHT_CREATEMODE)i;
		}
	}

		ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

		ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);

		ImGui::SeparatorText(u8"라이트 셋팅");
		{

			static _int iLightType = 0;

			const char* LightType[3] = { u8"디렉션 라이트", u8"포인트 라이트", u8"스포트 라이트" };

			for (_uint i = 0; i < IM_ARRAYSIZE(LightType); ++i)
			{
				if (i > 0) { ImGui::SameLine(); }

				if (ImGui::RadioButton(LightType[i], &iLightType, i))
				{
					m_eLightType = (LIGHT_DESC::TYPE)iLightType;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button(u8"라이트타입 변경"))
			{
				LIGHT_DESC::TYPE eLightType = LIGHT_DESC::TYPE::TYPE_END;

				if (iLightType == 0)
					eLightType = LIGHT_DESC::TYPE::TYPE_DIRECTIONAL;
				else if (iLightType == 1)
					eLightType = LIGHT_DESC::TYPE::TYPE_POINT;
				else if (iLightType == 2)
					eLightType = LIGHT_DESC::TYPE::TYPE_SPOTLIGHT;

				m_pPreviewLightObject->Change_LightType(eLightType);
			};

			if (iLightType == 0)
			{
				if (ImGui::InputFloat4(u8"라이트 디렉션", &m_tEditLightDesc.vDirection.x))
				{
					LIGHT_DESC OriginDesc = m_pPreviewLightObject->Get_LightDesc();
					OriginDesc.vDirection = m_tEditLightDesc.vDirection;

					m_pPreviewLightObject->Set_LightDesc(OriginDesc);
				}

			}
			else if (iLightType == 1)
			{
				ImGui::SameLine();

				if (ImGui::Button(u8"라이트포지션 셋"))
				{
					_float4 vObjectPos = m_pPreviewLightObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);

					m_pPreviewLightObject->Set_LightPos(_float3(vObjectPos.x, vObjectPos.y, vObjectPos.z));

					m_tEditLightDesc.vPosition = _float4(vObjectPos.x, vObjectPos.y, vObjectPos.z, 1.f);
				}

				if (ImGui::InputFloat(u8"라이트 레인지", &m_tEditLightDesc.fRange))
				{
					LIGHT_DESC OriginDesc = m_pPreviewLightObject->Get_LightDesc();
					OriginDesc.fRange = m_tEditLightDesc.fRange;

					m_pPreviewLightObject->Set_LightDesc(OriginDesc);

				}

				if (ImGui::InputFloat4(u8"라이트 포지션", &m_tEditLightDesc.vPosition.x))
				{
					m_pPreviewLightObject->Set_LightPos(_float3(m_tEditLightDesc.vPosition.x, m_tEditLightDesc.vPosition.y, m_tEditLightDesc.vPosition.z));
				}
			}

			ImGui::Checkbox(u8"활성화", &m_tEditLightDesc.bEnable);

			ImGui::NewLine();

			ImGui::InputFloat4(u8"라이트 디퓨즈", &m_tEditLightDesc.vDiffuse.x);
			ImGui::InputFloat4(u8"라이트 앰비언트", &m_tEditLightDesc.vAmbient.x);
			ImGui::InputFloat4(u8"라이트 스페큘러", &m_tEditLightDesc.vSpecular.x);

			ImGui::Checkbox(u8"이펙트부여", &m_bLightEffect);

			if (m_bLightEffect == true)
			{
				const char* EffectTypes[] = { "TORCH", "TEST1", "TEST2", "TEST3", "TEST4" };
				const char* LightEffectPreviewType = EffectTypes[m_eLightEffectType];

				static ImGuiComboFlags ComboFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall;

				if (ImGui::BeginCombo(u8"이펙트 타입", LightEffectPreviewType, ComboFlags))
				{
					for (int i = 0; i < IM_ARRAYSIZE(EffectTypes); ++i)
					{
						const bool is_Selected = (m_eLightEffectType == i);

						if (ImGui::Selectable(EffectTypes[i], is_Selected))
						{
							m_eLightEffectType = i;
						}

						if (true == is_Selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			ImGui::NewLine();
		}
		ImGui::Separator();

		if (ImGui::BeginListBox(u8"라이트 모델태그", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_int iModelTagSize = 0;
			vector<string> vecModelTag;

			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
			{
				iModelTagSize = (_int)m_vecSingleModelTag.size();
				vecModelTag = m_vecSingleModelTag;
			}
			else if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
			{
				iModelTagSize = (_int)m_vecAnimEnviroModelTag.size();
				vecModelTag = m_vecAnimEnviroModelTag;
			}

			for (_uint i = 0; i < (_uint)iModelTagSize; ++i)
			{
				const _bool isSelected = (m_iSelectModelTag == i);

				if (ImGui::Selectable(vecModelTag[i].c_str(), isSelected))
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

		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_Guizmo(m_pPickingObject);

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

	


void CWindow_MapTool::Light_SelectTab()
{
	if (m_pPreviewLightObject != nullptr)
	{
		m_pPreviewLightObject->Set_Dead(true);
		m_pPreviewLightObject = nullptr;
	}

	static _int iSelectLightType = 0;

	const char* SelectLightType[2] = { u8"라이트 선택", u8"라이트 오브젝트 선택" };


	for (_uint i = 0; i < IM_ARRAYSIZE(SelectLightType); ++i)
	{
		if (i > 0) { ImGui::SameLine(); }

		if (ImGui::RadioButton(SelectLightType[i], &iSelectLightType, i))
		{
			iSelectLightType = i;
		}
	}

	static _bool bSelectDiffuseColor = false;
	ImGui::Checkbox(u8"선택한 조명 색 다르게하기", &bSelectDiffuseColor);

	

	

	if (iSelectLightType == 0)
	{
		_uint iObjectTagSize = (_uint)m_vecCreateLightTag.size();

		if (true == m_vecCreateLight.empty())
		{
			ImGui::Text(u8"생성한 라이트 객체가 없습니다. ");
		}
		else
		{
			if (ImGui::BeginListBox(u8"생성 라이트 객체 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (_uint i = 0; i < iObjectTagSize; ++i)
				{
					const _bool isSelected = (m_iSelectLightIndex == i);

					if (ImGui::Selectable(m_vecCreateLightTag[i].c_str(), isSelected))
					{
						m_iSelectLightIndex = i;
						
						if(bSelectDiffuseColor == true)
							m_vecCreateLight[m_iSelectLightIndex]->Set_Select(true); //! 선택될시 디퓨즈 컬러를 보라색으로 셰이더에 바인딩한다.
						

						m_tEditLightDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();

						for (_uint j = 0; j < iObjectTagSize; ++j)
						{
							if (bSelectDiffuseColor == true)
							{
								if (j == i)
									continue;
							}
								

							m_vecCreateLight[j]->Set_Select(false);
							
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				ImGui::EndListBox();
			}

			ImGui::SeparatorText(u8"라이트 셋팅");
			{

				static _int iLightType = 0;

				const char* LightType[3] = { u8"디렉션 라이트", u8"포인트 라이트", u8"스포트 라이트" };

				for (_uint i = 0; i < IM_ARRAYSIZE(LightType); ++i)
				{
					if (i > 0) { ImGui::SameLine(); }

					if (ImGui::RadioButton(LightType[i], &iLightType, i))
					{
						iLightType = i;
					}
				}

				ImGui::SameLine();
				if (ImGui::Button(u8"라이트타입 변경"))
				{
					LIGHT_DESC::TYPE eLightType = LIGHT_DESC::TYPE::TYPE_END;

					if (iLightType == 0)
						eLightType = LIGHT_DESC::TYPE::TYPE_DIRECTIONAL;
					else if (iLightType == 1)
						eLightType = LIGHT_DESC::TYPE::TYPE_POINT;
					else if (iLightType == 2)
						eLightType = LIGHT_DESC::TYPE::TYPE_SPOTLIGHT;

					m_vecCreateLight[m_iSelectLightIndex]->Set_LightType(eLightType);
				};

				if (iLightType == 0)
				{
					if (ImGui::InputFloat4(u8"라이트 디렉션", &m_tEditLightDesc.vDirection.x))
					{
						LIGHT_DESC OriginDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();
						OriginDesc.vDirection = m_tEditLightDesc.vDirection;

						m_vecCreateLight[m_iSelectLightIndex]->Set_LightDesc(OriginDesc);
					}

				}
				else if (iLightType == 1)
				{
					if (ImGui::InputFloat(u8"라이트 레인지", &m_tEditLightDesc.fRange))
					{
						LIGHT_DESC OriginDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();
						OriginDesc.fRange = m_tEditLightDesc.fRange;

						m_vecCreateLight[m_iSelectLightIndex]->Set_LightDesc(OriginDesc);

					}

					if (ImGui::InputFloat3(u8"라이트 포지션", &m_tEditLightDesc.vPosition.x))
					{
						LIGHT_DESC OriginDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();
						OriginDesc.vPosition = m_tEditLightDesc.vPosition;

						m_vecCreateLight[m_iSelectLightIndex]->Set_LightDesc(OriginDesc);

					}

					ImGui::SameLine();

					static _bool bLightMoveMode = false;

					ImGui::Checkbox(u8"라이트 이동모드, INSERT키 누를시 체크", &bLightMoveMode);

					if (m_pGameInstance->Key_Down(DIK_INSERT))
					{
						bLightMoveMode = !bLightMoveMode;
					}

					if (bLightMoveMode == true)
					{
						LIGHT_DESC OriginDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();

						if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
						{
							OriginDesc.vPosition = _float4{m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f};
						}
						else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
						{
							OriginDesc.vPosition = _float4(m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f );
						}

						m_vecCreateLight[m_iSelectLightIndex]->Set_LightDesc(OriginDesc);
						m_tEditLightDesc.vPosition = OriginDesc.vPosition;
					}
				}



				if (ImGui::Checkbox(u8"활성화", &m_tEditLightDesc.bEnable))
				{
					m_vecCreateLight[m_iSelectLightIndex]->Set_LightEnable(m_tEditLightDesc.bEnable);
				}


				ImGui::NewLine();
				
				if (ImGui::ColorEdit4(u8"라이트 디퓨즈", &m_tEditLightDesc.vDiffuse.x))
				{
					m_vecCreateLight[m_iSelectLightIndex]->Set_Diffuse(m_tEditLightDesc.vDiffuse);
				}

				if (ImGui::ColorEdit4(u8"라이트 앰비언트", &m_tEditLightDesc.vAmbient.x))
				{
					m_vecCreateLight[m_iSelectLightIndex]->Set_Ambient(m_tEditLightDesc.vAmbient);
				}

				if (ImGui::ColorEdit4(u8"라이트 스페큘러", &m_tEditLightDesc.vSpecular.x))
				{
					m_vecCreateLight[m_iSelectLightIndex]->Set_Specular(m_tEditLightDesc.vSpecular);
				}

				ImGui::NewLine();
			}
		}
	}

	else if (iSelectLightType == 1)
	{
		_uint iObjectTagSize = (_uint)m_vecCreateLightObjectTag.size();

		if (true == m_vecCreateLightObject.empty())
		{
			ImGui::Text(u8"생성한 라이트 오브젝트 객체가 없습니다. ");
		}
		else
		{
			if (ImGui::BeginListBox(u8"생성 라이트 오브젝트 객체 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
			
				for (_uint i = 0; i < iObjectTagSize; ++i)
				{
					const _bool isSelected = (m_iSelectLightObjectIndex == i);

					if (ImGui::Selectable(m_vecCreateLightObjectTag[i].c_str(), isSelected))
					{
						m_iSelectLightObjectIndex = i;

						m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_Select(true);
						m_tEditLightDesc = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_LightDesc();
						
						CEffect* pLightEffect = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_Effect();

						if (pLightEffect != nullptr)
						{
							m_vLightEffectPos = pLightEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
						}

						
						for (_uint j = 0; j < iObjectTagSize; ++j)
						{
							if(j == i)
								continue;

							m_vecCreateLightObject[j]->Set_Select(false);
						}

								
						m_pPickingObject = m_vecCreateLightObject[m_iSelectLightObjectIndex];

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndListBox();
			}


			if (ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex))
			{
#ifdef _DEBUG
				m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_ShaderPassindex(m_iShaderPassIndex);
#endif // _DEBUG			
			}



			ImGui::SeparatorText(u8"라이트 셋팅");
			{

				static _int iLightType = 0;

				const char* LightType[3] = { u8"디렉션 라이트", u8"포인트 라이트", u8"스포트 라이트" };

				for (_uint i = 0; i < IM_ARRAYSIZE(LightType); ++i)
				{
					if (i > 0) { ImGui::SameLine(); }

					if (ImGui::RadioButton(LightType[i], &iLightType, i))
					{
						iLightType = i;
					}
				}

				ImGui::SameLine();
				if (ImGui::Button(u8"라이트타입 변경"))
				{
					LIGHT_DESC::TYPE eLightType = LIGHT_DESC::TYPE::TYPE_END;

					if (iLightType == 0)
						eLightType = LIGHT_DESC::TYPE::TYPE_DIRECTIONAL;
					else if (iLightType == 1)
						eLightType = LIGHT_DESC::TYPE::TYPE_POINT;
					else if (iLightType == 2)
						eLightType = LIGHT_DESC::TYPE::TYPE_SPOTLIGHT;

					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Change_LightType(eLightType);
				};


				if (iLightType == 0)
				{
					if (ImGui::InputFloat4(u8"라이트 디렉션", &m_tEditLightDesc.vDirection.x))
					{
						LIGHT_DESC OriginDesc = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_LightDesc();
						OriginDesc.vDirection = m_tEditLightDesc.vDirection;

						m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_LightDesc(OriginDesc);
					}

				}
				else if (iLightType == 1)
				{
					if (ImGui::InputFloat(u8"라이트 레인지", &m_tEditLightDesc.fRange))
					{
						LIGHT_DESC OriginDesc = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_LightDesc();
						OriginDesc.fRange = m_tEditLightDesc.fRange;

						m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_LightDesc(OriginDesc);

					}

					

					if (ImGui::InputFloat3(u8"라이트 포지션", &m_tEditLightDesc.vPosition.x))
					{
						LIGHT_DESC OriginDesc = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_LightDesc();
						OriginDesc.vPosition = m_tEditLightDesc.vPosition;

						m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_LightDesc(OriginDesc);
					}

					ImGui::SameLine();

					static _bool bLightMoveMode = false;

					ImGui::Checkbox(u8"라이트 이동모드, INSERT키 누를시 체크", &bLightMoveMode);

					if (m_pGameInstance->Key_Down(DIK_INSERT))
					{
						bLightMoveMode = !bLightMoveMode;
					}

					if (bLightMoveMode == true)
					{
						LIGHT_DESC OriginDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();

						if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
						{
							OriginDesc.vPosition = _float4{ m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f };
						}
						else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
						{
							OriginDesc.vPosition = _float4(m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f);
						}

						m_vecCreateLight[m_iSelectLightIndex]->Set_LightDesc(OriginDesc);
						m_tEditLightDesc.vPosition = OriginDesc.vPosition;
					}


					if (ImGui::Button(u8"라이트포지션 셋"))
					{
						_float4 vObjectPos = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_Transform()->Get_State(CTransform::STATE_POSITION);

						m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_LightPos(_float3(vObjectPos.x, vObjectPos.y, vObjectPos.z));


						m_tEditLightDesc.vPosition = _float4(vObjectPos.x, vObjectPos.y, vObjectPos.z, 1.f);

						
					}
				}



				if (ImGui::Checkbox(u8"활성화", &m_tEditLightDesc.bEnable))
				{
					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_Enable(m_tEditLightDesc.bEnable);
				}


				ImGui::NewLine();

				if (ImGui::ColorEdit4(u8"라이트 디퓨즈", &m_tEditLightDesc.vDiffuse.x))
				{
					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_Diffuse(m_tEditLightDesc.vDiffuse);
				}

				if (ImGui::ColorEdit4(u8"라이트 스페큘러", &m_tEditLightDesc.vSpecular.x))
				{
					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_Specular(m_tEditLightDesc.vSpecular);
				}

				if (ImGui::ColorEdit4(u8"라이트 앰비언트", &m_tEditLightDesc.vAmbient.x))
				{
					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_Ambient(m_tEditLightDesc.vAmbient);
				}

				if (ImGui::Checkbox(u8"이펙트부여", &m_bLightEffect))
				{
					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_EffectEnable(m_bLightEffect);
				}

				if (m_bLightEffect == true)
				{
					const char* EffectTypes[] = { "TORCH", "TEST1", "TEST2", "TEST3", "TEST4" };
					const char* LightEffectPreviewType = EffectTypes[m_eLightEffectType];

					static ImGuiComboFlags ComboFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall;

					if (ImGui::BeginCombo(u8"이펙트 타입", LightEffectPreviewType, ComboFlags))
					{
						for (int i = 0; i < IM_ARRAYSIZE(EffectTypes); ++i)
						{
							const bool is_Selected = (m_eLightEffectType == i);

							if (ImGui::Selectable(EffectTypes[i], is_Selected))
							{
								m_eLightEffectType = i;
								m_vecCreateLightObject[m_iSelectLightObjectIndex]->Change_LightEffect((CEnvironment_LightObject::LIGHT_EFFECT)m_eLightEffectType);
							}

							if (true == is_Selected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}


					if (ImGui::InputFloat3(u8"이펙트 위치이동", &m_vLightEffectPos.x))
					{
						CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc = *m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_EnvironmentDesc();
						CEffect* pLightEffect = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_Effect();

						if (LightObjectDesc.bEffect == true && nullptr != pLightEffect)
						{
							m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_EffectPos(m_vLightEffectPos);
						}
					}
				}

				ImGui::NewLine();
			}

		}


		Guizmo_Tick(m_pPickingObject);
	}


	
}

void CWindow_MapTool::Light_DeleteTab()
{
	if (m_pPreviewLightObject != nullptr)
	{
		m_pPreviewLightObject->Set_Dead(true);
		m_pPreviewLightObject = nullptr;
	}

	static _int iDeleteLightType = 0;

	const char* DeleteLightType[2] = { u8"라이트 삭제", u8"라이트 오브젝트 삭제" };


	for (_uint i = 0; i < IM_ARRAYSIZE(DeleteLightType); ++i)
	{
		if (i > 0) { ImGui::SameLine(); }

		if (ImGui::RadioButton(DeleteLightType[i], &iDeleteLightType, i))
		{
			iDeleteLightType = i;
		}
	}


	if (iDeleteLightType == 0)
	{
		_uint iCreateLightSize = (_uint)m_vecCreateLight.size();

		if(iCreateLightSize == 0)
			return;

		ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

		if (ImGui::BeginListBox(u8"생성한 라이트 객체", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iCreateLightSize; ++i)
			{
				const _bool isSelected = (m_iSelectLightIndex == i);

				if (ImGui::Selectable(m_vecCreateLightTag[i].c_str(), isSelected))
				{
					m_iSelectLightIndex = i;

					m_bChange = true;
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}
	}
	else
	{
		_uint iCreateLightObjectSize = (_uint)m_vecCreateLightObject.size();

		if (iCreateLightObjectSize == 0)
			return;

		ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

		if (ImGui::BeginListBox(u8"생성한 라이트 오브젝트 객체", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iCreateLightObjectSize; ++i)
			{
				const _bool isSelected = (m_iSelectLightObjectIndex == i);

				if (ImGui::Selectable(m_vecCreateLightObjectTag[i].c_str(), isSelected))
				{
					m_iSelectLightObjectIndex = i;

					m_bChange = true;
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}


		if (m_vecCreateLightObject.size() < m_iSelectLightObjectIndex)
			return;


		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_Guizmo(m_vecCreateLightObject[m_iSelectLightObjectIndex]);
	}


	if (ImGui::Button(u8"삭제"))
	{	
		if (iDeleteLightType == 0)
		{
			if (true == m_pGameInstance->Remove_Light(m_vecCreateLight[m_iSelectLightIndex]->Get_LightIndex()))
			{
				m_vecCreateLight[m_iSelectLightIndex] = nullptr;
				m_vecCreateLight.erase(m_vecCreateLight.begin() + m_iSelectLightIndex);
				m_vecCreateLightTag.erase(m_vecCreateLightTag.begin() + m_iSelectLightIndex);
				m_pPickingObject = nullptr;

				if(m_iSelectLightIndex > 0)
					m_iSelectLightIndex--;
			}
			else
			{
				MSG_BOX("라이트 삭제 실패");
			}
		}
		else
		{
			m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_Dead(true);
			m_vecCreateLightObject[m_iSelectLightObjectIndex] = nullptr;
			m_pPickingObject = nullptr;
			m_vecCreateLightObject.erase(m_vecCreateLightObject.begin() + m_iSelectLightObjectIndex);
			m_vecCreateLightObjectTag.erase(m_vecCreateLightObjectTag.begin() + m_iSelectLightObjectIndex);

			if (m_iSelectLightObjectIndex > 0)
				m_iSelectLightObjectIndex--;
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
			Interact_CreateTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Interact_SelectFunction();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{
			Interact_DeleteTab();
			break;
		}
	}
}

void CWindow_MapTool::Interact_CreateTab()
{
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	Select_PickingType();

	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

	ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);

	ImGui::SeparatorText(u8"상호작용 셋팅");
	{
		const char* InteractTypes[] = { "INTERACT_JUMP100", "INTERACT_JUMP200", "INTERACT_JUMP300", "INTERACT_VAULT100", "INTERACT_VAULT200" };
		const char* InteractPreviewType = InteractTypes[m_eInteractType];

		static ImGuiComboFlags ComboFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall;

		if (ImGui::BeginCombo(u8"상호작용 타입", InteractPreviewType, ComboFlags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(InteractTypes); ++i)
			{
				const bool is_Selected = (m_eInteractType == i);

				if (ImGui::Selectable(InteractTypes[i], is_Selected))
				{
					m_eInteractType = i;
				}

				if(true == is_Selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		CEnvironment_Interact::INTERACT_STATE eInteractState = CEnvironment_Interact::INTERACT_STATE::INTERACTSTATE_END;
		
		//static _int iInstanceState = 0;
		const char* InstanceState[2] = { u8"무한 상호작용", u8"한번만 수행" };

		for (_uint i = 0; i < IM_ARRAYSIZE(InstanceState); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			if (ImGui::RadioButton(InstanceState[i], &m_eInteractState, i))
			{
				m_eInteractState = i;
				//eInteractState = CEnvironment_Interact::INTERACT_STATE(iInstanceState);
			}
		}

		ImGui::NewLine();

		if (ImGui::InputFloat3(u8"콜라이더 사이즈", m_fColliderSizeArray))
		{

		}

		if (ImGui::InputFloat3(u8"콜라이더 센터", m_fColliderCenterArray))
		{

		}
			
		if (ImGui::InputInt(u8"애니메이션 인덱스", &m_iInteractPlayAnimIndex))
		{

		}


		ImGui::Checkbox(u8"레벨 체인지", &m_bInteractLevelChange);

		if (m_bInteractLevelChange == true)
		{
			ImGui::SeparatorText(u8"상호작용시 이동할 레벨");
			{
				//!LEVEL_INTRO_BOSS,
				//!	LEVEL_SNOWMOUNTAIN,
				const char* InteractLevels[] = { u8"인트로보스레벨", u8"설산레벨" };
				const char* InteractPreviewLevel = InteractLevels[m_eInteractLevel];

				static ImGuiComboFlags ComboLevelFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall;

				if (ImGui::BeginCombo(u8"이동할 레벨", InteractPreviewLevel, ComboLevelFlags))
				{
					for (int i = 0; i < IM_ARRAYSIZE(InteractLevels); ++i)
					{
						const bool is_Selected = (m_eInteractLevel == i);

						if (ImGui::Selectable(InteractLevels[i], is_Selected))
						{
							m_eInteractLevel = i;
						}

						if (true == is_Selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}
		}



	}
	ImGui::Separator();

	if (ImGui::BeginListBox(u8"상호작용 모델태그", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		_int iInteractModelTagSize = 0;
		vector<string> vecModelTag;

		if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
		{
			iInteractModelTagSize = (_int)m_vecInteractModelTag.size();
			vecModelTag = m_vecInteractModelTag;
		}
		else if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
		{
			iInteractModelTagSize = (_int)m_vecAnimInteractModelTag.size();
			vecModelTag = m_vecAnimInteractModelTag;
		}

		for (_uint i = 0; i < (_uint)iInteractModelTagSize; ++i)
		{
			const _bool isSelected = (m_iSelectModelTag == i);

			if (ImGui::Selectable(vecModelTag[i].c_str(), isSelected))
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

	Set_GuizmoCamView();
	Set_GuizmoCamProj();
	Set_Guizmo(m_pPickingObject);

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

void CWindow_MapTool::Interact_DeleteTab()
{
	if (m_pPreviewInteract != nullptr)
	{
		m_pPreviewInteract->Set_Dead(true);
		m_pPreviewInteract = nullptr;
	}
	
	_uint iCreateInteractSize = (_uint)m_vecCreateInteractObject.size();

	if(iCreateInteractSize == 0)
		return;

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::BeginListBox(u8"생성한 상호작용오브젝트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iCreateInteractSize; ++i)
		{
			const _bool isSelected = (m_iSelectObjectIndex == i);

			if (ImGui::Selectable(m_vecCreateInteractObjectTag[i].c_str(), isSelected))
			{
				m_iSelectObjectIndex = i;

				m_bChange = true;
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
		}
		ImGui::EndListBox();
	}


	if (m_vecCreateInteractObject.size() < m_iSelectObjectIndex)
		return;

	
	Set_GuizmoCamView();
	Set_GuizmoCamProj();
	Set_Guizmo(m_vecCreateInteractObject[m_iSelectObjectIndex]);
		

	if (ImGui::Button(u8"삭제"))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_Dead(true);
		m_vecCreateInteractObject[m_iSelectObjectIndex] = nullptr;
		m_pPickingObject = nullptr;
		m_vecCreateInteractObject.erase(m_vecCreateInteractObject.begin() + m_iSelectObjectIndex);
		m_iSelectObjectIndex--;
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
			Delete_Tab(CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT);
	
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
			m_iSelectCharacterIndex = 0;
			Create_Tab(CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER);
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Character_SelectFunction();
			if (m_pPickingObject != nullptr)
				Guizmo_Tick(m_pPickingObject);
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{
			Delete_Tab(CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER);
			break;
		}
	}

	#ifdef _DEBUG
         if(m_pNavigation != nullptr)
		     m_pGameInstance->Add_DebugRender(m_pNavigation);
     #endif // _DEBUG

	 
}


void CWindow_MapTool::NPC_Tab_Function()
{
}

void CWindow_MapTool::Navigation_CreateTab()
{
	

	if (ImGui::Button(u8"마지막 저장한 네비파일 불러오기"))
	{
		LoadNavi(m_strNaviFinalSavePath);
	}


	if (nullptr == m_pNavigation)
		return;

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

		_uint iEnvironmentSize = (_uint)m_vecCreateObject.size();

		if (ImGui::BeginListBox(u8"네비게이션 픽킹대상"))
		{
			for (_int i = 0; i < (_int)iEnvironmentSize; ++i)
			{
				const _bool isSelected = m_iNavigationTargetIndex;

				if (ImGui::Selectable(m_vecCreateObjectTag[i].c_str(), isSelected))
				{
					m_iNavigationTargetIndex = i;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();
		}

	ImGui::EndChild();

	ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);

	_int iPickedSize = (_int)m_vecPickingListBox.size();

	if (false == m_vecPickedPoints.empty())
	{
		if (ImGui::BeginListBox(u8"픽킹 정보"))
		{
			for (_int i = 0; i < iPickedSize; ++i)
			{
				const _bool isSelected = (m_iNaviListBoxIndex == i);

				if (ImGui::Selectable(m_vecPickingListBox[i].c_str(), isSelected))
				{
					m_iNaviListBoxIndex = i;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}

		if (m_iNaviListBoxIndex != -1)
		{
			ImGui::Text(u8"픽킹 X : %f", m_vecPickedPoints[m_iNaviListBoxIndex].x);
			ImGui::Text(u8"픽킹 Y : %f", m_vecPickedPoints[m_iNaviListBoxIndex].y);
			ImGui::Text(u8"픽킹 Z : %f", m_vecPickedPoints[m_iNaviListBoxIndex].z);

			_float vPoints[3] = { m_vecPickedPoints[m_iNaviListBoxIndex].x, m_vecPickedPoints[m_iNaviListBoxIndex].y, m_vecPickedPoints[m_iNaviListBoxIndex].z };

			if (ImGui::InputFloat3(u8"포인트값변경", vPoints))
			{
				m_vecPickedPoints[m_iNaviListBoxIndex].x = vPoints[0];
				m_vecPickedPoints[m_iNaviListBoxIndex].y = vPoints[1];
				m_vecPickedPoints[m_iNaviListBoxIndex].z = vPoints[2];
			}



		}

		if (ImGui::Button(u8"픽킹인덱스 삭제"))
		{
			if (m_iNaviListBoxIndex < m_vecPickedPoints.size()) {
				m_vecPickedPoints.erase(m_vecPickedPoints.begin() + m_iNaviListBoxIndex);
				m_vecPickingListBox.erase(m_vecPickingListBox.begin() + m_iNaviListBoxIndex);

				if (m_vecPickingListBox.size() == 0)
					m_iNaviListBoxIndex = -1;
				else
					m_iNaviListBoxIndex = (_int)m_vecPickingListBox.size() - 1;

			}
		}
	}

	ImGui::EndChild();


	ImGui::NewLine();

	if (ImGui::Button(u8"네비게이션 생성") || m_pGameInstance->Key_Down(DIK_K))
	{
		if (3 > m_iCurrentPickingIndex)
			return;



		vector<double> fPoints;
		//fPoints.reserve(iPickedSize * 2);

		for (_int i = 0; i < iPickedSize; ++i)
		{
			fPoints.push_back(m_vecPickedPoints[i].x);

			fPoints.push_back(m_vecPickedPoints[i].z);
		}


		delaunator::Delaunator d(fPoints);


		for (size_t i = 0; i < d.triangles.size(); i += 3)
		{
			//"Triangle points: [[%f, %f], [%f, %f], [%f, %f]]\n",
			//	d.coords[2 * d.triangles[i]],        //tx0            
			//	d.coords[2 * d.triangles[i] + 1],    //ty0
			//	d.coords[2 * d.triangles[i + 1]],    //tx1
			//	d.coords[2 * d.triangles[i + 1] + 1],//ty1
			//	d.coords[2 * d.triangles[i + 2]],    //tx2
			//	d.coords[2 * d.triangles[i + 2] + 1] //ty2
			_float3 points[3] = { m_vecPickedPoints[d.triangles[i]], m_vecPickedPoints[d.triangles[i + 1]], m_vecPickedPoints[d.triangles[i + 2]] };

			Set_CCW(points);

			CCell* pCell = CCell::Create(m_pDevice, m_pContext, points, m_iNaviIndex++);

			m_pNavigation->AddCell(pCell);
		}

		Reset_NaviPicking();
	}

	ImGui::Checkbox(u8"픽킹모드", &m_bPickingNaviMode);

	if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck() && true == m_bPickingNaviMode)
	{

		_float3 fPickedPos = { 0.f, 0.f, 0.f };

		if (true == m_vecCreateObject[m_iNavigationTargetIndex]->Picking(&fPickedPos))
		{
			fPickedPos = XMVector3TransformCoord(XMLoadFloat3(&fPickedPos), m_vecCreateObject[m_iNavigationTargetIndex]->Get_Transform()->Get_WorldMatrix());

			Find_NearPointPos(&fPickedPos);
			m_vecPickedPoints.push_back(fPickedPos);
			m_vecPickingListBox.push_back(to_string(m_iNaviPickingIndex));
			++m_iCurrentPickingIndex;
			++m_iNaviPickingIndex;
			m_fNaviPickingPos = fPickedPos;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_H))
	{

		_float4 vCamPos = m_pGameInstance->Get_CamPosition();

		_float3 vPointPos = { vCamPos.x, vCamPos.y, vCamPos.z };

		Find_NearPointPos(&vPointPos);
		m_vecPickedPoints.push_back(vPointPos);
		m_vecPickingListBox.push_back(to_string(m_iNaviPickingIndex));
		++m_iCurrentPickingIndex;
		++m_iNaviPickingIndex;
		m_fNaviPickingPos = vPointPos;

	}
}

void CWindow_MapTool::Navigation_SelectTab()
{
	if(true == m_vecCreateObject.empty())
		return;

	if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck())
	{
		_bool bIsPicking = false;
		_float3 fPickedPos = {};

		if (m_vecCreateObject[m_iNavigationTargetIndex]->Picking(&fPickedPos))
		{
			fPickedPos = XMVector3TransformCoord(XMLoadFloat3(&fPickedPos), m_vecCreateObject[m_iNavigationTargetIndex]->Get_Transform()->Get_WorldMatrix());

			Find_NearPointPos(&fPickedPos);

			m_fNaviPickingPos = fPickedPos;
			bIsPicking = true;
		}

		if (true == bIsPicking)
		{
			CCell* pTargetCell = Find_NearCell(fPickedPos);

			if (nullptr == pTargetCell)
				return;

			m_iCellIndex = pTargetCell->Get_Index();


			m_vecCells[m_iCellIndex]->Set_Picking(true);
		}
	}


	_int iCellSize = (_int)m_vecCellIndexs.size();

	if (nullptr != m_pNavigation && false == m_vecCells.empty())
	{
		if (ImGui::BeginListBox(u8""))
		{
			for (_int i = 0; i < iCellSize; ++i)
			{
				const _bool isSelected = (m_iCellIndex == i);

				if (ImGui::Selectable(m_vecCellIndexs[i].c_str(), isSelected))
				{

					m_iCellIndex = i;

					m_vecCells[m_iCellIndex]->Set_Picking(true);

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				if (i == m_iCellIndex)
					continue;
				else
					m_vecCells[i]->Set_Picking(false);


			}

			ImGui::EndListBox();
		}

		ImGui::RadioButton(u8"포인트 A", &m_iPointIndex, 0); ImGui::SameLine(); ImGui::RadioButton(u8"포인트 B", &m_iPointIndex, 1);  ImGui::SameLine(); ImGui::RadioButton(u8"포인트 C", &m_iPointIndex, 2);

		ImGui::NewLine();

		_float3 vPoint = *m_vecCells[m_iCellIndex]->Get_Point((CCell::POINT)m_iPointIndex);

		_float vPoints[3] = { vPoint.x, vPoint.y, vPoint.z };

		if (ImGui::DragFloat3(u8"포인트값변경", vPoints, 0.1f))
		{
			_float3 vPassPoint = { vPoints[0], vPoints[1], vPoints[2] };

			//m_vecCells[m_iCellIndex]->Set_Point(CCell::POINT_A, vPassPoint);
			//m_vecCells[m_iCellIndex]->Set_Point((CCell::POINT)m_iPointIndex, vPassPoint);

			m_pNavigation->InRangeCellChange(m_vecCells[m_iCellIndex], m_iPointIndex, vPassPoint);
		}

	}
}

void CWindow_MapTool::Navigation_DeleteTab()
{
	if (m_pPlayer != nullptr)
	{
		CNavigation* pNavi = m_pPlayer->Get_Navigation();
		if(pNavi != nullptr)
			Safe_Release(pNavi);
	}

	vector<CCell*> vecCells = m_pNavigation->Get_Cells();
	_int iCellSize = (_int)vecCells.size();

	if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck())
	{
		_int index = 0;

		_float3 fPickedPos = { 0.f, 0.f, 0.f };

		_int	iNonAnimObjectSize = (_int)m_vecCreateObject.size();

		_int	iIndex = 0;
		_float fHighestYValue = -FLT_MAX;
		_float3 vHighestPickesPos = {};
		_bool	bIsPicking = false;



		if (m_vecCreateObject[m_iNavigationTargetIndex]->Picking(&fPickedPos))
		{
			
			Find_NearPointPos(&fPickedPos);

			m_fNaviPickingPos = fPickedPos;
			bIsPicking = true;
		}

		if (true == bIsPicking)
		{
			fPickedPos = XMVector3TransformCoord(XMLoadFloat3(&fPickedPos), m_vecCreateObject[m_iNavigationTargetIndex]->Get_Transform()->Get_WorldMatrix());

			CCell* pTargetCell = nullptr;
			pTargetCell = Find_NearCell(fPickedPos);

			if (nullptr == pTargetCell)
				return;

			m_pNavigation->Delete_Cell(pTargetCell->Get_Index());
		}
		
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPADENTER))
	{
		_float4 vCamPos = m_pGameInstance->Get_CamPosition();

		_float3 vPointPos = { vCamPos.x, vCamPos.y, vCamPos.z };

		Find_NearPointPos(&vPointPos);

		CCell* pTargetCell = nullptr;

		pTargetCell = Find_NearCell(vPointPos);

		if (nullptr == pTargetCell)
			return;

		m_pNavigation->Delete_Cell(pTargetCell->Get_Index());

	}
}

void CWindow_MapTool::Set_CCW(_float3* vPoint)
{
	_vector vPositionFromVector[3];
	for (int i(0); i < 3; i++)
		vPositionFromVector[i] = XMLoadFloat3(&(vPoint[i]));

	_vector vAtoB(vPositionFromVector[1] - vPositionFromVector[0]);
	_vector vAtoC(vPositionFromVector[2] - vPositionFromVector[0]);

	_vector vAtoB2D, vAtoC2D, vAtoB2DCross;
	vAtoB2D = vAtoC2D = vAtoB2DCross = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	vAtoB2D = XMVectorSetX(vAtoB2D, XMVectorGetX(vAtoB));
	vAtoB2D = XMVectorSetY(vAtoB2D, XMVectorGetZ(vAtoB));
	vAtoC2D = XMVectorSetX(vAtoC2D, XMVectorGetX(vAtoC));
	vAtoC2D = XMVectorSetY(vAtoC2D, XMVectorGetZ(vAtoC));
	vAtoB2DCross = XMVectorSetX(vAtoB2DCross, -1.f * XMVectorGetY(vAtoB2D));
	vAtoB2DCross = XMVectorSetY(vAtoB2DCross, XMVectorGetX(vAtoB2D));
	_float fDot(XMVectorGetX(XMVector2Dot(vAtoB2DCross, vAtoC2D)));
	if (0.f < fDot)
	{
		XMStoreFloat3(&vPoint[1], vPositionFromVector[2]);
		XMStoreFloat3(&vPoint[2], vPositionFromVector[1]);
	}
}

void CWindow_MapTool::Reset_NaviPicking()
{
	m_iCurrentPickingIndex = 0;
	m_vecPickedPoints.clear();
	m_vecPickingListBox.clear();

	m_iNaviListBoxIndex = 0;
	m_iNaviPickingIndex = 0;
}

void CWindow_MapTool::Find_NearPointPos(_float3* fPickedPos)
{
	vector<CCell*> vecCells = m_pNavigation->Get_Cells();
	_int iCellSize = (_int)vecCells.size();
	_float fMinDistance = FLT_MAX;

	_float3 vPickedPos = *fPickedPos;



	for (_int i = 0; i < iCellSize; ++i)
	{
		_float3 vPointA = *vecCells[i]->Get_Point(CCell::POINT_A);
		_float3 vPointB = *vecCells[i]->Get_Point(CCell::POINT_B);
		_float3 vPointC = *vecCells[i]->Get_Point(CCell::POINT_C);

		_float distanceA = (_float)sqrt(pow(vPickedPos.x - vPointA.x, 2) +
			pow(vPickedPos.y - vPointA.y, 2) +
			pow(vPickedPos.z - vPointA.z, 2));

		_float distanceB = (_float)sqrt(pow(vPickedPos.x - vPointB.x, 2) +
			pow(vPickedPos.y - vPointB.y, 2) +
			pow(vPickedPos.z - vPointB.z, 2));

		_float distanceC = (_float)sqrt(pow(vPickedPos.x - vPointC.x, 2) +
			pow(vPickedPos.y - vPointC.y, 2) +
			pow(vPickedPos.z - vPointC.z, 2));

		if (distanceA < fMinDistance && distanceA < m_fCombinationRange)
		{
			fMinDistance = distanceA;
			*fPickedPos = vPointA;
		}

		if (distanceB < fMinDistance && distanceB < m_fCombinationRange)
		{
			fMinDistance = distanceB;
			*fPickedPos = vPointB;
		}

		if (distanceC < fMinDistance && distanceC < m_fCombinationRange)
		{
			fMinDistance = distanceC;
			*fPickedPos = vPointC;
		}
	}
}

CCell* CWindow_MapTool::Find_NearCell(_float3 fPickedPos)
{
	vector<CCell*> vecCells = m_pNavigation->Get_Cells();
	_int iCellSize = (_int)vecCells.size();
	_float fMinDistance = FLT_MAX;
	_float3 vPickedPos = fPickedPos;
	CCell* pNearestCell = nullptr; // 가장 근접한 셀을 저장할 변수

	for (_int i = 0; i < iCellSize; ++i)
	{
		_float3 vPointA = *vecCells[i]->Get_Point(CCell::POINT_A);
		_float3 vPointB = *vecCells[i]->Get_Point(CCell::POINT_B);
		_float3 vPointC = *vecCells[i]->Get_Point(CCell::POINT_C);

		_float distanceA = (_float)sqrt(pow(vPickedPos.x - vPointA.x, 2) +
			pow(vPickedPos.y - vPointA.y, 2) +
			pow(vPickedPos.z - vPointA.z, 2));

		_float distanceB = (_float)sqrt(pow(vPickedPos.x - vPointB.x, 2) +
			pow(vPickedPos.y - vPointB.y, 2) +
			pow(vPickedPos.z - vPointB.z, 2));

		_float distanceC = (_float)sqrt(pow(vPickedPos.x - vPointC.x, 2) +
			pow(vPickedPos.y - vPointC.y, 2) +
			pow(vPickedPos.z - vPointC.z, 2));

		// 각 거리를 비교하여 최소 거리를 찾음
		if (distanceA < fMinDistance && distanceA < m_fCombinationRange)
		{
			fMinDistance = distanceA;
			pNearestCell = vecCells[i];
		}

		if (distanceB < fMinDistance && distanceB < m_fCombinationRange)
		{
			fMinDistance = distanceB;
			pNearestCell = vecCells[i];
		}

		if (distanceC < fMinDistance && distanceC < m_fCombinationRange)
		{
			fMinDistance = distanceC;
			pNearestCell = vecCells[i];
		}
	}

	return pNearestCell;
}

void CWindow_MapTool::SaveNavi(string strFullPath)
{
	m_strNaviFinalSavePath = strFullPath;
	m_bHaveNaviSave = true;

	wstring strConvertPath;
	m_pGameInstance->String_To_WString(strFullPath, strConvertPath);

	m_pNavigation->SaveData(strConvertPath);
}

void CWindow_MapTool::LoadNavi(string strFullPath)
{
	wstring strConvertPath;
	m_pGameInstance->String_To_WString(strFullPath, strConvertPath);

	m_pNavigation->LoadData(strConvertPath);
	
	LoadCells();
}

void CWindow_MapTool::LoadCells()
{
	vector<CCell*> vecCells = m_pNavigation->Get_Cells();

	_int iCellSize = (_int)vecCells.size();

	for (_int i = 0; i < iCellSize; ++i)
	{
		m_vecCells.push_back(vecCells[i]);
		m_vecCellIndexs.push_back(to_string(m_vecCells[i]->Get_Index()));
	}
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
					m_pToolCamera->Set_CameraType((CMasterCamera::CameraType)i);
				}
			}
			
			ImGui::Text(u8"키보드 PAGE UP 키누를시 모드 전환입니다.");
			
			if (m_pGameInstance->Key_Down(DIK_PGUP))
			{
					//DynamicCamera,//툴 및 테스트용 카메라 
					//SpringCamera,//플레이어 에 붙어 있는 카메라 

					if(IM_ARRAYSIZE(CameraType) > iCameraType + 1)
						iCameraType = iCameraType + 1;
					else
						iCameraType = 0;

					m_pToolCamera->Set_CameraType((CMasterCamera::CameraType)iCameraType);
			}

			
		}ImGui::NewLine();
	}
	



	ImGui::End();
}



#ifdef _DEBUG
void CWindow_MapTool::MouseInfo_Window(_float fTimeDelta)
{

	
	
	if (true == m_bShowCreateField)
	{
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysHorizontalScrollbar;

		ImGui::Begin(u8"셋팅", nullptr, WindowFlags);

			if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
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
			else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
			{
				if (true == m_vecCreateObject.empty())
				{
					ImGui::Text(u8"메쉬픽킹 대상 오브젝트가 없습니다.");
					ImGui::End();
					return;
				}

				_uint iCreateObjectSize = (_uint)m_vecCreateObject.size();

				
					if (ImGui::BeginListBox(u8"메쉬픽킹리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						for (_uint i = 0; i < iCreateObjectSize; ++i)
						{
							const _bool isSelected = (m_iSelectMeshObjectIndex == i);

							if (ImGui::Selectable(m_vecCreateObjectTag[i].c_str(), isSelected))
							{
								m_iSelectMeshObjectIndex = i;

								if (isSelected)
								{

									ImGui::SetItemDefaultFocus();
								}
							}

							if (i != m_iSelectMeshObjectIndex)
								m_vecCreateObject[i]->Set_ColliderRender(false);
						}

						m_vecCreateObject[m_iSelectMeshObjectIndex]->Set_ColliderRender(true);
						ImGui::EndListBox();
					}

					if (m_vecCreateObject[m_iSelectMeshObjectIndex]->Picking(&m_fRayPos) && true == ImGui_MouseInCheck())
					{
						m_fMeshPos = XMVector3TransformCoord(XMLoadFloat3(&m_fRayPos), m_vecCreateObject[m_iSelectMeshObjectIndex]->Get_Transform()->Get_WorldMatrix());
					}

					ImGui::Text(u8"마우스 X: %.2f", m_fMeshPos.x); ImGui::SameLine();	ImGui::Text(u8"마우스 Y: %.2f", m_fMeshPos.y); ImGui::SameLine();	ImGui::Text(u8"마우스 Z: %.2f", m_fMeshPos.z);
			}

			else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE)
			{

				if (true == m_vecCreateInstance.empty())
				{
					ImGui::Text(u8"생성된 인스턴스 개체가 없습니다.");
					ImGui::End();
					return;
				}
					_int iCreateInstanceSize = (_int)m_vecCreateInstance.size();
		
					for (_int i = 0; i < iCreateInstanceSize; ++i)
					{
						m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
					
						if (true == m_vecCreateInstance[i]->Picking_Instance(&m_tWorldRay, &m_fRayPos) && true == ImGui_MouseInCheck())
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

			ImGui::SameLine();


			static _bool bFieldMoveMode = false;

			ImGui::Checkbox(u8"필드무브", &bFieldMoveMode);

			if (bFieldMoveMode == true)
			{
				if (m_pGameInstance->Key_Down(DIK_U))
				{
					m_pField->Update(XMLoadFloat3(&m_fRayPos), true);
				}

				if (m_pGameInstance->Key_Down(DIK_I))
				{
					m_pField->Update(XMLoadFloat3(&m_fRayPos), false);
				}

				
				static _float fMove[3];
				if (ImGui::InputFloat3(u8"필드이동", fMove))
				{
					_float3 vMove = { fMove[0], fMove[1], fMove[2] };
					//m_pField
					m_pField->Move_Field(vMove);
				}
			}

			
			
			

			

			ImGui::Separator();
		
		ImGui::End();
	}
}
#endif

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

		if (ImGui::Button(u8"리셋"))
		{
			Reset_Function();
		}
	}ImGui::NewLine();

	ImGui::SeparatorText(u8"카메라");
	{
		if (ImGui::InputFloat(u8"카메라 속도", &m_fCamaraSpeed))
		{
			//CData_Manager::GetInstance()->Get_Camera_Dynamic()->Get_Transform()->Set_Speed(m_fCamaraSpeed);
			m_pToolCamera->Get_DynamicCamera()->Get_Transform()->Set_Speed(m_fCamaraSpeed);
			//m_pToolCamera->Get_Transform()->Set_Speed(m_fCamaraSpeed);
		}
	}

	ImGui::SeparatorText(u8"스카이 박스");
	{
		// 스카이박스 텍스처 변경
		if (ImGui::InputInt(u8"스카이박스 텍스처", &m_iSkyTextureIndex, 1))
		{
			_uint iSkyTextureCount = m_pSkybox->Get_SkyTextureCount();

			if ((_int)iSkyTextureCount - 1 < m_iSkyTextureIndex)
				m_iSkyTextureIndex = iSkyTextureCount - 1;

			if (0 > m_iSkyTextureIndex)
				m_iSkyTextureIndex = 0;

			if (nullptr == m_pSkybox)
				return;

			m_pSkybox->Set_SkyType((CSky::SKYTYPE)m_iSkyTextureIndex);
		}
	}




	#ifdef _DEBUG
	MouseInfo_Window(m_fTimeDelta);
	#endif // _DEBUG

}

void CWindow_MapTool::IsCreatePlayer_ReadyCamara()
{
	if (nullptr != m_pGameInstance->Get_Player())
	{
			m_bCreateCamera = true;
			m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());
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

		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_SINGLE)
		{
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
			{
				iModelTagSize = (_uint)m_vecAnimEnviroModelTag.size();
				vecModelTag = m_vecAnimEnviroModelTag;
			}
			else
			{
				iModelTagSize = (_uint)m_vecSingleModelTag.size();
				vecModelTag = m_vecSingleModelTag;
			}
			
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

		ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);
		
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
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
			{
				Set_GuizmoCamView();
				Set_GuizmoCamProj();
				Set_Guizmo(m_pPickingObject);
			}
			else if(m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM && false == m_vecPreViewInstance.empty())
			{
				
				Set_GuizmoCamView();
				Set_GuizmoCamProj();
				Set_Guizmo(m_vecPreViewInstance[m_iSelectPreviewIndex]);

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

	if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT)
	{
		if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
		{
			if (ImGui::BeginListBox(u8"미리보기인스턴스 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_int iPreviewInstanceSize = (_int)m_vecPreViewInstance.size();


				for (_int i = 0; i < iPreviewInstanceSize; ++i)
				{
					const _bool isSelected = (m_iSelectPreviewIndex == i);

					if (ImGui::Selectable(m_vecPreViewInstanceTag[i].c_str(), isSelected))
					{
						m_iSelectPreviewIndex = i;

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndListBox();
			}
		}
		else
			ImGui::Text(u8"테스트2");
	}
	else
	{
		ImGui::Text(u8"테스트2");

	}

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

void CWindow_MapTool::Delete_Tab(TAP_TYPE eTabType)
{

	_uint iTagSize = 0;
	vector<string> vecCreateTag;
	_uint iSelectTag = 0;

	string strListBoxName = u8"";

	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
	{
		iTagSize = (_uint)m_vecCreateMonster.size();
		vecCreateTag = m_vecCreateMonsterTag;
		strListBoxName = u8"삭제할 캐릭터 객체 리스트";
		iSelectTag = m_iSelectCharacterTag;
	}
	else if(m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT)
		{
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
			{
				iTagSize = (_uint)m_vecPreViewInstance.size();
				vecCreateTag = m_vecPreViewInstanceTag;
				strListBoxName = u8"삭제할 미리보기 인스턴스 객체 리스트";
				iSelectTag = m_iSelectPreviewIndex;
				
			}
			else
			{
				iTagSize = (_uint)m_vecCreateObject.size();
				vecCreateTag = m_vecCreateObjectTag;
				strListBoxName = u8"삭제할 환경 객체 리스트";
				iSelectTag = m_iSelectObjectIndex;
			}
		}
		else
		{
			iTagSize = (_uint)m_vecCreateObject.size();
			vecCreateTag = m_vecCreateObjectTag;
			strListBoxName = u8"삭제할 환경 객체 리스트";
			iSelectTag = m_iSelectObjectIndex;
		}

	}



	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;
	
	if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iTagSize; ++i)
		{
			const _bool isSelected = (iSelectTag == i);

			if (ImGui::Selectable(vecCreateTag[i].c_str(), isSelected))
			{
				if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
					m_iSelectCharacterTag = i;
				else
					m_iSelectObjectIndex = i;

				m_bChange = true;
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
		}
		ImGui::EndListBox();
	}


	if(m_vecCreateObject.size() < m_iSelectObjectIndex || m_vecCreateMonster.size() < m_iSelectCharacterTag)
		return;

	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER && false == m_vecCreateMonster.empty() && m_vecCreateMonster[m_iSelectCharacterTag] != nullptr)
	{
		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_Guizmo(m_vecCreateMonster[m_iSelectCharacterTag]);
	}
	else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT && false == m_vecCreateObject.empty() && m_vecCreateObject[m_iSelectObjectIndex] != nullptr)
	{
		if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT && m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM && false == m_vecPreViewInstance.empty())
		{
			Set_GuizmoCamView();
			Set_GuizmoCamProj();
			Set_Guizmo(m_vecPreViewInstance[m_iSelectPreviewIndex]);
		}
		else
		{
			Set_GuizmoCamView();
			Set_GuizmoCamProj();
			Set_Guizmo(m_vecCreateObject[m_iSelectObjectIndex]);
		}
		
	}

	if (ImGui::Button(u8"삭제"))
	{
		if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
		{
			m_vecCreateMonster[m_iSelectCharacterTag]->Set_Dead(true);
			m_vecCreateMonster[m_iSelectCharacterTag] = nullptr;
			m_vecCreateMonster.erase(m_vecCreateMonster.begin() + m_iSelectCharacterTag);
			m_pPickingObject = nullptr;
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
		{
			if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT)
			{
				if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
				{


					for (auto& tag : m_vecCreateInstanceTag)
					{
						// 문자열에서 '@' 문자 이후의 부분을 지움
						size_t atIndex = tag.find('@');
						if (atIndex != std::string::npos) {
							tag.erase(atIndex); // '@' 이후의 문자열을 모두 제거
						}
					}

					size_t atIndex = m_vecPreViewInstanceTag[m_iSelectPreviewIndex].find('@');
					if (atIndex != std::string::npos)
					{
						m_vecPreViewInstanceTag[m_iSelectPreviewIndex].erase(atIndex);
					}

					auto iter = m_mapPreviewInstance.find(m_vecPreViewInstanceTag[m_iSelectPreviewIndex]);

					
					if (iter->second.size() != 0)
					{
						iter->second.erase(iter->second.begin() + iter->second.size() - 1);
					}
					else
					{
						iter->second.clear();
					}
					

					m_vecPreViewInstance[m_iSelectPreviewIndex]->Set_Dead(true);
					m_vecPreViewInstance[m_iSelectPreviewIndex] = nullptr;
					m_pPickingObject = nullptr;
					m_vecPreViewInstance.erase(m_vecPreViewInstance.begin() + m_iSelectPreviewIndex);
					m_iSelectPreviewIndex = 0;
				}
				else
				{
					m_vecCreateObject[m_iSelectObjectIndex]->Set_Dead(true);
					m_pPickingObject = nullptr;
					m_vecCreateObject[m_iSelectObjectIndex] = nullptr;
					m_vecCreateObject.erase(m_vecCreateObject.begin() + m_iSelectObjectIndex);
					m_iSelectObjectIndex--;
				}
			}
			else
			{
				m_vecCreateObject[m_iSelectObjectIndex]->Set_Dead(true);
				m_vecCreateObject[m_iSelectObjectIndex] = nullptr;
				m_pPickingObject = nullptr;
				m_vecCreateObject.erase(m_vecCreateObject.begin() + m_iSelectObjectIndex);
				m_iSelectObjectIndex--;
			}
			
			
		}
	}


}


void CWindow_MapTool::Preview_Function()
{
	Change_PreViewObject(m_eTabType);

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_NONE || m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_END)
		return;

	if (m_bDeadComplete == true && nullptr != m_pPreviewObject || nullptr != m_pPreviewCharacter || nullptr != m_pPreviewInteract || nullptr != m_pPreviewLightObject)
	{

		_vector vPos = {};

		if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
			vPos = { m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f};

		else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
		{
			vPos = { m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f};

		}
		else if(m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE)
			vPos = { m_fInstanceMeshPos.x, m_fInstanceMeshPos.y, m_fInstanceMeshPos.z, 1.f };


		if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER && m_pPreviewCharacter != nullptr)
		{
			m_pPreviewCharacter->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
		{
			if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_INTERACT && m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
			}
			else if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_LIGHT && m_pPreviewLightObject != nullptr)
			{
				m_pPreviewLightObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
			}
			else if (m_pPreviewObject != nullptr)
			{
				m_pPreviewObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
			}
			

		}
	}
}

void CWindow_MapTool::Change_PreViewObject(TAP_TYPE eTabType)
{	
	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER)
	{
		if (m_bChange == true && m_pPreviewCharacter != nullptr)
		{
			m_pPreviewCharacter->Set_Dead(true);

			m_bChange = false;
			m_pPreviewCharacter = nullptr;
		}

		if (nullptr == m_pPreviewCharacter)
		{

			wstring strPrototypeTag;
			//m_vecMonsterTag[m_iSelectCharacterTag];

			switch (eTabType)
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
				CGameObject::GAMEOBJECT_DESC Desc = {};

				m_pPreviewCharacter = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strPrototypeTag, &Desc);

				m_pPreviewCharacter->Get_Transform()->Set_Position(m_fRayPos);

			}
			
			
		}
	}
	else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_LIGHT)
		{
			if (m_pPreviewLightObject != nullptr && m_bChange == true)
			{
				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
				m_bChange = false;
			}


			if (m_pPreviewObject != nullptr)
			{
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;

			}
			else if (m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
			}

			if (nullptr == m_pPreviewLightObject)
			{
				CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc;

				LightObjectDesc.bAnimModel = m_bAnimType;
				LightObjectDesc.iShaderPassIndex = m_iShaderPassIndex;
				LightObjectDesc.bPreview = true;
				LightObjectDesc.iLightIndex = m_tEditLightDesc.iLightIndex;
				LightObjectDesc.LightDesc = m_tEditLightDesc;
				

				switch (m_eLightEffectType)
				{
				case 0:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TORCH;
					break;
				}

				case 1:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST1;
					break;
				}

				case 2:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST2;
					break;
				}

				case 3:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST3;
					break;
				}

				case 4:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST4;
					break;
				}

				}


				if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
				{
					m_pGameInstance->String_To_WString(m_vecSingleModelTag[m_iSelectModelTag], LightObjectDesc.strModelTag);

				}
				else
				{
					if (m_bAnimType == true)
					{
						LightObjectDesc.bAnimModel = true;
						LightObjectDesc.iPlayAnimationIndex = 0;
					}
					m_pGameInstance->String_To_WString(m_vecAnimEnviroModelTag[m_iSelectModelTag], LightObjectDesc.strModelTag);
				}

				m_pPreviewLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));
				m_pPreviewLightObject->Get_Transform()->Set_Position(m_fRayPos);
			}
		}

		else if (eTabType != CWindow_MapTool::TAP_TYPE::TAB_INTERACT)
		{
			if (m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
			}
				

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
					case Client::CWindow_MapTool::TAP_TYPE::TAB_SINGLE:
					{
						m_pGameInstance->String_To_WString(m_vecSingleModelTag[m_iSelectModelTag], Desc.strModelTag);

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
		
		else
		{
			if (m_pPreviewObject != nullptr)
			{
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;
			}

			if (m_bChange == true && m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_bChange = false;
				m_pPreviewInteract = nullptr;
				
			}

			if (nullptr == m_pPreviewInteract)
			{
				CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC InteractDesc = {};

				InteractDesc.bAnimModel = m_bAnimType;
				InteractDesc.iPlayAnimationIndex = m_iInteractPlayAnimIndex;
				InteractDesc.eInteractType = (CEnvironment_Interact::INTERACT_TYPE)m_eInteractType;
				InteractDesc.eInteractState = (CEnvironment_Interact::INTERACT_STATE)m_eInteractState;
				InteractDesc.iShaderPassIndex = m_iShaderPassIndex;
				InteractDesc.vColliderSize = _float3(m_fColliderSizeArray[0], m_fColliderSizeArray[1], m_fColliderSizeArray[2]);
				InteractDesc.vColliderCenter = _float3(m_fColliderCenterArray[0], m_fColliderCenterArray[1], m_fColliderCenterArray[2]);

				if (m_bAnimType == true)
				{
					InteractDesc.bAnimModel = true;
					m_pGameInstance->String_To_WString(m_vecAnimInteractModelTag[m_iSelectModelTag], InteractDesc.strModelTag);
				}
				else
				{
					InteractDesc.bAnimModel = false;
					m_pGameInstance->String_To_WString(m_vecInteractModelTag[m_iSelectModelTag], InteractDesc.strModelTag);
				}


				m_pPreviewInteract = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_InteractObject", &InteractDesc));
				m_pPreviewInteract->Get_Transform()->Set_Position(m_fRayPos);
			}
		}
	
	}
	

}



void CWindow_MapTool::Picking_Function()
{


	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		if (nullptr != m_pPreviewLightObject && true == ImGui_MouseInCheck() || nullptr != m_pPreviewInteract && true == ImGui_MouseInCheck() || nullptr != m_pPreviewObject && true == ImGui_MouseInCheck())
		{

			switch (m_eTabType)
			{
				case Client::CWindow_MapTool::TAP_TYPE::TAB_SINGLE:
				{
					Ground_CreateFunction();
					break;
				}
				case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
				{
					Interact_CreateFunction();
					break;
				}
				case Client::CWindow_MapTool::TAP_TYPE::TAB_LIGHT:
				{
					Light_CreateFunction();
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
	else if(m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER) //! OBJECTMODE_CHARACTER
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
	
		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
		Desc.bAnimModel = m_bAnimType;
		Desc.iShaderPassIndex = m_iShaderPassIndex;
		Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
		Desc.bPreview = false;


		Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldFloat4x4();

		CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

		m_vecCreateObject.push_back(pObject);

		wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateObjectIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
		m_vecCreateObjectTag.push_back(strConvertTag);

		m_iCreateObjectIndex++;

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

void CWindow_MapTool::Light_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (m_eLightCreateMode == CWindow_MapTool::LIGHT_CREATEMODE::LIGHT_MODE && nullptr != m_pField && true == m_pField->MouseOnTerrain())
		{
			
			m_tEditLightDesc.eType = tagLightDesc::TYPE_POINT;
			CLight* pLight = m_pGameInstance->Add_Light_AndGet(m_tEditLightDesc, m_tEditLightDesc.iLightIndex);

			if (pLight == nullptr)
			{
				MSG_BOX("라이트 생성실패");
			}
			else
			{
				m_vecCreateLight.push_back(pLight);
				
				string strLightTag = "Light" + to_string(pLight->Get_LightIndex());
				m_vecCreateLightTag.push_back(strLightTag);
			}
			
		}

		else if (m_eLightCreateMode == CWindow_MapTool::LIGHT_CREATEMODE::LIGHT_OBJECTMODE)
		{
			if (nullptr != m_pField && true == m_pField->MouseOnTerrain())
			{
				CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc;
				LightObjectDesc.bAnimModel = m_bAnimType;
				LightObjectDesc.iShaderPassIndex = m_iShaderPassIndex;
				LightObjectDesc.bPreview = false;
				LightObjectDesc.iLightIndex = m_tEditLightDesc.iLightIndex;
				LightObjectDesc.LightDesc = m_tEditLightDesc;
				LightObjectDesc.strModelTag = m_pPreviewLightObject->Get_ModelTag();
				LightObjectDesc.WorldMatrix = m_pPreviewLightObject->Get_Transform()->Get_WorldMatrix();

				switch (m_eLightEffectType)
				{
				case 0:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TORCH;
					break;
				}

				case 1:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST1;
					break;
				}

				case 2:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST2;
					break;
				}

				case 3:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST3;
					break;
				}

				case 4:
				{
					LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST4;
					break;
				}

				}


				CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));

				if (pLightObject == nullptr)
				{
					MSG_BOX("라이트오브젝트 생성실패");
				}

				m_vecCreateLightObject.push_back(pLightObject);

				wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pLightObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateLightObjectIndex));
				string strConvertTag;
				m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
				m_vecCreateLightObjectTag.push_back(strConvertTag);

				m_iCreateLightObjectIndex++;
			}
		}
		
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{

		CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC LightObjectDesc;

		LightObjectDesc.bAnimModel = m_bAnimType;
		LightObjectDesc.iShaderPassIndex = m_iShaderPassIndex;
		LightObjectDesc.bPreview = false;
		LightObjectDesc.iLightIndex = m_tEditLightDesc.iLightIndex;
		LightObjectDesc.LightDesc = m_tEditLightDesc;
		LightObjectDesc.strModelTag = m_pPreviewLightObject->Get_ModelTag();
		LightObjectDesc.WorldMatrix = m_pPreviewLightObject->Get_Transform()->Get_WorldMatrix();
		

		switch (m_eLightEffectType)
		{
			case 0:
			{
				LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TORCH;
				break;
			}

			case 1:
			{
				LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST1;
				break;
			}

			case 2:
			{
				LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST2;
				break;
			}

			case 3:
			{
				LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST3;
				break;
			}

			case 4:
			{
				LightObjectDesc.eLightEffect = CEnvironment_LightObject::LIGHTEFFECT_TEST4;
				break;
			}

		}
		

		CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_LightObject", &LightObjectDesc));

		if (pLightObject == nullptr)
		{
			MSG_BOX("라이트오브젝트 생성실패");
		}

		m_vecCreateLightObject.push_back(pLightObject);

		wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pLightObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateLightObjectIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
		m_vecCreateLightObjectTag.push_back(strConvertTag);


		m_iCreateLightObjectIndex++;
	}

}

void CWindow_MapTool::Interact_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (nullptr != m_pField && true == m_pField->MouseOnTerrain())
		{
			CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC Desc;
			Desc.bAnimModel = m_bAnimType;
			Desc.iShaderPassIndex = m_iShaderPassIndex;
			Desc.eInteractState = (CEnvironment_Interact::INTERACT_STATE)m_eInteractState;
			Desc.eInteractType = (CEnvironment_Interact::INTERACT_TYPE)m_eInteractType;
			Desc.vColliderCenter = _float3(m_fColliderCenterArray[0], m_fColliderCenterArray[1], m_fColliderCenterArray[2]);
			Desc.vColliderSize = _float3(m_fColliderSizeArray[0], m_fColliderSizeArray[1], m_fColliderSizeArray[2]);
			Desc.strModelTag = m_pPreviewInteract->Get_ModelTag();
			Desc.bPreview = false;
			Desc.WorldMatrix = m_pPreviewInteract->Get_Transform()->Get_WorldMatrix();
			Desc.bLevelChange = m_bInteractLevelChange;
			//Desc.eChangeLevel = (LEVEL)m_eInteractLevel;

			switch (m_eInteractLevel)
			{
				case 0:
				{
					Desc.eChangeLevel = LEVEL_INTRO_BOSS;
					break;
				}

				case 1:
				{
					Desc.eChangeLevel = LEVEL_SNOWMOUNTAIN;
					break;
				}
			}

			CEnvironment_Interact* pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

			m_vecCreateInteractObject.push_back(pObject);


			wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_vecCreateInteractIndex));
			string strConvertTag;
			m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
			m_vecCreateInteractObjectTag.push_back(strConvertTag);

			m_vecCreateInteractIndex++;
		}
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{

		CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC Desc;
		Desc.bAnimModel = m_bAnimType;
		Desc.iShaderPassIndex = m_iShaderPassIndex;
		Desc.eInteractState = (CEnvironment_Interact::INTERACT_STATE)m_eInteractState;
		Desc.eInteractType = (CEnvironment_Interact::INTERACT_TYPE)m_eInteractType;
		Desc.vColliderCenter = _float3(m_fColliderCenterArray[0], m_fColliderCenterArray[1], m_fColliderCenterArray[2]);
		Desc.vColliderSize = _float3(m_fColliderSizeArray[0], m_fColliderSizeArray[1], m_fColliderSizeArray[2]);
		Desc.strModelTag = m_pPreviewInteract->Get_ModelTag();
		Desc.bPreview = false;
		Desc.WorldMatrix = m_pPreviewInteract->Get_Transform()->Get_WorldMatrix();
		Desc.bLevelChange = m_bInteractLevelChange;
		//Desc.eChangeLevel = (LEVEL)m_eInteractLevel;

		switch (m_eInteractLevel)
		{
			case 0:
			{
				Desc.eChangeLevel = LEVEL_INTRO_BOSS;
				break;
			}

			case 1:
			{
				Desc.eChangeLevel = LEVEL_SNOWMOUNTAIN;
				break;
			}
		}

		CEnvironment_Interact* pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

		m_vecCreateInteractObject.push_back(pObject);


		wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_vecCreateInteractIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
		m_vecCreateInteractObjectTag.push_back(strConvertTag);

		m_vecCreateInteractIndex++;
	}
	

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
			m_pPickingObject = pObject;


			string strModelTag = {};
			m_pGameInstance->WString_To_String(Desc.strModelTag, strModelTag);
		
			strModelTag = strModelTag + "@" + to_string(m_vecPreViewInstance.size());

			m_vecPreViewInstance.push_back(pObject);
			m_vecPreViewInstanceTag.push_back(strModelTag);
			m_iCreatePreviewIndex++;
		}

		
		
		//!m_vecPreViewInstance.push_back(pObject);
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{
			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;
			Desc.bAnimModel = m_bAnimType;
			Desc.iShaderPassIndex = m_iShaderPassIndex;
			Desc.strModelTag = m_pPreviewObject->Get_ModelTag();
			Desc.bPreview = false;

			Desc.WorldMatrix = m_pPreviewObject->Get_Transform()->Get_WorldFloat4x4();

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
				m_pPickingObject = pObject;

				string strModelTag = {};
				m_pGameInstance->WString_To_String(Desc.strModelTag, strModelTag);

				strModelTag = strModelTag + "@" + to_string(m_vecPreViewInstance.size());
				m_vecPreViewInstance.push_back(pObject);
				m_vecPreViewInstanceTag.push_back(strModelTag);
				m_iCreatePreviewIndex++;

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
				m_pPickingObject = pObject;

				string strModelTag = {};
				m_pGameInstance->WString_To_String(Desc.strModelTag, strModelTag);

				strModelTag = strModelTag + "@" + to_string(m_vecPreViewInstance.size());
				m_vecPreViewInstance.push_back(pObject);
				m_vecPreViewInstanceTag.push_back(strModelTag);
				m_iCreatePreviewIndex++;
			}
	}
}

void CWindow_MapTool::Create_Instance()
{
	
	//_int iCreateInstanceSize = m_mapPreviewInstance.size();
	if (m_pPickingObject != nullptr)
	{
		m_pPickingObject->Set_Dead(true);
		m_pPickingObject = nullptr;
	}


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

	_int iEnvironModelTagSize = (_int)m_vecEnviroModelTag.size();
	for (int i = 0; i < iEnvironModelTagSize; ++i) //! 인스턴싱용 깡통맵
	{
		vector<CEnvironment_Object*> EmptyVector;

		m_mapPreviewInstance.emplace(m_vecEnviroModelTag[i], EmptyVector);
	}

	_int iPreviewVectorSize = (_int)m_vecPreViewInstance.size();

	for (_int i = 0; i < iPreviewVectorSize; ++i)
	{
		m_vecPreViewInstance[i]->Set_Dead(true);
		m_vecPreViewInstance[i] = nullptr;
	}
	m_vecPreViewInstance.clear();

	m_iCreatePreviewIndex = 0;
	m_iSelectPreviewIndex = 0;
	m_vecPreViewInstanceTag.clear();
	
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
		if (nullptr != m_pField && true == m_pField->MouseOnTerrain() && true == ImGui_MouseInCheck())
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
	
	CMonster_Character::MONSTER_DESC Desc;
	Desc.bPreview = false;
	Desc.WorldMatrix = m_pPreviewCharacter->Get_Transform()->Get_WorldMatrix();
	

	wstring strProtoTag;
	m_pGameInstance->String_To_WString(m_vecMonsterTag[m_iSelectCharacterTag], strProtoTag);

	Desc.strProtoTypeTag = strProtoTag;
	Desc.eDescType = CGameObject::MONSTER_DESC;

	CMonster_Character* pMonster = dynamic_cast<CMonster_Character*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strProtoTag, &Desc));

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
				m_iSelectInstanceIndex = 0;

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
					

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button(u8"인스턴스삭제"))
		{
			if (FAILED(m_vecCreateInstance[m_iSelectEnvironmentIndex]->Remove_Instance(m_iSelectInstanceIndex)))
			{
				MSG_BOX("삭제 실패");
			}
			else
			{
				if (m_iSelectInstanceIndex != 0)
					m_iSelectInstanceIndex--;

				
				//m_vecCreateInstance[m_iSelectInstanceIndex]->
			}
		}

		ImGui::EndChild();

		ImGui::SameLine();


		vector<INSTANCE_INFO_DESC> Desc = *m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfoDesc();

		_int iNumInstance = (_int)Desc.size();

		if (ImGui::Button(u8"인스턴스 흔들림잡기용"))
		{

			for (_uint i = 0; i < (_uint)iNumInstance; ++i)
			{
				m_pPickingInstanceInfo = m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfo(i);
				Instance_GuizmoTick(m_iSelectEnvironmentIndex, m_pPickingInstanceInfo);
			}
		}

		if (m_pGameInstance->Key_Down(DIK_HOME))
		{
			if(iNumInstance - 1 > (_int)m_iSelectInstanceIndex)
				m_iSelectInstanceIndex++;
			else
				m_iSelectInstanceIndex = 0;
		
		}

		if (m_pGameInstance->Key_Down(DIK_END))
		{
			if (0 < m_iSelectInstanceIndex)
				m_iSelectInstanceIndex--;
			else
				m_iSelectInstanceIndex = iNumInstance - 1;
		}


		if (m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_NumInstance() > 0)
		{

			Set_GuizmoCamView();
			Set_GuizmoCamProj();

			m_pPickingInstanceInfo = m_vecCreateInstance[m_iSelectEnvironmentIndex]->Get_InstanceInfo(m_iSelectInstanceIndex);
			Instance_GuizmoTick(m_iSelectEnvironmentIndex, m_pPickingInstanceInfo);
		}
		else
		{
			CEnvironment_Instance* pInstance = m_vecCreateInstance[m_iSelectEnvironmentIndex];

			m_vecCreateInstance.erase(m_vecCreateInstance.begin() + m_iSelectEnvironmentIndex);
			m_vecCreateInstanceTag.erase(m_vecCreateInstanceTag.begin() + m_iSelectEnvironmentIndex);
			m_vecInstanceInfoTag.clear();
			m_iSelectInstanceIndex = 0;
			m_iSelectEnvironmentIndex--;
			Safe_Release(pInstance);

			
		}

	}

	
}

void CWindow_MapTool::Interact_SelectFunction()
{
	_uint iObjectTagSize = (_uint)m_vecCreateInteractObject.size();

	if (m_pPreviewInteract != nullptr)
	{
		m_pPreviewInteract->Set_Dead(true);
		m_pPreviewInteract = nullptr;
	}
	
	if (true == m_vecCreateInteractObject.empty())
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

				if (ImGui::Selectable(m_vecCreateInteractObjectTag[i].c_str(), isSelected))
				{
					m_iSelectObjectIndex = i;

					m_pPickingObject = m_vecCreateInteractObject[m_iSelectObjectIndex];

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}


		if(ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex))
		{
#ifdef _DEBUG
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_ShaderPassIndex(m_iShaderPassIndex);
#endif // _DEBUG
		}

		

		ImGui::SeparatorText(u8"상호작용 셋팅");
		{
			const char* InteractTypes[] = { "INTERACT_JUMP100", "INTERACT_JUMP200", "INTERACT_JUMP300", "INTERACT_VAULT100", "INTERACT_VAULT200" };
			const char* InteractPreviewType = InteractTypes[m_eInteractType];

			static ImGuiComboFlags ComboFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall;

			if (ImGui::BeginCombo(u8"상호작용 타입", InteractPreviewType, ComboFlags))
			{
				for (int i = 0; i < IM_ARRAYSIZE(InteractTypes); ++i)
				{
					const bool is_Selected = (m_eInteractType == i);

					if (ImGui::Selectable(InteractTypes[i], is_Selected))
					{
						m_eInteractType = i;
						#ifdef _DEBUG
                          m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_InteractType((CEnvironment_Interact::INTERACT_TYPE)m_eInteractType);
                        #endif // _DEBUG

					}

					if (true == is_Selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::SameLine();

			CEnvironment_Interact::INTERACT_STATE eInteractState = CEnvironment_Interact::INTERACT_STATE::INTERACTSTATE_END;

			//static _int iInstanceState = 0;
			const char* InstanceState[2] = { u8"무한 상호작용", u8"한번만 수행" };

			for (_uint i = 0; i < IM_ARRAYSIZE(InstanceState); ++i)
			{
				if (i > 0) { ImGui::SameLine(); }

				if (ImGui::RadioButton(InstanceState[i], &m_eInteractState, i))
				{
					m_eInteractState = i;
					#ifdef _DEBUG
						m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_InteractState((CEnvironment_Interact::INTERACT_STATE)m_eInteractState);
					#endif // _DEBUG

					//eInteractState = CEnvironment_Interact::INTERACT_STATE(iInstanceState);
				}
			}
		}


		

		ImGui::Checkbox(u8"레벨 체인지", &m_bInteractLevelChange);

		if (m_bInteractLevelChange == true)
		{
			ImGui::SeparatorText(u8"상호작용시 이동할 레벨");
			{
				//!LEVEL_INTRO_BOSS,
				//!	LEVEL_SNOWMOUNTAIN,
				const char* InteractLevels[] = { u8"인트로보스레벨", u8"설산레벨" };
				const char* InteractPreviewLevel = InteractLevels[m_eInteractLevel];

				static ImGuiComboFlags ComboLevelFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightSmall;

				if (ImGui::BeginCombo(u8"이동할 레벨", InteractPreviewLevel, ComboLevelFlags))
				{
					for (int i = 0; i < IM_ARRAYSIZE(InteractLevels); ++i)
					{
						const bool is_Selected = (m_eInteractLevel == i);

						if (ImGui::Selectable(InteractLevels[i], is_Selected))
						{
							m_eInteractLevel = i;
						}

						if (true == is_Selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}


			if (ImGui::Button(u8"레벨체인지 셋팅"))
			{
				switch (m_eInteractLevel)
				{
					case 0:
					{
						#ifdef _DEBUG
                          m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_LevelChangeType(m_bInteractLevelChange, LEVEL_INTRO_BOSS);
                        #endif // _DEBUG
						break;
					}

					case 1:
					{
						#ifdef _DEBUG
							m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_LevelChangeType(m_bInteractLevelChange, LEVEL_SNOWMOUNTAIN);
						#endif // _DEBUG
						break;
					}
				}
			}
			
		}

		ImGui::SeparatorText(u8"콜라이더 셋팅");
		{
			ImGui::InputFloat3(u8"콜라이더 사이즈", m_fSelectColliderSizeArray); 
			ImGui::InputFloat3(u8"콜라이더 센터", m_fSelectColliderCenterArray);


			
			if (ImGui::Button(u8"콜라이더 사이즈 업데이트"))
			{
				#ifdef _DEBUG
					m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_ColliderSize(_float3(m_fSelectColliderSizeArray[0], m_fSelectColliderSizeArray[1], m_fSelectColliderSizeArray[2]));
				#endif // _DEBUG
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"콜라이더 센터 업데이트"))
			{
				#ifdef _DEBUG
					m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_ColliderCenter(_float3(m_fSelectColliderCenterArray[0], m_fSelectColliderCenterArray[1], m_fSelectColliderCenterArray[2]));
				#endif // _DEBUG
			}
		}
	}



	Guizmo_Tick(m_pPickingObject);
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

		static _bool bAllInstanceMove = false;
		
		ImGui::Checkbox(u8"모든 인스턴싱 이동", &bAllInstanceMove); //ImGui::SameLine(); ImGui::Checkbox(u8"이동값 기록모드")

		if (bAllInstanceMove == true)
		{
			static _int iInstanceMode = 0;

			const char* InstanceModeType[3] = { u8"X 좌표 전부이동", u8"Y 좌표 전부이동", u8"Z 좌표 전부이동"};

			for (_int i = 0; i < IM_ARRAYSIZE(InstanceModeType); ++i)
			{
				if (i > 0) { ImGui::SameLine(); }

				if (ImGui::RadioButton(InstanceModeType[i], &iInstanceMode, i))
				{
					m_eInstanceAllMoveMode = ECast<INSTANCE_ALLMOVETYPE>(iInstanceMode);
				}
				
			}

			if (ImGui::Button(u8"인스턴스 전부 이동"))
			{
				switch (m_eInstanceAllMoveMode)
				{
				case Client::CWindow_MapTool::INSTANCE_ALLMOVETYPE::ALLMOVE_X:
					{
						MAPTOOL_INSTANCE_DESC InstanceDesc = m_vecCreateInstance[iIndex]->Get_InstanceDesc();

						_uint iNumInstance = InstanceDesc.iNumInstance;

						for (_uint i = 0; i < iNumInstance; ++i)
						{
							InstanceDesc.vecInstanceInfoDesc[i].vTranslation.x = pInstance->vTranslation.x;						
							m_vecCreateInstance[iIndex]->Update(InstanceDesc.vecInstanceInfoDesc[i], i);
						}

						break;
					}
				case Client::CWindow_MapTool::INSTANCE_ALLMOVETYPE::ALLMOVE_Y:
					{
						MAPTOOL_INSTANCE_DESC InstanceDesc = m_vecCreateInstance[iIndex]->Get_InstanceDesc();

						_uint iNumInstance = InstanceDesc.iNumInstance;

						for (_uint i = 0; i < iNumInstance; ++i)
						{
							InstanceDesc.vecInstanceInfoDesc[i].vTranslation.y = pInstance->vTranslation.y;
							m_vecCreateInstance[iIndex]->Update(InstanceDesc.vecInstanceInfoDesc[i], i);
						}
						break;
					}
				case Client::CWindow_MapTool::INSTANCE_ALLMOVETYPE::ALLMOVE_Z:
					{
						MAPTOOL_INSTANCE_DESC InstanceDesc = m_vecCreateInstance[iIndex]->Get_InstanceDesc();

						_uint iNumInstance = InstanceDesc.iNumInstance;

						for (_uint i = 0; i < iNumInstance; ++i)
						{
							InstanceDesc.vecInstanceInfoDesc[i].vTranslation.z = pInstance->vTranslation.z;
							m_vecCreateInstance[iIndex]->Update(InstanceDesc.vecInstanceInfoDesc[i], i);
						}
						break;
					}
				
				}
				
			}
		}

		_float* arrView = m_arrView;
		_float* arrProj = m_arrProj;

		_float	matrixTranslation[3], matrixRotation[3], matrixScale[3];
		_matrix matWorld = pInstance->Get_Matrix();

		XMStoreFloat4x4(&m_matInstanceMatrix, matWorld);
		
		
	

		_float arrWorld[] = { m_matInstanceMatrix._11,m_matInstanceMatrix._12,m_matInstanceMatrix._13,m_matInstanceMatrix._14,
							  m_matInstanceMatrix._21,m_matInstanceMatrix._22,m_matInstanceMatrix._23,m_matInstanceMatrix._24,
							  m_matInstanceMatrix._31,m_matInstanceMatrix._32,m_matInstanceMatrix._33,m_matInstanceMatrix._34,
							  m_matInstanceMatrix._41,m_matInstanceMatrix._42,m_matInstanceMatrix._43,m_matInstanceMatrix._44 };




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

		if (arrView == nullptr ||
			arrProj == nullptr ||
			arrWorld == nullptr)
			return;

		ImGuizmo::Manipulate(arrView, arrProj, InstanceCurrentGizmoOperation, InstanceCurrentGizmoMode, arrWorld, NULL, InstanceuseSnap ? &snap[0] : NULL);
		

		XMFLOAT4X4 matW = { arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3],
						   arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7],
						   arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11],
						   arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15] };

		pInstance->Set_Matrix(XMLoadFloat4x4(&matW));
		

		m_vecCreateInstance[iIndex]->Update(*pInstance, m_iSelectInstanceIndex);

		if (ImGuizmo::IsOver())
		{
			int a = 0;
		}
}

void CWindow_MapTool::Character_SelectFunction()
{
	if (m_pPreviewCharacter != nullptr)
	{
		m_pPreviewCharacter->Set_Dead(true);
		m_pPreviewCharacter = nullptr;
	}


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