#include "stdafx.h"
#include "Window_MapTool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

#include "Environment_Object.h"
#include "Environment_Interact.h"
#include "Environment_Instance.h"
#include "Environment_LightObject.h"
#include "Environment_SpecialObject.h"
#include "Light.h"
#include "Effect.h"
#include "Event_MonsterSpawnTrigger.h"

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
#include "UI.h"

#include "Navigation.h"
#include "Cell.h"
#include <chrono>
#include <iomanip>

#include "../../Reference/Public/Delaunator/delaunator.hpp"



#include "DebugDraw.h"
#include "../../Reference/Public/DebugDraw.cpp"
#include "Window_AnimTool.h"


//#include "../../Reference/Public/dEBUG"
//#include "DebugDraw.h"

static ImGuizmo::OPERATION InstanceCurrentGizmoOperation;
static ImGuizmo::MODE	   InstanceCurrentGizmoMode;
static bool InstanceuseSnap(false);
static bool InstanceuseSnapUI(false);

static ImGuizmo::OPERATION TriggerCurrentGizmoOperation;
static ImGuizmo::MODE	   TriggerCurrentGizmoMode;
static bool TriggeruseSnap(false);
static bool TriggeruseSnapUI(false);


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
	size_t	iShaderCodeLength = { 0 };

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
	
	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 120), ImGuiChildFlags_Border, WindowFlag);
	
	ImGui::SeparatorText(u8"세이브 / 로드");
	{
		
		if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG; m_strDialogPath = "../Bin/DataFiles/Data_Map/"; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DataFiles/Data_Map/";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
	}ImGui::Separator(); 

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Create_RightChild", ImVec2(0, 120), ImGuiChildFlags_Border, WindowFlag);
	
	FieldWindowMenu(); //! 필드 창 보이기 감추기

	if (ImGui::Button(u8"테스트버튼"))
	{
		CWindow_AnimTool* pAnimTool = dynamic_cast<CWindow_AnimTool*>(CImgui_Manager::GetInstance()->Find_Window(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_ANIMATIONTOOL_WINDOW));
		pAnimTool->Call_SeungYongButton();

		

		string strFilePath = "..\\Bin\\DataFiles\\Data_Map";
		string strFileName = "SnowMountainInteractNavi_MapData.json";
		Load_Function(strFilePath, strFileName);
		LoadNavi("..\\Bin\\DataFiles\\Navigation\\SnowMountainNavigation5.dat");
		
		if (nullptr != m_pGameInstance->Get_Player())
		{
			m_bCreateCamera = true;
			m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());
			m_pPlayer->Set_Navigation(m_pNavigation);
		}

		
		
	}

	ImGui::EndChild();

	CameraWindow_Function();

	ImGui::SeparatorText(u8"오브젝트 타입");
	{

		static _int iObjectType = 0;
		const char* CharObjectType[4] = { u8"환경", u8"캐릭터", u8"네비게이션", u8"트리거"};

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


	if (false == m_mapNonAnimModelTag.empty() || false == m_mapAnimModelTag.empty())
	{
		if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
		{
			EnvironmentMode_Function();
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER) //! OBJECTMODE_CHARACTER
		{
			CharacterMode_Function();
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_NAVIGATION)
		{
			NavigationMode_Function();
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_TRIGGER)
		{
			TriggerMode_Function();
		}
	}
	else
	{
		if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_NAVIGATION)	
		{
			NavigationMode_Function();
		}
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
	
	if(m_pNavigation != nullptr)
		m_pGameInstance->Add_DebugRender(m_pNavigation);
}

void CWindow_MapTool::Render()
{
	if (false == m_vecPickingPoints.empty() && nullptr != m_pBatch)
	{
		_int iPickingPointSize = m_vecPickingPoints.size();

		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		m_pEffect->Apply(m_pContext);
		m_pContext->IASetInputLayout(m_pInputLayOut);

		m_pBatch->Begin();

		for (_int i = 0; i < iPickingPointSize; ++i)
		{
			/* m_pAABB가 월드스페이스 상의 정보다. */
			if (m_vecPickingPoints[i] != nullptr)
			{

				DX::Draw(m_pBatch, *m_vecPickingPoints[i], XMVectorSet(1.f, 0.f, 0.f, 1.f));
			}
			else
				break;
		}
		m_pBatch->End();
	}
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
				json UpdateCellJson = {};

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
				InteractJson[i].emplace("UseGravity", Desc.bUseGravity);
				InteractJson[i].emplace("SplineJsonPath", Desc.strSplineJsonPath);
				InteractJson[i].emplace("EnableJsonPath", Desc.strEnableJsonPath);
				
				InteractJson[i].emplace("InteractGroupIndex", Desc.iInteractGroupIndex);				
				InteractJson[i].emplace("RotationAngle", Desc.fRotationAngle);
				InteractJson[i].emplace("RotationSpeed", Desc.fRotationSpeed);
				InteractJson[i].emplace("Offset", Desc.bOffset);
				InteractJson[i].emplace("Rotate", Desc.bRotate);
				InteractJson[i].emplace("Owner", Desc.bOwner);
				InteractJson[i].emplace("RootTranslate", Desc.bRootTranslate);
				InteractJson[i].emplace("Arrival", Desc.bArrival);
				InteractJson[i].emplace("Enable", Desc.bEnable);

				

				CJson_Utility::Write_Float4(InteractJson[i]["EnablePosition"], Desc.vEnablePosition);
				CJson_Utility::Write_Float4(InteractJson[i]["ArrivalPosition"], Desc.vArrivalPosition);
				CJson_Utility::Write_Float4(InteractJson[i]["OffsetPosition"], Desc.vOffset);

				CJson_Utility::Write_Float3(InteractJson[i]["RootMoveRate"], Desc.vPlayerRootMoveRate);

				CJson_Utility::Write_Float3(InteractJson[i]["ColliderSize"], Desc.vColliderSize);
				CJson_Utility::Write_Float3(InteractJson[i]["ColliderCenter"], Desc.vColliderCenter);
				CJson_Utility::Write_Float3(InteractJson[i]["MoveColliderSize"], Desc.vMoveRangeColliderSize);
				CJson_Utility::Write_Float3(InteractJson[i]["MoveColliderCenter"], Desc.vMoveRangeColliderCenter);
				m_vecCreateInteractObject[i]->Write_Json(InteractJson[i]);

				vector<_int> vecUpdateCellIndexs = m_vecCreateInteractObject[i]->Get_UpdateCellIndexs();
				_int iUpdateCellSize = vecUpdateCellIndexs.size();

				for (_int i = 0; i < iUpdateCellSize; ++i)
				{
					UpdateCellJson[i].emplace("UpdateCellIndex", vecUpdateCellIndexs[i]);
				}

				InteractJson[i].emplace("UpdateCellJson", UpdateCellJson);
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
				MonsterJson[i].emplace("MonsterGroupIndex", m_vecCreateMonster[i]->Get_MonsterGroupIndex());
				MonsterJson[i].emplace("StartNaviIndex", Desc.iStartNaviIndex);
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
					LightObjectJson[i].emplace("SpecialGroupIndex", LightObjectDesc.iSpecialGroupIndex);

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

		json TriggerJson;

		json MonsterTriggerJson;
		

		if (false == m_vecCreateMonsterTrigger.empty())
		{
			_int iCreateMonsterTriggerSize = (_int)m_vecCreateMonsterTrigger.size();

			for (_int i = 0; i < iCreateMonsterTriggerSize; ++i)
			{
				CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC MonsterTriggerDesc = *m_vecCreateMonsterTrigger[i]->Get_MonsterTriggerDesc();
				
				MonsterTriggerJson[i].emplace("OnTrigger", MonsterTriggerDesc.bOnTrigger);
				string strSavePath = strPath + "/" + strNoExtFileName + "_MapData.json";
				MonsterTriggerJson[i].emplace("JsonPath", strSavePath);
				MonsterTriggerJson[i].emplace("NameTag", MonsterTriggerDesc.strTriggerNameTag);
				MonsterTriggerJson[i].emplace("SpawnGroupIndex", MonsterTriggerDesc.iSpawnGroupIndex);
				CJson_Utility::Write_Float3(MonsterTriggerJson[i]["ColliderSize"], MonsterTriggerDesc.vColliderSize);
				CJson_Utility::Write_Float3(MonsterTriggerJson[i]["ColliderCenter"], MonsterTriggerDesc.vColliderCenter);


				m_vecCreateMonsterTrigger[i]->Write_Json(MonsterTriggerJson[i]);
				
			}

			TriggerJson.emplace("MonsterTriggerJson", MonsterTriggerJson);
		}

		json SpecialJson;

		if (false == m_vecCreateSpecialObject.empty())
		{
			_int iCreateSpecialSize = (_int)m_vecCreateSpecialObject.size();

			for (_int i = 0; i < iCreateSpecialSize; ++i)
			{
				CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc = *m_vecCreateSpecialObject[i]->Get_EnvironmentDesc();

				SpecialJson[i].emplace("iShaderPassIndex", SpecialDesc.iShaderPassIndex);

				string strModelTag;
				m_pGameInstance->WString_To_String(m_vecCreateSpecialObject[i]->Get_ModelTag(), strModelTag);
				SpecialJson[i].emplace("ModelTag", strModelTag);
				m_vecCreateSpecialObject[i]->Write_Json(SpecialJson[i]);

				SpecialJson[i].emplace("SpecialType", SpecialDesc.eSpecialType);
				SpecialJson[i].emplace("AnimType", SpecialDesc.bAnimModel);
				SpecialJson[i].emplace("PlayAnimationIndex", SpecialDesc.iPlayAnimationIndex);
				SpecialJson[i].emplace("SpecialGroupIndex", SpecialDesc.iSpecialGroupIndex);
				SpecialJson[i].emplace("BloomMeshIndex", SpecialDesc.iBloomMeshIndex);
			}
		}
		

		SaveJson.emplace("Monster_Json", MonsterJson);
		SaveJson.emplace("Trigger_Json", TriggerJson);
		SaveJson.emplace("Basic_Json", BasicJson);
		SaveJson.emplace("Interact_Json", InteractJson);
		SaveJson.emplace("Instance_Json", InstanceJson);
		SaveJson.emplace("Light_Json", LightJson);
		SaveJson.emplace("LightObject_Json", LightObjectJson);
		SaveJson.emplace("Special_Json", SpecialJson);



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
			
			//Desc.strSplineJsonPath = InteractJson[i]["SplineJsonPath"];
			Desc.bEnable = InteractJson[i]["Enable"];
			Desc.strEnableJsonPath = InteractJson[i]["EnableJsonPath"];
			Desc.iInteractGroupIndex = InteractJson[i]["InteractGroupIndex"];
			Desc.bOffset = InteractJson[i]["Offset"];
			Desc.bOwner = InteractJson[i]["Owner"];
			Desc.bRootTranslate = InteractJson[i]["RootTranslate"];
			Desc.bRotate = InteractJson[i]["Rotate"];
			Desc.fRotationAngle = InteractJson[i]["RotationAngle"];
			Desc.fRotationSpeed = InteractJson[i]["RotationSpeed"];
			Desc.bArrival = InteractJson[i]["Arrival"];
			

			 Desc.bUseGravity = InteractJson[i]["UseGravity"];
			 CJson_Utility::Load_Float3(InteractJson[i]["RootMoveRate"], Desc.vPlayerRootMoveRate);

			CJson_Utility::Load_Float3(InteractJson[i]["ColliderSize"], Desc.vColliderSize);
			CJson_Utility::Load_Float3(InteractJson[i]["ColliderCenter"], Desc.vColliderCenter);

			CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderSize"], Desc.vMoveRangeColliderSize);
			CJson_Utility::Load_Float3(InteractJson[i]["MoveColliderCenter"], Desc.vMoveRangeColliderCenter);

			CJson_Utility::Load_Float4(InteractJson[i]["OffsetPosition"], Desc.vOffset);
			CJson_Utility::Load_Float4(InteractJson[i]["EnablePosition"], Desc.vEnablePosition);
			CJson_Utility::Load_Float4(InteractJson[i]["ArrivalPosition"], Desc.vArrivalPosition);
				
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

			json UpdateCellJson = InteractJson[i]["UpdateCellJson"];
			_int iUpdateCellJsonSize = UpdateCellJson.size();

			for (_int i = 0; i < iUpdateCellJsonSize; ++i)
			{
				Desc.vecUpdateCellIndex.push_back(UpdateCellJson[i]["UpdateCellIndex"]);
			}
			
			
			CEnvironment_Interact* pObject = { nullptr };

			pObject = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_InteractObject", &Desc));

			m_vecCreateInteractObject.push_back(pObject);
			m_vecCreateInteractIndex++;
		}
		
		_int iInteractObjectSize = (_int)m_vecCreateInteractObject.size();

		CEnvironment_Interact* pOwnerObject = nullptr;

		for (_int i = 0; i < iInteractObjectSize; ++i)
		{
			CEnvironment_Interact* pSearchObject = m_vecCreateInteractObject[i];
			CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC FindInteractInfo = *pSearchObject->Get_EnvironmentDesc();

			if (FindInteractInfo.bOwner == true)
			{
				pOwnerObject = pSearchObject;
			}

			if (pOwnerObject != nullptr)
			{
				CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC OwnerInteractInfo = *pOwnerObject->Get_EnvironmentDesc();

				for (_int j = 0; j < iInteractObjectSize; ++j)
				{
					CEnvironment_Interact* pSearchObject = m_vecCreateInteractObject[j];
					CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC FindInteractInfo = *pSearchObject->Get_EnvironmentDesc();

					if (FindInteractInfo.bOwner == false && FindInteractInfo.iInteractGroupIndex == OwnerInteractInfo.iInteractGroupIndex)
					{
						pSearchObject->Set_OwnerObject(pOwnerObject);
					}
				}
			}
			
		}




		//if (pOwnerObject == nullptr)
		//	MSG_BOX("오너를 찾지 못했습니다.");
		//else
		//	m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_OwnerObject(pOwnerObject);

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

		//TODO  트리거 추가하면서 주석처리
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
 			MonsterDesc.iMonsterGroupIndex = MonsterJson[i]["MonsterGroupIndex"];
			MonsterDesc.iStartNaviIndex = MonsterJson[i]["StartNaviIndex"];
 
 
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
			LightObjectDesc.iSpecialGroupIndex = LightObjectJson[i]["SpecialGroupIndex"];
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

		json TriggerJson = LoadJson["Trigger_Json"];



		json MonsterTriggerJson = TriggerJson["MonsterTriggerJson"];
		_int iMonsterTriggerJsonSize = (_int)MonsterTriggerJson.size();

		for (_int i = 0; i < iMonsterTriggerJsonSize; ++i)
		{
			CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC MonsterTriggerDesc = {};
			MonsterTriggerDesc.bOnTrigger = MonsterTriggerJson[i]["OnTrigger"];
			MonsterTriggerDesc.strSpawnMonsterJsonPath = MonsterTriggerJson[i]["JsonPath"];
			MonsterTriggerDesc.strTriggerNameTag = MonsterTriggerJson[i]["NameTag"];
			MonsterTriggerDesc.iSpawnGroupIndex = MonsterTriggerJson[i]["SpawnGroupIndex"];
			CJson_Utility::Load_Float3(MonsterTriggerJson[i]["ColliderSize"], MonsterTriggerDesc.vColliderSize);
			CJson_Utility::Load_Float3(MonsterTriggerJson[i]["ColliderCenter"], MonsterTriggerDesc.vColliderCenter);

			CEvent_MosnterSpawnTrigger* pMonsterTrigger = CEvent_MosnterSpawnTrigger::Create(m_pDevice, m_pContext, &MonsterTriggerDesc);

// 
// 			const json& TransformJson = MonsterTriggerJson[i]["Component"]["Transform"];
// 			_float4x4 WorldMatrix;
// 
// 			for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
// 			{
// 				for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
// 				{
// 					WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
// 				}
// 			}

			pMonsterTrigger->Load_FromJson(MonsterTriggerJson[i]);

			if (pMonsterTrigger == nullptr)
			{
				MSG_BOX("몬스터 트리거 불러오기 실패");
				return E_FAIL;
			}
			else
			{
				m_vecCreateMonsterTrigger.push_back(pMonsterTrigger);
				m_vecCreateMonsterTriggerTag.push_back(MonsterTriggerDesc.strTriggerNameTag);
			}


		}

		json SpecialJson = LoadJson["Special_Json"];
		_int iSpecialJsonSize = (_int)SpecialJson.size();

		for (_int i = 0; i < iSpecialJsonSize; ++i)
		{
			CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc = {};

			SpecialDesc.iShaderPassIndex =		SpecialJson[i]["iShaderPassIndex"];
			SpecialDesc.bAnimModel =			SpecialJson[i]["AnimType"];
			SpecialDesc.iPlayAnimationIndex =	SpecialJson[i]["PlayAnimationIndex"];
			SpecialDesc.iSpecialGroupIndex =	SpecialJson[i]["SpecialGroupIndex"];
			SpecialDesc.eSpecialType =			SpecialJson[i]["SpecialType"];
			//TODOSpecialDesc.iBloomMeshIndex =		SpecialJson[i]["BloomMeshIndex"];
			SpecialDesc.bPreview = false;
			

			m_pGameInstance->String_To_WString((string)SpecialJson[i]["ModelTag"], SpecialDesc.strModelTag);

			const json& TransformJson = SpecialJson[i]["Component"]["Transform"];
			_float4x4 WorldMatrix;

			for (_int TransformLoopIndex = 0; TransformLoopIndex < 4; ++TransformLoopIndex)
			{
				for (_int TransformSecondLoopIndex = 0; TransformSecondLoopIndex < 4; ++TransformSecondLoopIndex)
				{
					WorldMatrix.m[TransformLoopIndex][TransformSecondLoopIndex] = TransformJson[TransformLoopIndex][TransformSecondLoopIndex];
				}
			}

			SpecialDesc.WorldMatrix = WorldMatrix;

			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &SpecialDesc));

			if (pSpecialObject == nullptr)
			{
				MSG_BOX("스페셜오브젝트 생성실패");
			}

			m_vecCreateSpecialObject.push_back(pSpecialObject);

			wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pSpecialObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateSpecialObjectIndex));
			string strConvertTag;
			m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
			m_vecCreateSpecialObjectTag.push_back(strConvertTag);

			m_iCreateSpecialObjectIndex++;

		}


	}


		


			

	return S_OK;
}

void CWindow_MapTool::Reset_Function()
{

	if (m_pPreviewObject != nullptr)
		m_pPreviewObject->Set_Dead(true);
	
	if(m_pPreviewCharacter != nullptr)
		m_pPreviewCharacter->Set_Dead(true);

	if(m_pPreviewInteract != nullptr)
		m_pPreviewInteract->Set_Dead(true);

	if(m_pPreviewLightObject != nullptr)
		m_pPreviewLightObject->Set_Dead(true);

	if(m_pPreviewSpecialObject != nullptr)
		m_pPreviewSpecialObject->Set_Dead(true);

	m_pPickingObject = nullptr;
	m_pPickingInstanceInfo = nullptr;
	m_pPickingTrigger = nullptr;


	m_pPreviewObject = nullptr;
	m_pPreviewCharacter = nullptr;
	m_pPreviewInteract = nullptr;
	m_pPreviewLightObject = nullptr;
	m_pPreviewSpecialObject = nullptr;


		

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
	
	m_mapSplinePoints.clear();
	m_mapSplineSpeeds.clear();
	m_mapSplineListBox.clear();
	ZeroMemory(m_strSplinePointKeyTag, sizeof(m_strSplinePointKeyTag));
	m_vecSplinePoints.clear();
	m_vecSplineListBox.clear();

	m_iSplineDivergingCount = 0;
	m_iSplinePickingIndex = 0;
	m_iSplineListIndex = 0;

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
	
	
	_int iCreateMonsterTriggerSize = (_int)m_vecCreateMonsterTrigger.size();

	for (_int i = 0; i < iCreateMonsterTriggerSize; ++i)
	{
		Safe_Release(m_vecCreateMonsterTrigger[i]);
	}

	m_iSelectMonsterTriggerIndex = 0;
	m_iSelectMonsterSpawnGroupIndex = 0;
	m_vecCreateMonsterTrigger.clear();
	m_vecCreateMonsterTriggerTag.clear();


	_int iCreateSpecialSize = (_int)m_vecCreateSpecialObject.size();

	for (_int i = 0; i < iCreateSpecialSize; ++i)
	{
		m_vecCreateSpecialObject[i]->Set_Dead(false);
	}

	m_iCreateSpecialObjectIndex = 0;
	m_iSelectSpecialObjectIndex = 0;
	m_vecCreateSpecialObject.clear();
	m_vecCreateSpecialObjectTag.clear();



	m_eSpecialType = 0;
	m_iSpecialGroupIndex = 0;
}

void CWindow_MapTool::ObjectMode_Change_For_Reset()
{
	if (m_bChangeObjectMode == true)
	{
		if (m_eObjectMode == OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
		{
			m_iSelectCharacterTag = 0;
			m_pPickingObject = nullptr;
			m_iSelectModelTag = 0;

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
			m_iSelectModelTag = 0;

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
	//m_vecMonsterTag.push_back("Prototype_GameObject_Bandit_Heavy");
	m_vecMonsterTag.push_back("Prototype_GameObject_Bandit_Sniper");
	m_vecMonsterTag.push_back("Prototype_GameObject_Heavy_Vampiric_Zombie");
	m_vecMonsterTag.push_back("Prototype_GameObject_Tank");


	m_vecMonsterTag.push_back("Prototype_GameObject_VampireCommander");
	m_vecMonsterTag.push_back("Prototype_GameObject_Mother");
	m_vecMonsterTag.push_back("Prototype_GameObject_Son");
	
	
		
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
		string strFileName = "Stage1Final_MapData.json";
		Load_Function(strFilePath, strFileName);
		LoadNavi("..\\Bin\\DataFiles\\Navigation\\Stage1NavigationFinal.dat");
	}
	
	//ImGui::SameLine();
	//
	//if (ImGui::Button(u8"인트로보스맵 불러오기"))
	//{
	//	string strFilePath = "..\\Bin\\DataFiles\\Data_Map";
	//	string strFileName = "Stage1Boss_MapData_MapData.json";
	//	Load_Function(strFilePath, strFileName);
	//	LoadNavi("..\\Bin\\DataFiles\\Navigation\\IntroBossNaviFinal.dat");
	//}
	//
	//ImGui::SameLine();

	if (ImGui::Button(u8"설산맵 불러오기"))
	{
		string strFilePath = "..\\Bin\\DataFiles\\Data_Map";
		string strFileName = "SnowMountainRetrack_MapData_MapData.json";
		Load_Function(strFilePath, strFileName);
		LoadNavi("..\\Bin\\DataFiles\\Navigation\\Stage1NavigationFinal.dat");
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

		if (ImGui::BeginTabItem(u8"스페셜"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_SPECIAL)
			{
				m_iSelectSpecialObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_SPECIAL;
			SpecialTab_Function();

			ImGui::EndTabItem();
		}

	
		if (ImGui::BeginTabItem(u8"인스턴스 환경"))
		{
			if (m_eTabType != CWindow_MapTool::TAP_TYPE::TAB_INSTANCE)
			{
				m_iSelectObjectIndex = 0;
				m_iSelectModelTag = 0;
			}

			m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_INSTANCE;
			InstanceTab_Function();

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

			if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DataFiles/Data_Map/Navigation";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
		}
		else
		{
			if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG;  m_strDialogPath = "../Bin/DataFiles/Data_Map/Navigation"; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DataFiles/Data_Map/Navigation";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
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

			if (ImGui::Button(u8"플레이어 카메라 위치이동"))
			{
				
				m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_CamPosition());
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

void CWindow_MapTool::TriggerMode_Function()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;

// 	ImGui::SeparatorText(u8"트리거 세이브 / 로드");
// 	{
// 		if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG;  m_strDialogPath = "../Bin/DataFiles/Data_Map/Navigation"; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_strDialogPath = "../Bin/DataFiles/Data_Map/Navigation";  m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
// 	}ImGui::Separator();

	if (ImGui::BeginTabBar(u8"트리거 모드 타입", tab_bar_flags))
	{

		if (ImGui::BeginTabItem(u8"트리거 생성"))
		{
			Trigger_CreateTab();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"트리거 선택"))
		{
			Trigger_SelectTab();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"트리거 삭제"))
		{
			Trigger_DeleteTab();

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
				Ground_CreateTab();
				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
			{
				Ground_SelectTab();
				
				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
			{
				Ground_DeleteTab();
				break;
			}
	}
	
}

void CWindow_MapTool::Ground_CreateTab()
{

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	Select_PickingType();

	_uint iModelTagSize;
	vector<string> vecModelTag;

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
		

	ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);


	if (ImGui::BeginListBox(u8"일반오브젝트모델 태그", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iModelTagSize; ++i)
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

	
		if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
		{
			if (ImGui::InputInt(u8"애니메이션 인덱스", &m_iAnimIndex))
			{
				m_pPreviewObject->Set_AnimationIndex(m_iAnimIndex);
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

	if (ImGui::Button(u8"방금 생성한 오브젝트와 같은 값으로 생성하기"))
	{

		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc = *m_vecCreateObject[m_iCreateObjectIndex - 1]->Get_EnvironmentDesc();

		Desc.WorldMatrix = m_vecCreateObject[m_iCreateObjectIndex - 1]->Get_Transform()->Get_WorldFloat4x4();

		CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

		m_vecCreateObject.push_back(pObject);


		wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateObjectIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
		m_vecCreateObjectTag.push_back(strConvertTag);

		m_iCreateObjectIndex++;
	}
}

void CWindow_MapTool::Ground_SelectTab()
{
	_uint iObjectTagSize = (_uint)m_vecCreateObject.size();


	if (true == m_vecCreateObject.empty())
	{
		ImGui::Text(u8"생성한 객체가 없습니다. ");
		return;
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

					CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc = *m_vecCreateObject[m_iSelectObjectIndex]->Get_EnvironmentDesc();

					m_pPickingObject = m_vecCreateObject[m_iSelectObjectIndex];
					m_iShaderPassIndex = Desc.iShaderPassIndex;

					if (Desc.bAnimModel == true)
					{
						m_iAnimIndex = m_vecCreateObject[m_iSelectObjectIndex]->Get_AnimationIndex();
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}
	}

	if (ImGui::InputInt(u8"셰이더패스 인덱스", &m_iShaderPassIndex))
	{
		m_vecCreateObject[m_iSelectObjectIndex]->Set_ShaderPassIndex(m_iShaderPassIndex);
	}

	if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM && m_vecCreateObject[m_iSelectObjectIndex]->Get_EnvironmentDesc()->bAnimModel == true)
	{
		if (ImGui::InputInt(u8"애니메이션 인덱스", &m_iAnimIndex))
		{
			m_vecCreateObject[m_iSelectObjectIndex]->Set_AnimationIndex(m_iAnimIndex);
		}

	}

	if (ImGui::Button(u8"방금 생성한 오브젝트와 같은 값으로 생성하기"))
	{

		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc = *m_vecCreateObject[m_iCreateObjectIndex - 1]->Get_EnvironmentDesc();

		Desc.WorldMatrix = m_vecCreateObject[m_iCreateObjectIndex - 1]->Get_Transform()->Get_WorldFloat4x4();

		CEnvironment_Object* pObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_Object", &Desc));

		m_vecCreateObject.push_back(pObject);


		wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateObjectIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
		m_vecCreateObjectTag.push_back(strConvertTag);

		m_iCreateObjectIndex++;
	}

	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		_bool bChange = false;

		if (iObjectTagSize - 1 > (_uint)m_iSelectObjectIndex)
		{
			bChange = true;
			m_iSelectObjectIndex++;
		}
		else
		{
			bChange = true;
			m_iSelectObjectIndex = 0;
		}


		if (bChange == true)
		{
			m_pPickingObject = m_vecCreateObject[m_iSelectObjectIndex];

			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc = *m_vecCreateObject[m_iSelectObjectIndex]->Get_EnvironmentDesc();
			m_iShaderPassIndex = Desc.iShaderPassIndex;


			if (Desc.bAnimModel == true)
			{
				m_iAnimIndex = m_vecCreateObject[m_iSelectObjectIndex]->Get_AnimationIndex();
			}
		}
		
	}

	if (m_pGameInstance->Key_Down(DIK_END))
	{
		_bool bChange = false;

		if (0 < m_iSelectObjectIndex)
		{
			bChange = true;
			m_iSelectObjectIndex--;
		}
		else
		{
			bChange = true;
			m_iSelectObjectIndex = iObjectTagSize - 1;
		}


		if (bChange == true)
		{
			m_pPickingObject = m_vecCreateObject[m_iSelectObjectIndex];

			CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc = *m_vecCreateObject[m_iSelectObjectIndex]->Get_EnvironmentDesc();
			m_iShaderPassIndex = Desc.iShaderPassIndex;


			if (Desc.bAnimModel == true)
			{
				m_iAnimIndex = m_vecCreateObject[m_iSelectObjectIndex]->Get_AnimationIndex();
			}
		}
	}


	Guizmo_Tick(m_pPickingObject);
}

void CWindow_MapTool::Ground_DeleteTab()
{
	_uint iTagSize = 0;
	vector<string> vecCreateTag;
	_uint iSelectTag = 0;

	string strListBoxName = u8"";

	iTagSize = (_uint)m_vecCreateObject.size();
	vecCreateTag = m_vecCreateObjectTag;
	strListBoxName = u8"삭제할 환경 객체 리스트";
	iSelectTag = m_iSelectObjectIndex;
		

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iTagSize; ++i)
		{
			const _bool isSelected = (iSelectTag == i);

			if (ImGui::Selectable(vecCreateTag[i].c_str(), isSelected))
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


	if (m_vecCreateObject.size() < m_iSelectObjectIndex)
		return;

	
	if (false == m_vecCreateObject.empty() && m_vecCreateObject[m_iSelectObjectIndex] != nullptr)
	{
		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_Guizmo(m_vecCreateObject[m_iSelectObjectIndex]);
	}

	if (ImGui::Button(u8"삭제"))
	{
	  m_vecCreateObject[m_iSelectObjectIndex]->Set_Dead(true);
	  m_vecCreateObject[m_iSelectObjectIndex] = nullptr;
	  m_pPickingObject = nullptr;
	  m_vecCreateObject.erase(m_vecCreateObject.begin() + m_iSelectObjectIndex);
	  m_vecCreateObjectTag.erase(m_vecCreateObjectTag.begin() + m_iSelectObjectIndex);
	  m_iCreateObjectIndex--;

	  if(m_iSelectObjectIndex > 0)
		m_iSelectObjectIndex--;
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
				m_tEditLightDesc.eType = (LIGHT_DESC::TYPE)eLightType;
				
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
				if (m_pPreviewLightObject != nullptr)
				{
					_float4 vObjectPos = m_pPreviewLightObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);
					m_pPreviewLightObject->Set_LightPos(_float3(vObjectPos.x, vObjectPos.y, vObjectPos.z));
					m_tEditLightDesc.vPosition = _float3(vObjectPos.x, vObjectPos.y, vObjectPos.z);
				}

				if (ImGui::InputFloat(u8"라이트 레인지", &m_tEditLightDesc.fRange))
				{
					LIGHT_DESC OriginDesc = m_pPreviewLightObject->Get_LightDesc();
					OriginDesc.fRange = m_tEditLightDesc.fRange;

					m_pPreviewLightObject->Set_LightDesc(OriginDesc);

				}
			}

			ImGui::Checkbox(u8"활성화", &m_tEditLightDesc.bEnable);

			ImGui::NewLine();

			if (ImGui::ColorEdit4(u8"라이트 디퓨즈", &m_tEditLightDesc.vDiffuse.x))
			{
				m_pPreviewLightObject->Set_Diffuse(m_tEditLightDesc.vDiffuse);
			}

			if (ImGui::ColorEdit4(u8"라이트 스페큘러", &m_tEditLightDesc.vSpecular.x))
			{
				m_pPreviewLightObject->Set_Specular(m_tEditLightDesc.vSpecular);
			}

			if (ImGui::ColorEdit4(u8"라이트 앰비언트", &m_tEditLightDesc.vAmbient.x))
			{
				m_pPreviewLightObject->Set_Ambient(m_tEditLightDesc.vAmbient);
			}

			if (iCreateLightType == 1)
			{
				if (ImGui::Checkbox(u8"이펙트부여", &m_bLightEffect))
				{
					m_pPreviewLightObject->Set_EffectEnable(m_bLightEffect);
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
							}

							if (true == is_Selected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}
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

	_uint iObjectTagSize = 0;
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


	if (iSelectLightType == 0)
	{
			iObjectTagSize = (_uint)m_vecCreateLightTag.size();

		if (true == m_vecCreateLight.empty())
		{
			ImGui::Text(u8"생성한 라이트 객체가 없습니다. ");
			return;
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

						m_tEditLightDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();
						m_eLightType = m_tEditLightDesc.eType;

						
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
				wstring lightType = L"";

				if (m_eLightType == LIGHT_DESC::TYPE_DIRECTIONAL)
				{
					lightType = L"Direction";
				}
				else if (m_eLightType == LIGHT_DESC::TYPE_POINT)
				{
					lightType = L"Point";
				}
				else if(m_eLightType == LIGHT_DESC::TYPE_SPOTLIGHT)
				{
					lightType = L"SpotLight";
				}
				

				ImGui::Text("Current Light Type : %s", lightType.c_str());

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
					m_eLightType = eLightType;
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
		iObjectTagSize = (_uint)m_vecCreateLightObjectTag.size();

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
						

						CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC Desc = {};
						Desc = *m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_EnvironmentDesc();

						m_tEditLightDesc = Desc.LightDesc;
						m_iSpecialGroupIndex = Desc.iSpecialGroupIndex;
						m_iShaderPassIndex = Desc.iShaderPassIndex;

						if (Desc.bAnimModel == true)
						{
							m_iAnimIndex = Desc.iPlayAnimationIndex;
						}

						CEffect* pLightEffect = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_Effect();

						if (pLightEffect != nullptr)
						{
							m_vLightEffectPos = pLightEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
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

			if (ImGui::InputInt(u8"스페셜그룹인덱스", &m_iSpecialGroupIndex))
			{
				m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_SpecialGroupIndex(m_iSpecialGroupIndex);
			}

			if (m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_EnvironmentDesc()->bAnimModel == true)
			{
				ImGui::SameLine();

				if (ImGui::InputInt(u8"애니메이션 인덱스", &m_iAnimIndex))
				{
					m_vecCreateLightObject[m_iSelectLightObjectIndex]->Set_AnimationIndex(m_iAnimIndex);
				}
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

	
	if (iSelectLightType == 0)
	{
		_bool bChange = false;

		if (m_pGameInstance->Key_Down(DIK_HOME))
		{
			if (iObjectTagSize - 1 > (_uint)m_iSelectLightIndex)
			{
				m_iSelectLightIndex++;
				bChange = true;
			}
			else
			{
				m_iSelectLightIndex = 0;
				bChange = true;
			}

			if (bChange == true)
			{
				m_tEditLightDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();
				m_eLightType = m_tEditLightDesc.eType;
			}
		}

		if (m_pGameInstance->Key_Down(DIK_END))
		{
		
			if (0 < m_iSelectLightIndex)
			{
				m_iSelectLightIndex--;
				bChange = true;
			}
			else
			{
				m_iSelectLightIndex = iObjectTagSize - 1;
				bChange = true;
			}

			if (bChange == true)
			{
				m_tEditLightDesc = m_vecCreateLight[m_iSelectLightIndex]->Get_LightDesc();
				m_eLightType = m_tEditLightDesc.eType;
			}
		}
	}
	else if (iSelectLightType == 1)
	{
		_bool bChange = false;


		if (m_pGameInstance->Key_Down(DIK_HOME))
		{
			if (iObjectTagSize - 1 > (_uint)m_iSelectLightObjectIndex)
			{
				m_iSelectLightObjectIndex++;
				bChange = true;
			}
			else
			{
				m_iSelectLightObjectIndex = 0;
				bChange = true;
			}

			if (bChange == true)
			{
				CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC Desc = {};
				Desc = *m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_EnvironmentDesc();

				m_tEditLightDesc = Desc.LightDesc;
				m_iSpecialGroupIndex = Desc.iSpecialGroupIndex;
				m_iShaderPassIndex = Desc.iShaderPassIndex;

				if (Desc.bAnimModel == true)
				{
					m_iAnimIndex = Desc.iPlayAnimationIndex;
				}

				CEffect* pLightEffect = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_Effect();

				if (pLightEffect != nullptr)
				{
					m_vLightEffectPos = pLightEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				}


				m_pPickingObject = m_vecCreateLightObject[m_iSelectLightObjectIndex];
			}
		}

		if (m_pGameInstance->Key_Down(DIK_END))
		{
			if (0 < m_iSelectObjectIndex)
			{
				m_iSelectLightObjectIndex--;
				bChange = true;
			}
			else
			{
				bChange = true;
				m_iSelectLightObjectIndex = iObjectTagSize - 1;
			}

			if (bChange == true)
			{
				CEnvironment_LightObject::ENVIRONMENT_LIGHTOBJECT_DESC Desc = {};
				Desc = *m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_EnvironmentDesc();

				m_tEditLightDesc = Desc.LightDesc;
				m_iSpecialGroupIndex = Desc.iSpecialGroupIndex;
				m_iShaderPassIndex = Desc.iShaderPassIndex;

				if (Desc.bAnimModel == true)
				{
					m_iAnimIndex = Desc.iPlayAnimationIndex;
				}

				CEffect* pLightEffect = m_vecCreateLightObject[m_iSelectLightObjectIndex]->Get_Effect();

				if (pLightEffect != nullptr)
				{
					m_vLightEffectPos = pLightEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				}


				m_pPickingObject = m_vecCreateLightObject[m_iSelectLightObjectIndex];
			}
		}
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
			Interact_SelectTab();
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
		const char* InteractTypes[] =
		{
			"INTERACT_JUMP100",
			"INTERACT_JUMP200",
			"INTERACT_JUMP300",
			"INTERACT_VAULT100",
			"INTERACT_VAULT200",
			"INTERACT_WAGONPUSH",
			"INTERACT_WAGONJUMP",
			"INTERACT_WAGONEVENT",
			"INTEARCT_WAGONROPEJUMP",
			"INTERACT_CLIMB100",
			"INTERACT_CLIMB200",
			"INTERACT_CLIMB300",
			"INTERACT_CLIMB450",
			"INTERACT_SLIDE",
			"INTERACT_LEVER",
			"INTERACT_PLANK",
			"INTERACT_ROPECLIMB",
			"INTERACT_ROPEDOWN",
			"INTERACT_DOOROPEN",
			"INTERACT_LADDERUP",
			"INTERACT_WHIPSWING",
			"INTERACT_WHIPPULL",
			"INTERACT_ROTATIONVALVE",
		};

		if (IM_ARRAYSIZE(InteractTypes) <= m_eInteractType)
		{
			m_eInteractType = 0;
			return;
		}

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
			m_pPreviewInteract->Set_AnimationIndex(m_iInteractPlayAnimIndex);
		}


		ImGui::Checkbox(u8"레벨 체인지", &m_bInteractLevelChange);

		if (m_bInteractLevelChange == true)
		{
			ImGui::SeparatorText(u8"상호작용시 이동할 레벨");
			{
				//!LEVEL_INTRO_BOSS,
				//!	LEVEL_SNOWMOUNTAIN,
				const char* InteractLevels[] = { u8"인트로보스레벨", u8"설산레벨", u8"설산보스레벨"};
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
		
		if (m_iSelectObjectIndex > 0)
			m_iSelectObjectIndex--;
		
	}
}

void CWindow_MapTool::Interact_SplineSave()
{
	json SplineJson;


	_int iSplinePointSize = (_int)m_mapSplinePoints.size();
	_int iJsonIndex = 0;

	SplineJson[iJsonIndex].emplace("DivergingCount", m_iSplineDivergingCount);


	for (auto& iter : m_mapSplinePoints)
	{
		json SplineVectorJson;

		string strTest = iter.first.c_str();
		SplineJson[iJsonIndex].emplace("SplineKey", strTest);

		_int iSplineVectorSize = _int(iter.second.size());

		for (_int i = 0; i < iSplineVectorSize; ++i)
		{
			CJson_Utility::Write_Float4(SplineVectorJson[i]["SplinePoint"], iter.second[i]);
		}

		SplineJson[iJsonIndex].emplace("SplineVectorJson", SplineVectorJson);

		iJsonIndex++;


	}

	iJsonIndex = 0;

	for (auto& iter : m_mapSplineSpeeds)
	{
		SplineJson[iJsonIndex].emplace("SplineSpeed", iter.second);
		iJsonIndex++;
	}




	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	// 현재 년도, 월, 일, 시간, 분 구하기
	std::tm now_tm = *std::localtime(&now_c);
	int year = now_tm.tm_year + 1900; // tm_year는 1900년부터의 연도를 나타냄
	int month = now_tm.tm_mon + 1; // tm_mon은 0부터 시작하므로 1을 더해줌
	int day = now_tm.tm_mday;
	int hour = now_tm.tm_hour;
	int minute = now_tm.tm_min;

	// 문자열에 추가
	std::ostringstream oss;
	oss << "../Bin/DataFiles/Data_Map/Spline/SplineData_"
		<< std::setfill('0') << std::setw(4) << year // 4자리로 년도 포맷
		<< std::setw(2) << month // 2자리로 월 포맷
		<< std::setw(2) << day // 2자리로 일 포맷
		<< std::setw(2) << hour // 2자리로 시간 포맷
		<< std::setw(2) << minute // 2자리로 분 포맷
		<< ".json";

	// 결과 출력
	std::string strSavePath = oss.str();

	if (FAILED(CJson_Utility::Save_Json(strSavePath.c_str(), SplineJson)))
	{
		MSG_BOX("스플라인데이터 저장 실패");
	}
	else
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_SplineJsonPath(strSavePath);
		//m_vecCreateInteractObject[m_iSelectObjectIndex]
		MSG_BOX("스플라인데이터 저장 성공");
	}
}

void CWindow_MapTool::Interact_SplineLoad()
{

	json SplineJson = {};
	//C:\Users\PC\Desktop\3D_TeamPortpolio\Client\Bin\DataFiles\Data_Map\Spline
	//string strHardPath = "../Bin/DataFiles/Data_Map/Spline/SplineData_202403252212.json";
	string strHardPath = "../Bin/DataFiles/Data_Map/Spline/SplineData_202403271555.json";
	

	if (FAILED(CJson_Utility::Load_Json(strHardPath.c_str()/*m_tEnvironmentDesc.strSplineJsonPath.c_str()*/, SplineJson)))
	{
		MSG_BOX("스플라인 데이터 불러오기 실패");
	}

	_int iSplineJsonSize = (_int)SplineJson.size();

	for (_int i = 0; i < iSplineJsonSize; ++i)
	{
		string strSplineKey = (string)SplineJson[i]["SplineKey"];
		strSplineKey = strSplineKey.c_str();
		json   SplineVectorJson = SplineJson[i]["SplineVectorJson"];
		_float fSplineSpeed = SplineJson[i]["SplineSpeed"]; //! 해당 구간 속도

		_int iSplineVectorJsonSize = (_int)SplineVectorJson.size();

		vector<_float4> vecSplinePoint;

		for (_int j = 0; j < iSplineVectorJsonSize; ++j)
		{
			_float4 vSplinePoint = {};
			CJson_Utility::Load_Float4(SplineVectorJson[j]["SplinePoint"], vSplinePoint);
			vecSplinePoint.push_back(vSplinePoint);

			m_vecSplineListBox.push_back(to_string(m_iSplinePickingIndex));
			++m_iSplinePickingIndex;
		}

		m_mapSplineSpeeds.emplace(strSplineKey, fSplineSpeed);
		m_mapSplinePoints.emplace(strSplineKey, vecSplinePoint);
		m_mapSplineListBox.emplace(strSplineKey, m_vecSplineListBox);
		m_vecSplineListBox.clear();
		m_iSplinePickingIndex = 0;
		
	}

		
}

void CWindow_MapTool::Interact_SplineFucntion()
{
	ImGui::SeparatorText(u8"스플라인 셋팅");

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck())
		{
			m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
			m_fRayPos = m_pField->GetMousePos(m_tWorldRay);

			_float4 vCurrentRayPos = { m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f };
			m_vecSplinePoints.push_back(vCurrentRayPos);

			m_vecSplineListBox.push_back(to_string(m_iSplinePickingIndex));
			++m_iSplinePickingIndex;
		}
	}
	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{
		if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck())
		{
			_float4 vCurrentRayPos = { m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f };
			m_vecSplinePoints.push_back(vCurrentRayPos);
			m_vecSplineListBox.push_back(to_string(m_iSplinePickingIndex));

			++m_iSplinePickingIndex;
		}
	}

	_int iPickedSize = (_int)m_vecSplineListBox.size();

	if (false == m_vecSplinePoints.empty())
	{
		if (ImGui::BeginListBox(u8"픽킹 정보"))
		{
			for (_int i = 0; i < iPickedSize; ++i)
			{
				const _bool isSelected = (m_iSplineListIndex == i);

				if (ImGui::Selectable(m_vecSplineListBox[i].c_str(), isSelected))
				{
					m_iSplineListIndex = i;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}

		if (m_iSplineListIndex < m_vecSplinePoints.size())
		{
			ImGui::Text(u8"픽킹 X : %f", m_vecSplinePoints[m_iSplineListIndex].x);
			ImGui::Text(u8"픽킹 Y : %f", m_vecSplinePoints[m_iSplineListIndex].y);
			ImGui::Text(u8"픽킹 Z : %f", m_vecSplinePoints[m_iSplineListIndex].z);

			_float vPoints[3] = { m_vecSplinePoints[m_iSplineListIndex].x, m_vecSplinePoints[m_iSplineListIndex].y, m_vecSplinePoints[m_iSplineListIndex].z };

			if (ImGui::InputFloat3(u8"포인트값변경", vPoints))
			{
				m_vecSplinePoints[m_iSplineListIndex].x = vPoints[0];
				m_vecSplinePoints[m_iSplineListIndex].y = vPoints[1];
				m_vecSplinePoints[m_iSplineListIndex].z = vPoints[2];
			}



		}

		if (ImGui::Button(u8"픽킹인덱스 삭제"))
		{
			if (m_iSplineListIndex < m_vecSplinePoints.size())
			{
				m_vecSplinePoints.erase(m_vecSplinePoints.begin() + m_iSplineListIndex);
				m_vecSplineListBox.erase(m_vecSplineListBox.begin() + m_iSplineListIndex);
			}
		}
	}

	if (ImGui::Button(u8"스플라인 이벤트 테스트"))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Start_Spline(&m_vecSplinePoints);
		//m_vecCreateInteractObject[m_iSelectObjectIndex]->Start_SplineDouble(&m_vecSplinePoints);
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"스플라인 클리어"))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Spline_Clear();
		m_vecSplineListBox.clear();
		m_vecSplinePoints.clear();
		m_iSplineListIndex = 0;
		m_iSplinePickingIndex = 0;
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"시작 지점으로 리셋"))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Reset_StartMatrix();
	}

	static _float fSplineSpeed = 1.f;
	if (ImGui::InputFloat(u8"스플라인 스피드", &fSplineSpeed))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_SplineSpeed(fSplineSpeed);
	}

	if (ImGui::InputInt(u8"스플라인 분기점 개수", &m_iSplineDivergingCount))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_SplineDivergingCount(m_iSplineDivergingCount);
	}

	ImGui::NewLine();


	ImGui::InputText(u8"트리거 네임태그", m_strSplinePointKeyTag, IM_ARRAYSIZE(m_strSplinePointKeyTag));


	if (ImGui::Button(u8"현재 스플라인벡터 저장"))
	{
		vector<_float4> vecSplinePoint;

		_int iSaveSplinePointSize = (_int)m_vecSplinePoints.size();

		for (_int i = 0; i < iSaveSplinePointSize; ++i)
		{
			vecSplinePoint.push_back(m_vecSplinePoints[i]);

		}
		string strEmplaceKey = m_strSplinePointKeyTag;



		m_mapSplineSpeeds.emplace(strEmplaceKey, fSplineSpeed);
		m_mapSplinePoints.emplace(strEmplaceKey, m_vecSplinePoints);
		m_mapSplineListBox.emplace(strEmplaceKey, m_vecSplineListBox);
		ZeroMemory(m_strSplinePointKeyTag, sizeof(m_strSplinePointKeyTag));
		m_vecSplinePoints.clear();
		m_vecSplineListBox.clear();

		m_iSplinePickingIndex = 0;
		m_iSplineListIndex = 0;

		//m_mapSplinePoints()
	}

	if (ImGui::Button(u8"스플라인 데이터 저장(Json)"))
	{
		Interact_SplineSave();
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"스플라인 데이터 불러오기(Json"))
	{
		Interact_SplineLoad();
	}

	if (false == m_mapSplinePoints.empty())
	{
		for (auto& iter : m_mapSplinePoints)
		{
			string strLabel = string(u8"") + iter.first;

			if (ImGui::Button(strLabel.c_str()))
			{
				m_vecSplineListBox.clear();
				m_vecSplinePoints.clear();

				m_vecSplinePoints = iter.second;

				for (auto& Listiter : m_mapSplineListBox)
				{
					if (Listiter.first == iter.first)
					{
						m_vecSplineListBox = Listiter.second;
						m_iSplineListIndex = 0;
					}
					else
						continue;
				}

			}
			ImGui::SameLine();
		}
	}
}


void CWindow_MapTool::Interact_LevelChangeFunction()
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
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_LevelChangeType(m_tSelectInteractDesc.bLevelChange, LEVEL_INTRO_BOSS);
#endif // _DEBUG
			break;
		}

		case 1:
		{
#ifdef _DEBUG
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_LevelChangeType(m_tSelectInteractDesc.bLevelChange, LEVEL_SNOWMOUNTAIN);
#endif // _DEBUG
			break;
		}
		}
	}

}


void CWindow_MapTool::Interact_GroupFunction()
{
	if (m_vecCreateInteractObject[m_iSelectObjectIndex] == nullptr)
		return;

	CEnvironment_Interact* pInteractObject = m_vecCreateInteractObject[m_iSelectObjectIndex];


	ImGui::SeparatorText(u8"상호작용 그룹설정");
	{
		if (ImGui::InputInt(u8"상호작용 그룹 인덱스", &m_tSelectInteractDesc.iInteractGroupIndex))
		{
			pInteractObject->Set_InteractGroupIndex(m_tSelectInteractDesc.iInteractGroupIndex);
		}

		

		ImGui::NewLine();

		if (ImGui::Checkbox(u8"오너 승격", &m_tSelectInteractDesc.bOwner))
		{
			pInteractObject->Set_OwnerPromotion(m_tSelectInteractDesc.bOwner);
		}
		ImGui::SameLine();
		ImGui::Checkbox(u8"오프셋 셋팅", &m_tSelectInteractDesc.bOffset);
		ImGui::SameLine();
		ImGui::Checkbox(u8"그룹 오브젝트 추가", &m_bShowAddInteract);
		ImGui::SameLine();


		if (ImGui::Checkbox(u8"활성화 위치", &m_tSelectInteractDesc.bEnable))
		{
			m_tSelectInteractDesc.bEnable = m_tSelectInteractDesc.bEnable;
		}

		
		if (m_tSelectInteractDesc.bEnable == true)
		{
			Interact_EnableFunction();
		}

		if (m_tSelectInteractDesc.bOffset == true)
		{
			
			if (ImGui::InputFloat4(u8"오프셋", &m_tSelectInteractDesc.vOffset.x))
			{
				pInteractObject->Set_Offset(m_tSelectInteractDesc.bOffset, m_tSelectInteractDesc.vOffset);
			}

		}

		

		if (m_bShowAddInteract == true)
		{
			ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

			ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 80), ImGuiChildFlags_Border, WindowFlag);

			if (ImGui::BeginListBox(u8"그룹에 추가할 상호작용오브젝트 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_uint iObjectTagSize = (_uint)m_vecCreateInteractObject.size();

				for (_uint i = 0; i < iObjectTagSize; ++i)
				{
					const _bool isSelected = (m_iAddInteractSelectIndex == i);

					if (ImGui::Selectable(m_vecCreateInteractObjectTag[i].c_str(), isSelected))
					{
						m_iAddInteractSelectIndex = i;



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

			ImGui::BeginChild("Create_RightChild", ImVec2(0, 80), ImGuiChildFlags_Border, WindowFlag);

			static _int iSelectincludedIndex = 0;
			_int iIncludedObjectSize = pInteractObject->Get_InteractGroupVector().size();

			vector<string> vecIncludedObjectTag = pInteractObject->Get_InteractGroupTag();

			if (ImGui::BeginListBox(u8"포함된 상호작용오브젝트 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (_uint i = 0; i < iIncludedObjectSize; ++i)
				{
					const _bool isSelected = (iSelectincludedIndex == i);

					if (ImGui::Selectable(vecIncludedObjectTag[i].c_str(), isSelected))
					{
						iSelectincludedIndex = i;

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndListBox();
			}

			ImGui::NewLine();

			if (ImGui::Button(u8"선택한 상호작용 오브젝트 그룹추가"))
			{
				if (m_iSelectObjectIndex == m_iAddInteractSelectIndex)
				{
					MSG_BOX("자기 자신을 그룹에 추가할 수 없습니다.");
					return;
				}

				m_vecCreateInteractObject[m_iSelectObjectIndex]->Add_InteractGroupObject(m_vecCreateInteractObject[m_iAddInteractSelectIndex]);
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"선택된 포함 상호작용오브젝트 제거"))
			{
				pInteractObject->Delete_InteractGroupObject(iSelectincludedIndex, &iSelectincludedIndex);
			}

			ImGui::EndChild();
			
		}
	}

	ImGui::NewLine();

	if (ImGui::Button(u8"오너 부여"))
	{
		_int iInteractObjectSize = (_int)m_vecCreateInteractObject.size();

		CEnvironment_Interact* pOwnerObject = nullptr;

		for (_int i = 0; i < iInteractObjectSize; ++i)
		{
			CEnvironment_Interact* pSearchObject = m_vecCreateInteractObject[i];
			CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC FindInteractInfo = *pSearchObject->Get_EnvironmentDesc();

			if (FindInteractInfo.bOwner == true && FindInteractInfo.iInteractGroupIndex == m_tSelectInteractDesc.iInteractGroupIndex)
			{
				pOwnerObject = pSearchObject;
				break;
			}
		}

		
		if (pOwnerObject == nullptr)
			MSG_BOX("오너를 찾지 못했습니다.");
		else
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_OwnerObject(pOwnerObject);
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"상호작용 테스트"))
	{
		pInteractObject->StartGroupInteract();
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"상호작용 리셋"))
	{
		pInteractObject->Reset_Interact();
	}
}

void CWindow_MapTool::Interact_ColliderFunction()
{
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

void CWindow_MapTool::Interact_RotationFunction()
{
	if (m_vecCreateInteractObject[m_iSelectObjectIndex] == nullptr)
		return;

	CEnvironment_Interact* pInteractObject = m_vecCreateInteractObject[m_iSelectObjectIndex];


	if (ImGui::InputFloat(u8"로테이션 각도", &m_tSelectInteractDesc.fRotationAngle))
	{
		pInteractObject->Set_RotationAngle(m_tSelectInteractDesc.fRotationAngle);
	}

	if (ImGui::InputFloat(u8"로테이션 스피드", &m_tSelectInteractDesc.fRotationSpeed))
	{
		pInteractObject->Set_RotationSpeed(m_tSelectInteractDesc.fRotationSpeed);
	}

	if (ImGui::Button(u8"로테이션 리셋"))
	{
		pInteractObject->Reset_Rotate();
	}
	
}

void CWindow_MapTool::Interact_ArrivalMissonFunction()
{
	if (m_vecCreateInteractObject[m_iSelectObjectIndex] == nullptr)
		return;

	CEnvironment_Interact* pInteractObject = m_vecCreateInteractObject[m_iSelectObjectIndex];
	
	if (ImGui::InputFloat4(u8"도착 지점", &m_tSelectInteractDesc.vArrivalPosition.x))
	{
		pInteractObject->Set_ArrivalMission(m_tSelectInteractDesc.bArrival, m_tSelectInteractDesc.vArrivalPosition);
	}

	static _bool bArrivalPosPicking = false;

	ImGui::Checkbox(u8"도착 지점 픽킹 모드", &bArrivalPosPicking);

	if (bArrivalPosPicking == true)
	{
		if (m_pGameInstance->Mouse_Down(DIM_LB) && ImGui_MouseInCheck() == TRUE)
		{
			if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
			{
				m_tSelectInteractDesc.vArrivalPosition = m_fRayPos;
				pInteractObject->Set_ArrivalMission(m_tSelectInteractDesc.bArrival, m_tSelectInteractDesc.vArrivalPosition);
				
			}
			else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
			{
				m_tSelectInteractDesc.vArrivalPosition = m_fMeshPos;
				pInteractObject->Set_ArrivalMission(m_tSelectInteractDesc.bArrival, m_tSelectInteractDesc.vArrivalPosition);
				
			}
		}

	}
	
}



void CWindow_MapTool::Interact_ShowInfoWindow()
{
	if (m_vecCreateInteractObject[m_iSelectObjectIndex] == nullptr)
		return;

	CEnvironment_Interact* pSelectInteract = m_vecCreateInteractObject[m_iSelectObjectIndex];

	CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC InteractInfo = *pSelectInteract->Get_EnvironmentDesc();

	ImGui::SetNextWindowPos(ImVec2(1700, 500), ImGuiCond_FirstUseEver);

	ImGui::Begin(u8"상호작용 오브젝트 정보", NULL);
	
	
	ImGui::SeparatorText(u8"도착 정보");
	{
		static int iArrivalType = !InteractInfo.bArrival;

		const char* ArrivalType[2] = { u8"Arrival", u8"NonArrival" };


		for (_uint i = 0; i < IM_ARRAYSIZE(ArrivalType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(ArrivalType[i], &iArrivalType, i);
		}

	}
	
	ImGui::SeparatorText(u8"레벨 체인지 정보");
	{
		static int iLevelChange = !InteractInfo.bLevelChange;

		const char* LevelChangeType[2] = { u8"LevelChange", u8"NonLevelChange" };


		for (_uint i = 0; i < IM_ARRAYSIZE(LevelChangeType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(LevelChangeType[i], &iLevelChange, i);
		}
	}
	
	ImGui::SeparatorText(u8"오프셋 정보");
	{
		static int iOffset = !InteractInfo.bOffset;

		const char* OffsetType[2] = { u8"Offset", u8"NonOffset" };


		for (_uint i = 0; i < IM_ARRAYSIZE(OffsetType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(OffsetType[i], &iOffset, i);
		}

	}

	ImGui::SeparatorText(u8"오너 정보");
	{
		static int iOwner = !InteractInfo.bOwner;

		const char* OwnerType[2] = { u8"Owner", u8"NonOwner" };


		for (_uint i = 0; i < IM_ARRAYSIZE(OwnerType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(OwnerType[i], &iOwner, i);
		}
		
	}
	
	{
		static int iRootTranslate = !InteractInfo.bRootTranslate;

		const char* RootTranslateType[2] = { u8"RootTranslate", u8"NonRootTranslate" };


		for (_uint i = 0; i < IM_ARRAYSIZE(RootTranslateType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(RootTranslateType[i], &iRootTranslate, i);
		}
	}

	{
		static int iRotate = !InteractInfo.bRotate;

		const char* RotateType[2] = { u8"Rotate", u8"NonRotate" };


		for (_uint i = 0; i < IM_ARRAYSIZE(RotateType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(RotateType[i], &iRotate, i);
		}
	}

	{
		static int iUseGravity = !InteractInfo.bUseGravity;

		const char* UseGravityType[2] = { u8"UseGravity", u8"NonUseGravity" };


		for (_uint i = 0; i < IM_ARRAYSIZE(UseGravityType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(UseGravityType[i], &iUseGravity, i);
		}
	}

	{
		static int iUseGravity = !InteractInfo.bUseGravity;

		const char* UseGravityType[2] = { u8"UseGravity", u8"NonUseGravity" };


		for (_uint i = 0; i < IM_ARRAYSIZE(UseGravityType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(UseGravityType[i], &iUseGravity, i);
		}
	}

	{
		static int iInteractState = InteractInfo.eInteractState;

		const char* InteractState[2] = { u8"무한상호작용", u8"한번만 수행" };


		for (_uint i = 0; i < IM_ARRAYSIZE(InteractState); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			ImGui::RadioButton(InteractState[i], &iInteractState, i);
		}
	}

	{
		string strChangeLevel = "";

		if (InteractInfo.eChangeLevel == LEVEL_INTRO_BOSS)
		{
			strChangeLevel = "LEVEL_INTRO_BOSS";
		}
		else if (InteractInfo.eChangeLevel == LEVEL_SNOWMOUNTAIN)
		{
			strChangeLevel = "LEVEL_SNOWMOUNTAIN";
		}
		else if (InteractInfo.eChangeLevel == LEVEL_SNOWMOUNTAINBOSS)
		{
			strChangeLevel = "LEVEL_SNOWMOUNTAINBOSS";
		}

		ImGui::NewLine();
		ImGui::Text(u8"레벨 체인지 : %s", strChangeLevel.c_str());
	}

	{
		string strInteractType = "";

		switch (InteractInfo.eInteractType)
		{
			case CEnvironment_Interact::INTERACT_JUMP100:
				strInteractType = "INTERACT_JUMP100";
				break;

			case CEnvironment_Interact::INTERACT_JUMP200:
				strInteractType = "INTERACT_JUMP200";
				break;

			case CEnvironment_Interact::INTERACT_JUMP300:
				strInteractType = "INTERACT_JUMP300";
				break;

			case CEnvironment_Interact::INTERACT_VAULT100:
				strInteractType = "INTERACT_VAULT100";
				break;

			case CEnvironment_Interact::INTERACT_VAULT200:
				strInteractType = "INTERACT_VAULT200";
				break;

			case CEnvironment_Interact::INTERACT_WAGONPUSH:
				strInteractType = "INTERACT_WAGONPUSH";
				break;

			case CEnvironment_Interact::INTERACT_WAGONJUMP:
				strInteractType = "INTERACT_WAGONJUMP";
				break;

			case CEnvironment_Interact::INTERACT_WAGONEVENT:
				strInteractType = "INTERACT_WAGONEVENT";
				break;

			case CEnvironment_Interact::INTEARCT_WAGONROPEJUMP:
				strInteractType = "INTEARCT_WAGONROPEJUMP";
				break;

			case CEnvironment_Interact::INTERACT_CLIMB100:
				strInteractType = "INTERACT_CLIMB100";
				break;

			case CEnvironment_Interact::INTERACT_CLIMB200:
				strInteractType = "INTERACT_CLIMB200";
				break;

			case CEnvironment_Interact::INTERACT_CLIMB300:
				strInteractType = "INTERACT_CLIMB300";
				break;

			case CEnvironment_Interact::INTERACT_CLIMB450:
				strInteractType = "INTERACT_CLIMB450";
				break;

			case CEnvironment_Interact::INTERACT_SLIDE:
				strInteractType = "INTERACT_SLIDE";
				break;

			case CEnvironment_Interact::INTERACT_LEVER:
				strInteractType = "INTERACT_LEVER";
				break;

			case CEnvironment_Interact::INTERACT_PLANK:
				strInteractType = "INTERACT_PLANK";
				break;

			case CEnvironment_Interact::INTERACT_ROPECLIMB:
				strInteractType = "INTERACT_ROPECLIMB";
				break;

			case CEnvironment_Interact::INTERACT_ROPEDOWN:
				strInteractType = "INTERACT_ROPEDOWN";
				break;

			case CEnvironment_Interact::INTERACT_DOOROPEN:
				strInteractType = "INTERACT_DOOROPEN";
				break;

			case CEnvironment_Interact::INTERACT_LADDERUP:
				strInteractType = "INTERACT_LADDERUP";
				break;

			case CEnvironment_Interact::INTERACT_WHIPSWING:
				strInteractType = "INTERACT_WHIPSWING";
				break;

			case CEnvironment_Interact::INTERACT_WHIPPULL:
				strInteractType = "INTERACT_WHIPPULL";
				break;

			case CEnvironment_Interact::INTERACT_ROTATIONVALVE:
				strInteractType = "INTERACT_ROTATIONVALVE";
				break;
		}

		ImGui::NewLine();
		ImGui::Text(u8"상호작용 타입 : %s", strInteractType.c_str());
	}

	{
		ImGui::NewLine();
		ImGui::InputFloat(u8"회전각도", &InteractInfo.fRotationAngle);
		ImGui::InputFloat(u8"회전속도", &InteractInfo.fRotationSpeed);
	}	

	{
		ImGui::NewLine();
		ImGui::InputInt(u8"상호작용그룹인덱스", &InteractInfo.iInteractGroupIndex);
	}

	{
		ImGui::NewLine();
		ImGui::InputFloat4(u8"도착위치", &InteractInfo.vArrivalPosition.x);
	}
	
	{
		ImGui::NewLine();
		ImGui::InputFloat4(u8"활성화위치", &InteractInfo.vEnablePosition.x);
	}

	{
		ImGui::NewLine();
		ImGui::InputFloat4(u8"오프셋", &InteractInfo.vOffset.x);
	}

	{
		ImGui::NewLine();
		ImGui::InputFloat4(u8"플레이어루트레이트", &InteractInfo.vPlayerRootMoveRate.x);
	}

	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), u8"현재 플레이어 셀 인덱스 : %d", m_pPlayer->Get_Navigation()->Get_CurrentCellIndex());
	//ImGui::Text("ArrivalMisson")



	ImGui::End();
}

void CWindow_MapTool::Interact_EnableFunction()
{
	ImGui::SeparatorText(u8"활성화 위치 셋팅");

	static _bool bPickingMode = false;

	ImGui::Checkbox(u8"활성화 위치 픽킹 모드", &bPickingMode);

	if (bPickingMode == true && true == ImGui_MouseInCheck() && m_pGameInstance->Mouse_Down(DIM_LB))
	{
		if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD && m_pField != nullptr)
		{	
			m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
			m_fRayPos = m_pField->GetMousePos(m_tWorldRay);

			_float4 vCurrentRayPos = { m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f };
			m_vecEnablePoints.push_back(vCurrentRayPos);
			m_vecEnableListBox.push_back(to_string(m_iEnablePickingIndex));
			++m_iEnablePickingIndex;
		}
		else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
		{
			_float4 vCurrentRayPos = { m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f };
			m_vecEnablePoints.push_back(vCurrentRayPos);
			m_vecEnableListBox.push_back(to_string(m_iEnablePickingIndex));
			++m_iEnablePickingIndex;
		}
	}
	

	_int iPickedSize = (_int)m_vecEnableListBox.size();

	if (false == m_vecEnablePoints.empty())
	{
		if (ImGui::BeginListBox(u8"픽킹 정보"))
		{
			for (_int i = 0; i < iPickedSize; ++i)
			{
				const _bool isSelected = (m_iEnableListIndex == i);

				if (ImGui::Selectable(m_vecEnableListBox[i].c_str(), isSelected))
				{
					m_iEnableListIndex = i;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}

		if (m_iEnableListIndex < m_vecEnablePoints.size())
		{
			ImGui::Text(u8"픽킹 X : %f", m_vecEnablePoints[m_iEnableListIndex].x);
			ImGui::Text(u8"픽킹 Y : %f", m_vecEnablePoints[m_iEnableListIndex].y);
			ImGui::Text(u8"픽킹 Z : %f", m_vecEnablePoints[m_iEnableListIndex].z);

			_float vPoints[3] = { m_vecEnablePoints[m_iEnableListIndex].x, m_vecEnablePoints[m_iEnableListIndex].y, m_vecEnablePoints[m_iEnableListIndex].z };

			if (ImGui::InputFloat3(u8"포인트값변경", vPoints))
			{
				m_vecEnablePoints[m_iEnableListIndex].x = vPoints[0];
				m_vecEnablePoints[m_iEnableListIndex].y = vPoints[1];
				m_vecEnablePoints[m_iEnableListIndex].z = vPoints[2];
			}

		}

		if (ImGui::Button(u8"픽킹인덱스 삭제"))
		{
			if (m_iEnableListIndex < m_vecEnablePoints.size())
			{
				m_vecEnablePoints.erase(m_vecEnablePoints.begin() + m_iEnableListIndex);
				m_vecEnableListBox.erase(m_vecEnableListBox.begin() + m_iEnableListIndex);
			}
		}
	}

	if (ImGui::Button(u8"활성화 위치 컨테이너 넘겨주기"))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_EnableForPoint(true);
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_EnablePosition(&m_vecEnablePoints);
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"활성화 위치 콜라이더 보기"))
	{
		Add_PickingCollider(&m_vecEnablePoints);
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"활성화 위치 콜라이더 클리어"))
	{
		Clear_PickingCollider();
	}

	
	ImGui::SameLine();

	if (ImGui::Button(u8"활성화 위치 클리어"))
	{
		m_vecCreateInteractObject[m_iSelectObjectIndex]->Reset_EnablePosition();
		m_vecEnablePoints.clear();
		m_vecEnableListBox.clear();
		m_iEnableListIndex = 0;
		m_iEnablePickingIndex = 0;

		Clear_PickingCollider();
	}

	ImGui::NewLine();

	if (ImGui::Button(u8"현재 활성화벡터 저장"))
	{
		

		_int iSaveEnablePointSize = (_int)m_vecEnablePoints.size();


		json EnablePointJson;

		for (_int i = 0; i < iSaveEnablePointSize; ++i)
		{
			CJson_Utility::Write_Float4(EnablePointJson[i], m_vecEnablePoints[i]);
		}



		string strModelTag = m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_StringModelTag();
		string strFilePath = "../Bin/DataFiles/Data_Map/Enable/" + strModelTag + to_string(m_iSelectObjectIndex);
		

		CJson_Utility::Save_Json(strFilePath.c_str(), EnablePointJson);

		m_vecEnablePoints.clear();
		m_vecEnableListBox.clear();

		m_iEnableListIndex = 0;
		m_iEnablePickingIndex = 0;

		m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_EnableJsonPath(strFilePath);
		Clear_PickingCollider();
		//m_mapSplinePoints()
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"활성화 데이터 불러오기(Json)"))
	{
		json LoadEnablePointJson;

		string strModelTag = m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_StringModelTag();
		string strFilePath = "../Bin/DataFiles/Data_Map/Enable/" + strModelTag + to_string(m_iSelectObjectIndex);

		m_vecEnablePoints.clear();
		m_vecEnableListBox.clear();

		m_iEnableListIndex = 0;
		m_iEnablePickingIndex = 0;

		CJson_Utility::Load_Json(strFilePath.c_str(), LoadEnablePointJson);

		_int EnablePointSize = LoadEnablePointJson.size();

		for (_int i = 0; i < EnablePointSize; ++i)
		{
			_float4 vEnablePoint = {};
			CJson_Utility::Load_Float4(LoadEnablePointJson[i], vEnablePoint);

			m_vecEnablePoints.push_back(vEnablePoint);
			m_vecEnableListBox.push_back(to_string(m_iEnablePickingIndex));
			m_iEnablePickingIndex++;

		}
		
	}

	
}

void CWindow_MapTool::Interact_MoveColiderFunction()
{
	ImGui::SeparatorText(u8"무브 콜라이더 셋팅");
	{
		if (ImGui::InputFloat3(u8"무브 콜라이더 사이즈", m_fMoveColliderSizeArray))
		{
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_MoveRangeColliderSize(_float3(m_fMoveColliderSizeArray[0], m_fMoveColliderSizeArray[1], m_fMoveColliderSizeArray[2]));
		}

		if (ImGui::InputFloat3(u8"무브 콜라이더 센터", m_fMoveColliderCenterArray))
		{
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_MoveRangeColliderCenter(_float3(m_fMoveColliderCenterArray[0], m_fMoveColliderCenterArray[1], m_fMoveColliderCenterArray[2]));
		}
		
		if (ImGui::Button(u8"선택한 오브젝트 기준 센터로 이동"))
		{
			_float3 vPosition = m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_Position();
			vPosition.y = vPosition.y + 1.f;
			m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_MoveRangeColliderCenter(vPosition);

			m_fMoveColliderCenterArray[0] = vPosition.x;
			m_fMoveColliderCenterArray[1] = vPosition.y;
			m_fMoveColliderCenterArray[2] = vPosition.z;
		}
		
	}
}

void CWindow_MapTool::Interact_NavigationFunction()
{
	ImGui::Begin(u8"상호작용 네비게이션");

	if (nullptr == m_pNavigation)
		return;

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	CEnvironment_Interact* pInteract = m_vecCreateInteractObject[m_iSelectObjectIndex];
	vector<_int> vecUpdateCellIndex = pInteract->Get_UpdateCellIndexs();

	_int iUpdateCellSize = vecUpdateCellIndex.size();
	static _int iSelectUpdateCellIndex = 0;

	ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

	if (ImGui::BeginListBox(u8"업데이트 시킬 셀 인덱스 목록"))
	{
		for (_int i = 0; i < iUpdateCellSize; ++i)
		{
			const _bool isSelected = (iSelectUpdateCellIndex == i);

			if (ImGui::Selectable(to_string(vecUpdateCellIndex[i]).c_str(), isSelected))
			{
				iSelectUpdateCellIndex = i;

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button(u8"선택한 셀 인덱스 삭제"))
	{
		pInteract->Erase_UpdateCellForIndex(iSelectUpdateCellIndex);
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

	if (ImGui::Button(u8"상호작용 네비게이션 생성") || m_pGameInstance->Key_Down(DIK_K))
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

			CCell* pCell = CCell::Create(m_pDevice, m_pContext, points, m_pNavigation->Get_CellSize());
			

			m_pNavigation->AddCell(pCell);
			pInteract->Add_UpdateCellIndex(pCell->Get_Index());

		}

		Reset_NaviPicking();
	}

	ImGui::Checkbox(u8"픽킹모드", &m_bPickingNaviMode);

	if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck() && true == m_bPickingNaviMode)
	{
		_float3 fPickedPos = { 0.f, 0.f, 0.f };

		if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
		{
			fPickedPos = m_fRayPos;
		}
		else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
		{
			fPickedPos = m_fMeshPos;
		}

			Find_NearPointPos(&fPickedPos);
			m_vecPickedPoints.push_back(fPickedPos);
			m_vecPickingListBox.push_back(to_string(m_iNaviPickingIndex));
			++m_iCurrentPickingIndex;
			++m_iNaviPickingIndex;
			m_fNaviPickingPos = fPickedPos;
		
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

	ImGui::End();
}



void CWindow_MapTool::SpecialTab_Function()
{
	Select_ModeType(); //! 생성, 선택, 삭제 선택, 마우스 프레싱, 다운, 업 선택

	m_eTabType = CWindow_MapTool::TAP_TYPE::TAB_SPECIAL;

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
		{
			Special_CreateTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Special_SelectTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{
			Special_DeleteTab();
			break;
		}
	}
}

void CWindow_MapTool::Special_CreateTab()
{
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	Select_PickingType();
	

	

	ImGui::SeparatorText(u8"스페셜 타입 셋팅");
	{

		static _int iSpecialType = 0;

		const char* SpecialType[2] = { u8"신호등", u8"트랙레버"};

		for (_uint i = 0; i < IM_ARRAYSIZE(SpecialType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			if (ImGui::RadioButton(SpecialType[i], &iSpecialType, i))
			{
				m_eSpecialType = iSpecialType;
			}
		}
	}
	ImGui::Separator();

	if (ImGui::BeginListBox(u8"스페셜 모델태그", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
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

	ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);
	ImGui::InputInt(u8"스페셜그룹인덱스", &m_iSpecialGroupIndex);


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

void CWindow_MapTool::Special_SelectTab()
{
	if (m_pPreviewSpecialObject != nullptr)
	{
		m_pPreviewSpecialObject->Set_Dead(true);
		m_pPreviewSpecialObject = nullptr;
	}

	_uint iObjectTagSize = (_uint)m_vecCreateSpecialObjectTag.size();

	if (true == m_vecCreateSpecialObject.empty())
	{
		ImGui::Text(u8"생성한 스페셜 객체가 없습니다. ");
		return;
	}
	else
	{
		if (ImGui::BeginListBox(u8"생성 스페셜 객체 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iObjectTagSize; ++i)
			{
				const _bool isSelected = (m_iSelectSpecialObjectIndex == i);

				if (ImGui::Selectable(m_vecCreateSpecialObjectTag[i].c_str(), isSelected))
				{
					m_iSelectSpecialObjectIndex = i;

					m_pPickingObject = m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex];

					CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC Desc = {};

					Desc = *m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Get_EnvironmentDesc();

					m_iShaderPassIndex = Desc.iShaderPassIndex;
					m_iSpecialGroupIndex = Desc.iSpecialGroupIndex;

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}

			ImGui::EndListBox();
		}

		ImGui::SeparatorText(u8"스페셜 셋팅");
		{
			if (ImGui::InputInt(u8"스페셜그룹인덱스", &m_iSpecialGroupIndex))
			{
				m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Set_SpecialGroupIndex(m_iSpecialGroupIndex);
			}

			if (ImGui::InputInt(u8"블룸메쉬인덱스", &m_iSpecialBloonMeshIndex))
			{
				m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Set_BloonMeshIndex(m_iSpecialBloonMeshIndex);
			}

			if (m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Get_SpecialType() == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
			{
				if (ImGui::Button(u8"레버 테스트"))
				{
					CUI* pLeverWeakUI = m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Get_LeverWeakUI();

					if (pLeverWeakUI != nullptr)
					{
						pLeverWeakUI->Set_Active(false);
					}
				}

				ImGui::SameLine();

				if (ImGui::Button(u8"레버 리셋"))
				{
					m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->TrackLeverInit();
				}
			}
			else if (m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Get_SpecialType() == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
			{
				if (ImGui::Button(u8"신호등 테스트"))
				{
					m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Set_SignalChange(true);
				}

				ImGui::SameLine();

				if (ImGui::Button(u8"신호등 초기화"))
				{
					m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->SignalInit();
				}
			}
			

			ImGui::NewLine();
		}
	}



	if (ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex))
	{
		m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Set_ShaderPassIndex(m_iShaderPassIndex);
	}

	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		_bool bChange = false;

		
		if (iObjectTagSize - 1 > (_uint)m_iSelectSpecialObjectIndex)
		{
			bChange = true;
			m_iSelectSpecialObjectIndex++;
		}
		else
		{
			bChange = true;
			m_iSelectSpecialObjectIndex = 0;
		}

		if (bChange == true)
		{
			m_pPickingObject = m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex];

			CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC Desc = {};

			Desc = *m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Get_EnvironmentDesc();

			m_iShaderPassIndex = Desc.iShaderPassIndex;
			m_iSpecialGroupIndex = Desc.iSpecialGroupIndex;
		}

	
	}

	if (m_pGameInstance->Key_Down(DIK_END))
	{
		_bool bChange = false;

		if (0 < m_iSelectSpecialObjectIndex)
		{
			bChange = true;
			m_iSelectSpecialObjectIndex--;
		}
		else
		{
			bChange = true;
			m_iSelectSpecialObjectIndex = iObjectTagSize - 1;
		}

		if (bChange == true)
		{
			m_pPickingObject = m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex];

			CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC Desc = {};

			Desc = *m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Get_EnvironmentDesc();

			m_iShaderPassIndex = Desc.iShaderPassIndex;
			m_iSpecialGroupIndex = Desc.iSpecialGroupIndex;
		}
		
	}

	Guizmo_Tick(m_pPickingObject);
}

void CWindow_MapTool::Special_DeleteTab()
{
	if (m_pPreviewSpecialObject != nullptr)
	{
		m_pPreviewSpecialObject->Set_Dead(true);
		m_pPreviewSpecialObject = nullptr;
	}


	
		_uint iCreateSpecialObjectSize = (_uint)m_vecCreateSpecialObject.size();

		if (iCreateSpecialObjectSize == 0)
			return;

		ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

		if (ImGui::BeginListBox(u8"생성한 스페셜 오브젝트 객체", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iCreateSpecialObjectSize; ++i)
			{
				const _bool isSelected = (m_iSelectSpecialObjectIndex == i);

				if (ImGui::Selectable(m_vecCreateSpecialObjectTag[i].c_str(), isSelected))
				{
					m_iSelectSpecialObjectIndex = i;

					m_bChange = true;
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}


		if (m_vecCreateSpecialObject.size() < m_iSelectSpecialObjectIndex)
			return;


		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_Guizmo(m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]);
	

	if (ImGui::Button(u8"삭제"))
	{
		
			m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex]->Set_Dead(true);
			m_vecCreateSpecialObject[m_iSelectSpecialObjectIndex] = nullptr;
			m_pPickingObject = nullptr;
			m_vecCreateSpecialObject.erase(m_vecCreateSpecialObject.begin() + m_iSelectSpecialObjectIndex);
			m_vecCreateSpecialObjectTag.erase(m_vecCreateSpecialObjectTag.begin() + m_iSelectSpecialObjectIndex);

			if (m_iSelectSpecialObjectIndex > 0)
				m_iSelectSpecialObjectIndex--;
	}
}

void CWindow_MapTool::InstanceTab_Function()
{
	


	Select_ModeType(); //! 생성, 선택, 삭제 선택, 마우스 프레싱, 다운, 업 선택

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
		{
			Instance_CreateTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Instance_SelectTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{
			Instance_DeleteTab();
			break;
		}
	}
}

void CWindow_MapTool::Instance_CreateTab()
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

		iModelTagSize = (_uint)m_vecEnviroModelTag.size();
		vecModelTag = m_vecEnviroModelTag;
		
		
	}

	ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);


	if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iModelTagSize; ++i)
		{
			const _bool isSelected = (iSelectTag == i);

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
		
	if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM && false == m_vecPreViewInstance.empty())
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
	
	ImGui::EndChild();


	ImGui::SameLine();

	ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);

	
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

void CWindow_MapTool::Instance_SelectTab()
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

	if (ImGui::Button(u8"셰이더 패스변경 (풀,나무 알파클립)"))
	{
		m_vecCreateInstance[m_iSelectEnvironmentIndex]->Set_ShaderPassIndex(1);
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"셰이더 패스변경 (일반 모델)"))
	{
		m_vecCreateInstance[m_iSelectEnvironmentIndex]->Set_ShaderPassIndex(6);
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
			if (iNumInstance - 1 > (_int)m_iSelectInstanceIndex)
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

void CWindow_MapTool::Instance_DeleteTab()
{
	_uint iTagSize = 0;
	vector<string> vecCreateTag;
	_uint iSelectTag = 0;

	string strListBoxName = u8"";

	iTagSize = (_uint)m_vecPreViewInstance.size();
	vecCreateTag = m_vecPreViewInstanceTag;
	strListBoxName = u8"삭제할 미리보기 인스턴스 객체 리스트";
	iSelectTag = m_iSelectPreviewIndex;
	

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iTagSize; ++i)
		{
			const _bool isSelected = (iSelectTag == i);

			if (ImGui::Selectable(vecCreateTag[i].c_str(), isSelected))
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


	if (m_vecCreateObject.size() < m_iSelectObjectIndex)
		return;

	
	if (false == m_vecCreateObject.empty() && m_vecCreateObject[m_iSelectObjectIndex] != nullptr)
	{
		if (false == m_vecPreViewInstance.empty())
		{
			Set_GuizmoCamView();
			Set_GuizmoCamProj();
			Set_Guizmo(m_vecPreViewInstance[m_iSelectPreviewIndex]);
		}
	}

	if (ImGui::Button(u8"삭제"))
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
		m_vecPreViewInstanceTag.erase(m_vecPreViewInstanceTag.begin() + m_iSelectPreviewIndex);
		m_iSelectPreviewIndex = 0;
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
			Monster_CreateTab();
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
		{
			Monster_SelectTab();	
			break;
		}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
		{
			Monster_DeleteTab();
			break;
		}
	}

	#ifdef _DEBUG
         if(m_pNavigation != nullptr)
		     m_pGameInstance->Add_DebugRender(m_pNavigation);
     #endif // _DEBUG
}

void CWindow_MapTool::Monster_CreateTab()
{
	_uint iModelTagSize = 0;
	_uint iSelectTag = m_iSelectModelTag;
	vector<string> vecModelTag;
	string strListBoxName = u8"";

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	Select_PickingType();

		
	strListBoxName = u8"몬스터 리스트";
	iModelTagSize = (_uint)m_vecMonsterTag.size();
	vecModelTag = m_vecMonsterTag;
		

	ImGui::InputInt(u8"셰이더패스", &m_iShaderPassIndex);
	
	ImGui::InputInt(u8"몬스터스폰그룹인덱스", &m_iMonsterSpawnGroupIndex);

	if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iModelTagSize; ++i)
		{
			const _bool isSelected = (iSelectTag == i);

			if (ImGui::Selectable(vecModelTag[i].c_str(), isSelected))
			{
				iSelectTag = i;
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

void CWindow_MapTool::Monster_SelectTab()
{
	if (m_pPreviewCharacter != nullptr)
	{
		m_pPreviewCharacter->Set_Dead(true);
		m_pPreviewCharacter = nullptr;
	}

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

					CMonster_Character::MONSTER_DESC Desc = *m_vecCreateMonster[m_iSelectCharacterTag]->Get_MonsterDesc();

					
					m_pPickingObject = m_vecCreateMonster[m_iSelectCharacterTag];

					m_iSelectMonsterGroupIndex = Desc.iMonsterGroupIndex;
					m_iSelectMonsterNaviIndex = Desc.iStartNaviIndex;
					
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}

		if (ImGui::InputInt(u8"몬스터그룹인덱스", &m_iSelectMonsterGroupIndex))
		{
			m_vecCreateMonster[m_iSelectCharacterTag]->Set_MonsterGroupIndex(m_iSelectMonsterGroupIndex);
		}

		if (m_pNavigation != nullptr)
		{
			ImGui::NewLine();

			if (m_pPlayer != nullptr)
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), u8"현재 플레이어 셀 인덱스 : %d", m_pPlayer->Get_Navigation()->Get_CurrentCellIndex());
			}

			if (ImGui::InputInt(u8"시작 네비게이션 인덱스", &m_iSelectMonsterNaviIndex))
			{
				m_vecCreateMonster[m_iSelectCharacterTag]->Set_StartNaviIndex(m_iSelectMonsterNaviIndex);
			}


			if (ImGui::Button(u8"네비게이션 인덱스 셋"))
			{
				m_vecCreateMonster[m_iSelectCharacterTag]->Set_StartNaviIndex(m_pNavigation->Get_SelectRangeCellIndex(m_vecCreateMonster[m_iSelectCharacterTag]));
				m_iSelectMonsterNaviIndex = m_vecCreateMonster[m_iSelectCharacterTag]->Get_StartNaviIndex();
			}


		}
		else
		{
			ImGui::Text(u8"네비게이션 데이터를 불러와주세요");
		}

	}

	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		if (iCreateMonsterTagSize - 1 > (_int)m_iSelectCharacterTag)
			m_iSelectCharacterTag++;
		else
			m_iSelectCharacterTag = 0;


		CMonster_Character::MONSTER_DESC Desc = *m_vecCreateMonster[m_iSelectCharacterTag]->Get_MonsterDesc();


		m_pPickingObject = m_vecCreateMonster[m_iSelectCharacterTag];

		m_iSelectMonsterGroupIndex = Desc.iMonsterGroupIndex;
		m_iSelectMonsterNaviIndex = Desc.iStartNaviIndex;
	}

	if (m_pGameInstance->Key_Down(DIK_END))
	{
		if (0 < m_iSelectCharacterTag)
			m_iSelectCharacterTag--;
		else
			m_iSelectCharacterTag = iCreateMonsterTagSize - 1;

		CMonster_Character::MONSTER_DESC Desc = *m_vecCreateMonster[m_iSelectCharacterTag]->Get_MonsterDesc();


		m_pPickingObject = m_vecCreateMonster[m_iSelectCharacterTag];

		m_iSelectMonsterGroupIndex = Desc.iMonsterGroupIndex;
		m_iSelectMonsterNaviIndex = Desc.iStartNaviIndex;
	}

	Guizmo_Tick(m_pPickingObject);
}

void CWindow_MapTool::Monster_DeleteTab()
{
	_uint iTagSize = 0;
	vector<string> vecCreateTag;
	_uint iSelectTag = 0;

	string strListBoxName = u8"";
	
	iTagSize = (_uint)m_vecCreateMonster.size();
	vecCreateTag = m_vecCreateMonsterTag;
	strListBoxName = u8"삭제할 캐릭터 객체 리스트";
	iSelectTag = m_iSelectCharacterTag;
	

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::BeginListBox(strListBoxName.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (_uint i = 0; i < iTagSize; ++i)
		{
			const _bool isSelected = (iSelectTag == i);

			if (ImGui::Selectable(vecCreateTag[i].c_str(), isSelected))
			{
				m_iSelectCharacterTag = i;

				m_bChange = true;
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
		}
		ImGui::EndListBox();
	}


	if (m_vecCreateMonster.size() < m_iSelectCharacterTag)
		return;

	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_CHARACTER && false == m_vecCreateMonster.empty() && m_vecCreateMonster[m_iSelectCharacterTag] != nullptr)
	{
		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_Guizmo(m_vecCreateMonster[m_iSelectCharacterTag]);
	}

	if (ImGui::Button(u8"삭제"))
	{
		
		m_vecCreateMonster[m_iSelectCharacterTag]->Set_Dead(true);
		m_vecCreateMonster[m_iSelectCharacterTag] = nullptr;
		m_vecCreateMonster.erase(m_vecCreateMonster.begin() + m_iSelectCharacterTag);
		m_vecCreateMonsterTag.erase(m_vecCreateMonsterTag.begin() + m_iSelectCharacterTag);
		m_pPickingObject = nullptr;

		if (m_iSelectCharacterTag > 0)
			m_iSelectCharacterTag--;
	}

}


void CWindow_MapTool::NPC_Tab_Function()
{
}

void CWindow_MapTool::NPC_CreateTab()
{
}

void CWindow_MapTool::NPC_SelectTab()
{
}

void CWindow_MapTool::NPC_DeleteTab()
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


	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		
	}
	else
	{
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
	}
	

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

			CCell* pCell = CCell::Create(m_pDevice, m_pContext, points, m_pNavigation->Get_CellSize());

			m_pNavigation->AddCell(pCell);
			m_vecCells.push_back(pCell);
			m_vecCellIndexs.push_back(to_string(m_pNavigation->Get_CellSize()));
		}

		Reset_NaviPicking();
	}

	ImGui::Checkbox(u8"픽킹모드", &m_bPickingNaviMode);

	if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck() && true == m_bPickingNaviMode)
	{

		if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
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
		else if(m_pField != nullptr )
		{
			_float3 fPickedPos = m_fRayPos;
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

	//if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck())
	//{
	//	_bool bIsPicking = false;
	//	_float3 fPickedPos = {};
	//
	//	if (m_vecCreateObject[m_iNavigationTargetIndex]->Picking(&fPickedPos))
	//	{
	//		fPickedPos = XMVector3TransformCoord(XMLoadFloat3(&fPickedPos), m_vecCreateObject[m_iNavigationTargetIndex]->Get_Transform()->Get_WorldMatrix());
	//
	//		Find_NearPointPos(&fPickedPos);
	//
	//		m_fNaviPickingPos = fPickedPos;
	//		bIsPicking = true;
	//	}
	//
	//	if (true == bIsPicking)
	//	{
	//		CCell* pTargetCell = Find_NearCell(fPickedPos);
	//
	//		if (nullptr == pTargetCell)
	//			return;
	//
	//		m_iCellIndex = pTargetCell->Get_Index();
	//
	//
	//		m_vecCells[m_iCellIndex]->Set_Picking(true);
	//	}
	//}


	
	
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
			m_pPlayer->Remove_Component(L"Com_Navigation", reinterpret_cast<CComponent**>(&pNavi));
		
	}

	//vector<CCell*> vecCells = m_pNavigation->Get_Cells();
	//_int iCellSize = (_int)vecCells.size();

	//if (m_pGameInstance->Mouse_Down(DIM_LB) && true == ImGui_MouseInCheck())
	//{
	//	_int index = 0;
	//
	//	_float3 fPickedPos = { 0.f, 0.f, 0.f };
	//
	//	_int	iNonAnimObjectSize = (_int)m_vecCreateObject.size();
	//
	//	_int	iIndex = 0;
	//	_float fHighestYValue = -FLT_MAX;
	//	_float3 vHighestPickesPos = {};
	//	_bool	bIsPicking = false;
	//
	//
	//
	//	if (m_vecCreateObject[m_iNavigationTargetIndex]->Picking(&fPickedPos))
	//	{
	//		
	//		Find_NearPointPos(&fPickedPos);
	//
	//		m_fNaviPickingPos = fPickedPos;
	//		bIsPicking = true;
	//	}
	//
	//	if (true == bIsPicking)
	//	{
	//		fPickedPos = XMVector3TransformCoord(XMLoadFloat3(&fPickedPos), m_vecCreateObject[m_iNavigationTargetIndex]->Get_Transform()->Get_WorldMatrix());
	//
	//		CCell* pTargetCell = nullptr;
	//		pTargetCell = Find_NearCell(fPickedPos);
	//
	//		if (nullptr == pTargetCell)
	//			return;
	//
	//		m_pNavigation->Delete_Cell(pTargetCell->Get_Index());
	//	}
	//	
	//}

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

void CWindow_MapTool::Trigger_CreateTab()
{
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::SeparatorText(u8"트리거 탭");
	{
		
		static _int iTriggerType = 0;
		const char* TriggerType[2] = { u8"몬스터 스폰트리거", u8"카메라 컷신 트리거" };

		for (_uint i = 0; i < IM_ARRAYSIZE(TriggerType); ++i)
		{
			if (i > 0) { ImGui::SameLine(); }

			if (ImGui::RadioButton(TriggerType[i], &iTriggerType, i))
			{
				iTriggerType = i;
			}
		}

		ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);



		static char NameTagBuf[32] = "MonsterSpawn";
		ImGui::InputText(u8"트리거 네임태그", NameTagBuf, IM_ARRAYSIZE(NameTagBuf));

		ImGui::InputFloat3(u8"트리거 콜라이더 사이즈", m_fColliderSizeArray);

		ImGui::InputFloat3(u8"트리거 콜라이더 센터", m_fColliderCenterArray);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);

		if (iTriggerType == 0)
		{
			ImGui::InputInt(u8"스폰그룹인덱스", &m_iMonsterSpawnGroupIndex);
			
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "작업 해야함");
		}

		ImGui::EndChild();

		if (ImGui::Button(u8" 트리거 생성"))
		{

			string strSpawnMonsterJsonPath = "Stage1Final_MonsterInclude_Decrease.json";

			if (iTriggerType == 0)
			{
				CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC MonsterTriggerDesc = {};

				MonsterTriggerDesc.bOnTrigger = false;
				MonsterTriggerDesc.strTriggerNameTag = NameTagBuf;
				MonsterTriggerDesc.vColliderSize = _float3(m_fColliderSizeArray[0], m_fColliderSizeArray[1], m_fColliderSizeArray[2]);
				MonsterTriggerDesc.vColliderCenter = _float3(m_fColliderCenterArray[0], m_fColliderCenterArray[1], m_fColliderCenterArray[2]);

				MonsterTriggerDesc.iSpawnGroupIndex = m_iMonsterSpawnGroupIndex;
				MonsterTriggerDesc.strSpawnMonsterJsonPath = strSpawnMonsterJsonPath;

				CEvent_MosnterSpawnTrigger* pMonsterSpawnTrigger = CEvent_MosnterSpawnTrigger::Create(m_pDevice, m_pContext, &MonsterTriggerDesc);

				if (pMonsterSpawnTrigger == nullptr)
				{
					MSG_BOX("몬스터 스폰 트리거 생성 실패");
				}
				else
				{
					m_vecCreateMonsterTrigger.push_back(pMonsterSpawnTrigger);
					m_vecCreateMonsterTriggerTag.push_back(MonsterTriggerDesc.strTriggerNameTag);
					m_pPickingTrigger = pMonsterSpawnTrigger;
				}

			}
		}
	}
}

void CWindow_MapTool::Trigger_SelectTab()
{
	_uint iCreateTriggerSize = 0;//= (_uint)m_vecCreateMonsterTag.size();

	static _int iTriggerType = 0;
	const char* TriggerType[2] = { u8"몬스터 스폰트리거", u8"카메라 컷신 트리거" };

	for (_uint i = 0; i < IM_ARRAYSIZE(TriggerType); ++i)
	{
		if (i > 0) { ImGui::SameLine(); }

		if (ImGui::RadioButton(TriggerType[i], &iTriggerType, i))
		{
			iTriggerType = i;
		}
	}

	if (iTriggerType == 0) //! 몬스터 스폰트리거ㄴ
	{
		iCreateTriggerSize = (_uint)m_vecCreateMonsterTriggerTag.size();

		if (true == m_vecCreateMonsterTrigger.empty())
		{
			ImGui::Text(u8"생성한 몬스터트리거가 없습니다. ");
			return;
		}
		else
		{
			if (ImGui::BeginListBox(u8"몬스터트리거 리스트", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (_uint i = 0; i < iCreateTriggerSize; ++i)
				{
					const _bool isSelected = (m_iSelectMonsterTriggerIndex == i);

					if (ImGui::Selectable(m_vecCreateMonsterTriggerTag[i].c_str(), isSelected))
					{
						m_iSelectMonsterTriggerIndex = i;
						m_pPickingTrigger = m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex];
					

						CEvent_MosnterSpawnTrigger::MONSTERSPAWN_TRIGGERDESC Desc = *m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex]->Get_MonsterTriggerDesc();

						m_fColliderCenterArray[0] = Desc.vColliderCenter.x;
						m_fColliderCenterArray[1] = Desc.vColliderCenter.y;
						m_fColliderCenterArray[2] = Desc.vColliderCenter.z;

						m_fColliderSizeArray[0] = Desc.vColliderSize.x;
						m_fColliderSizeArray[1] = Desc.vColliderSize.y;
						m_fColliderSizeArray[2] = Desc.vColliderSize.z;

						m_iSelectMonsterSpawnGroupIndex = Desc.iSpawnGroupIndex;

						strcpy(m_strSelectTriggerNameTag, Desc.strTriggerNameTag.c_str());

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndListBox();
			}

			if (ImGui::InputInt(u8"몬스터트리거 그룹인덱스", &m_iSelectMonsterSpawnGroupIndex))
			{
				m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex]->Set_SpawnGroupIndex(m_iSelectMonsterTriggerIndex);
			}

			if (ImGui::Button(u8"트리거 강제실행"))
			{
				m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex]->Activate();
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"트리거 카메라 위치이동"))
			{
				m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex]->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_CamPosition());
			}
		}
		
	}
	
	Trigger_GuizmoTick(m_pPickingTrigger);

}

void CWindow_MapTool::Trigger_DeleteTab()
{

	static _int iDeleteTriggerType = 0;

	const char* DeleteTriggerType[2] = { u8"몬스터 트리거 삭제", u8"컷신 트리거 삭제" };


	for (_uint i = 0; i < IM_ARRAYSIZE(DeleteTriggerType); ++i)
	{
		if (i > 0) { ImGui::SameLine(); }

		if (ImGui::RadioButton(DeleteTriggerType[i], &iDeleteTriggerType, i))
		{
			iDeleteTriggerType = i;
		}
	}


	if (iDeleteTriggerType == 0)
	{
		_uint iCreateMonsterTriggerSize = (_uint)m_vecCreateMonsterTrigger.size();

		if (iCreateMonsterTriggerSize == 0)
			return;

		ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

		if (ImGui::BeginListBox(u8"생성한 몬스터 트리거", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (_uint i = 0; i < iCreateMonsterTriggerSize; ++i)
			{
				const _bool isSelected = (m_iSelectMonsterTriggerIndex == i);

				if (ImGui::Selectable(m_vecCreateMonsterTriggerTag[i].c_str(), isSelected))
				{
					m_iSelectMonsterTriggerIndex = i;

					m_bChange = true;
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}

		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Trigger_GuizmoTick(m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex]);
	}
	


	if (ImGui::Button(u8"삭제"))
	{
		if (iDeleteTriggerType == 0)
		{
				Safe_Release(m_vecCreateMonsterTrigger[m_iSelectMonsterTriggerIndex]);
				m_vecCreateMonsterTrigger.erase(m_vecCreateMonsterTrigger.begin() + m_iSelectMonsterTriggerIndex);
				m_vecCreateMonsterTriggerTag.erase(m_vecCreateMonsterTriggerTag.begin() + m_iSelectMonsterTriggerIndex);
				m_pPickingTrigger = nullptr;
				
				if(m_iSelectMonsterTriggerIndex > 0)
					m_iSelectMonsterTriggerIndex--;
				
		}
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
		else if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_INSTANCE)
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

		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER)
		{
			ImGui::InputInt(u8"몬스터스폰그룹인덱스", &m_iMonsterSpawnGroupIndex);
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

		if (eTabType == CWindow_MapTool::TAP_TYPE::TAB_INSTANCE)
		{
			if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
			{
				if (ImGui::InputInt(u8"애니메이션 인덱스", &m_iAnimIndex))
				{
						
				}

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

	if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_INSTANCE)
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
		if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_INSTANCE)
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
		if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_INSTANCE && m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM && false == m_vecPreViewInstance.empty())
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
			m_vecCreateMonsterTag.erase(m_vecCreateMonsterTag.begin() + m_iSelectCharacterTag);
			m_pPickingObject = nullptr;
		}
		else if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
		{
			if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_INSTANCE)
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
					m_vecPreViewInstanceTag.erase(m_vecPreViewInstanceTag.begin() + m_iSelectPreviewIndex);
					m_iSelectPreviewIndex = 0;
				}
				else
				{
					m_vecCreateObject[m_iSelectObjectIndex]->Set_Dead(true);
					m_pPickingObject = nullptr;
					m_vecCreateObject[m_iSelectObjectIndex] = nullptr;
					m_vecCreateObject.erase(m_vecCreateObject.begin() + m_iSelectObjectIndex);
					m_vecCreateObjectTag.erase(m_vecCreateObjectTag.begin() + m_iSelectObjectIndex);
					m_iSelectObjectIndex--;
				}
			}
			else
			{
				m_vecCreateObject[m_iSelectObjectIndex]->Set_Dead(true);
				m_vecCreateObject[m_iSelectObjectIndex] = nullptr;
				m_pPickingObject = nullptr;
				m_vecCreateObject.erase(m_vecCreateObject.begin() + m_iSelectObjectIndex);
				m_vecCreateObjectTag.erase(m_vecCreateObjectTag.begin() + m_iSelectObjectIndex);
				m_iCreateObjectIndex--;
				m_iSelectObjectIndex--;
			}
		
		}
	}


}

void CWindow_MapTool::Add_PickingCollider(vector<_float4>* vPickingVector)
{

	_int iPickingVectorSize = vPickingVector->size();

	vector<_float4> vTempvector = *vPickingVector;

	for (_int i = 0; i < iPickingVectorSize; ++i)
	{
		_float3 vCenter = { vTempvector[i].x, vTempvector[i].y, vTempvector[i].z};
		BoundingSphere* pPickingSphere = new BoundingSphere{ vCenter , 2.f };

		if (pPickingSphere != nullptr)
		{
			m_vecPickingPoints.push_back(pPickingSphere);
		}
	}
}

void CWindow_MapTool::Clear_PickingCollider()
{
	_int iPickingPointSize = m_vecPickingPoints.size();

	for (_int i = 0; i < iPickingPointSize; ++i)
	{
		if (m_vecPickingPoints[i] != nullptr)
		{
			Safe_Delete(m_vecPickingPoints[i]);
			m_vecPickingPoints[i] = nullptr;
		}
	}

	m_vecPickingPoints.clear();
}


void CWindow_MapTool::Preview_Function()
{
	Change_PreViewObject(m_eTabType);
	Preview_RayFollowForTabType(m_eTabType);
}

void CWindow_MapTool::Change_PreViewObject(TAP_TYPE eTabType)
{	
	Preview_DeadForTabType(eTabType);

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
					m_pGameInstance->String_To_WString(m_vecMonsterTag[m_iSelectModelTag], strPrototypeTag);
					break;
				case Client::CWindow_MapTool::TAP_TYPE::TAB_BOSSMONSTER:
					m_pGameInstance->String_To_WString(m_vecBossTag[m_iSelectModelTag], strPrototypeTag);
					break;
				case Client::CWindow_MapTool::TAP_TYPE::TAB_NPC:
					m_pGameInstance->String_To_WString(m_vecNpcTag[m_iSelectModelTag], strPrototypeTag);
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
		switch (eTabType)
		{
			case Client::CWindow_MapTool::TAP_TYPE::TAB_SINGLE:
			{
				CreateSinglePreview();
				break;
			}

			case Client::CWindow_MapTool::TAP_TYPE::TAB_LIGHT:
			{
				CreateLightPreveiw();
				break;
			}

			case Client::CWindow_MapTool::TAP_TYPE::TAB_SPECIAL:
			{
				CreateSpecialPreview();
				break;
			}

			case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
			{
				CreateInteractPreview();
				break;
			}

			case Client::CWindow_MapTool::TAP_TYPE::TAB_INSTANCE:
			{
				CreateInstancePreview();
				break;
			}
		}
	}
}

void CWindow_MapTool::CreateSinglePreview()
{
	if (nullptr == m_pPreviewObject)
	{
		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;

		if(m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
		{
			m_pGameInstance->String_To_WString(m_vecAnimEnviroModelTag[m_iSelectModelTag], Desc.strModelTag);
		}
		else
		{
			m_pGameInstance->String_To_WString(m_vecSingleModelTag[m_iSelectModelTag], Desc.strModelTag);
		}
		
		m_pPreviewObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_Object", &Desc));
	}
}

void CWindow_MapTool::CreateInstancePreview()
{
	if (nullptr == m_pPreviewObject)
	{
		CEnvironment_Object::ENVIRONMENT_OBJECT_DESC Desc;

		if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_ANIM)
		{
			
				MSG_BOX("인스턴스는 애니메이션이 불가능합니다");
				return;
		}
		else
		{
			Desc.bAnimModel = false;
			m_pGameInstance->String_To_WString(m_vecEnviroModelTag[m_iSelectModelTag], Desc.strModelTag);
		}
		
		m_pPreviewObject = dynamic_cast<CEnvironment_Object*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_Object", &Desc));
	}

}

void CWindow_MapTool::CreateLightPreveiw()
{
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
	}
}

void CWindow_MapTool::CreateSpecialPreview()
{
	if (nullptr == m_pPreviewSpecialObject)
	{
		CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC SpecialDesc;

		SpecialDesc.bAnimModel = m_bAnimType;
		SpecialDesc.iShaderPassIndex = m_iShaderPassIndex;
		SpecialDesc.bPreview = true;


		if (m_eAnimType == CWindow_MapTool::ANIM_TYPE::TYPE_NONANIM)
		{
			m_pGameInstance->String_To_WString(m_vecSingleModelTag[m_iSelectModelTag], SpecialDesc.strModelTag);

		}
		else
		{
			if (m_bAnimType == true)
			{
				SpecialDesc.bAnimModel = true;
				SpecialDesc.iPlayAnimationIndex = 0;
			}
			m_pGameInstance->String_To_WString(m_vecAnimEnviroModelTag[m_iSelectModelTag], SpecialDesc.strModelTag);
		}

		m_pPreviewSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_SpecialObject", &SpecialDesc));
	}
}

void CWindow_MapTool::CreateInteractPreview()
{
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
			if (true == m_vecAnimInteractModelTag.empty())
			{
				MSG_BOX("애니메이션 상호작용 모델이 벡터에 없습니다.");
				return;
			}

			m_pGameInstance->String_To_WString(m_vecAnimInteractModelTag[m_iSelectModelTag], InteractDesc.strModelTag);
		}
		else
		{
			InteractDesc.bAnimModel = false;
			if (true == m_vecInteractModelTag.empty())
			{
				MSG_BOX("논애니메이션 상호작용 모델이 벡터에 없습니다.");
				return;
			}
			m_pGameInstance->String_To_WString(m_vecInteractModelTag[m_iSelectModelTag], InteractDesc.strModelTag);
		}


		m_pPreviewInteract = dynamic_cast<CEnvironment_Interact*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Test", L"Prototype_GameObject_Environment_InteractObject", &InteractDesc));
	}
}

void CWindow_MapTool::Preview_DeadForTabType(TAP_TYPE eTabType)
{
	switch (eTabType)
	{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_SINGLE:
		{
			if (m_pPreviewObject != nullptr && m_bChange == true)
			{
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;
				m_bChange = false;
			}

			if (m_pPreviewCharacter != nullptr)
			{
				m_pPreviewCharacter->Set_Dead(true);
				m_pPreviewCharacter = nullptr;
			}

			if (m_pPreviewLightObject != nullptr)
			{
				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
			}

			if (m_pPreviewSpecialObject != nullptr)
			{
				m_pPreviewSpecialObject->Set_Dead(true);
				m_pPreviewSpecialObject = nullptr;
			}

			if (m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
			}

			break;
		}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_LIGHT:
		{
			if (m_pPreviewLightObject != nullptr && m_bChange == true)
			{
				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
				m_bChange = false;
			}

			if (m_pPreviewCharacter != nullptr)
			{
				m_pPreviewCharacter->Set_Dead(true);
				m_pPreviewCharacter = nullptr;
			}

			if (m_pPreviewObject != nullptr)
			{
				
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;
			}

			if (m_pPreviewSpecialObject != nullptr)
			{
				m_pPreviewSpecialObject->Set_Dead(true);
				m_pPreviewSpecialObject = nullptr;
			}

			if (m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
			}
			break;
		}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_SPECIAL:
		{
			if (m_pPreviewSpecialObject != nullptr && m_bChange == true)
			{
				m_pPreviewSpecialObject->Set_Dead(true);
				m_pPreviewSpecialObject = nullptr;
				m_bChange = false;
			}

			if (m_pPreviewCharacter != nullptr)
			{
				m_pPreviewCharacter->Set_Dead(true);
				m_pPreviewCharacter = nullptr;
			}

			if (m_pPreviewLightObject != nullptr)
			{
				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
			}

			if (m_pPreviewObject != nullptr)
			{
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;
			}

			if (m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
			}
			break;
		}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
		{
			if (m_pPreviewInteract != nullptr && m_bChange == true)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
				m_bChange = false;
			}

			if (m_pPreviewCharacter != nullptr)
			{
				m_pPreviewCharacter->Set_Dead(true);
				m_pPreviewCharacter = nullptr;
			}

			if (m_pPreviewLightObject != nullptr)
			{
				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
			}

			if (m_pPreviewSpecialObject != nullptr)
			{
				m_pPreviewSpecialObject->Set_Dead(true);
				m_pPreviewSpecialObject = nullptr;
			}

			if (m_pPreviewObject != nullptr)
			{
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;
			}
			break;
		}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_INSTANCE:
		{

			if (m_pPreviewObject != nullptr && m_bChange == true)
			{
				m_pPreviewObject->Set_Dead(true);
				m_pPreviewObject = nullptr;
				m_bChange = false;
			}

			if (m_pPreviewCharacter != nullptr)
			{
				m_pPreviewCharacter->Set_Dead(true);
				m_pPreviewCharacter = nullptr;
			}

			if (m_pPreviewLightObject != nullptr)
			{
				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
			}

			if (m_pPreviewSpecialObject != nullptr)
			{
				m_pPreviewSpecialObject->Set_Dead(true);
				m_pPreviewSpecialObject = nullptr;
			}

			if (m_pPreviewInteract != nullptr)
			{
				m_pPreviewInteract->Set_Dead(true);
				m_pPreviewInteract = nullptr;
			}
			break;
		}
	}
}

void CWindow_MapTool::Preview_RayFollowForTabType(TAP_TYPE eTabType)
{

	_vector vPos = {};

	switch (m_ePickingType)
	{
		case Client::CWindow_MapTool::PICKING_TYPE::PICKING_FIELD:
		{
			vPos = { m_fRayPos.x, m_fRayPos.y, m_fRayPos.z, 1.f };
			break;
		}
		case Client::CWindow_MapTool::PICKING_TYPE::PICKING_MESH:
		{
			vPos = { m_fMeshPos.x, m_fMeshPos.y, m_fMeshPos.z, 1.f };
			break;
		}
		case Client::CWindow_MapTool::PICKING_TYPE::PICKING_INSTANCE:
		{
			vPos = { m_fInstanceMeshPos.x, m_fInstanceMeshPos.y, m_fInstanceMeshPos.z, 1.f };
			break;
		}
		case Client::CWindow_MapTool::PICKING_TYPE::PICKING_NONE:
		{
			return;
		}
		case Client::CWindow_MapTool::PICKING_TYPE::PICKING_END:
		{
			return;
		}
	}


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
		else if (m_eTabType == CWindow_MapTool::TAP_TYPE::TAB_SPECIAL && m_pPreviewSpecialObject != nullptr)
		{
			m_pPreviewSpecialObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
		}
		else if (m_pPreviewObject != nullptr)
		{
			m_pPreviewObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);
		}


	}

}



void CWindow_MapTool::Picking_Function()
{


	if (m_eObjectMode == CWindow_MapTool::OBJECTMODE_TYPE::OBJECTMODE_ENVIRONMENT)
	{
		if (nullptr != m_pPreviewLightObject && true == ImGui_MouseInCheck() || nullptr != m_pPreviewInteract && true == ImGui_MouseInCheck() || nullptr != m_pPreviewObject && true == ImGui_MouseInCheck() || nullptr != m_pPreviewSpecialObject && true == ImGui_MouseInCheck())
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
				case Client::CWindow_MapTool::TAP_TYPE::TAB_SPECIAL:
				{
					Special_CreateFunction();
					break;
				}

				case Client::CWindow_MapTool::TAP_TYPE::TAB_INSTANCE:
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
			Desc.iPlayAnimationIndex = m_iAnimIndex;
		
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
		Desc.iPlayAnimationIndex = m_iAnimIndex;


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
			
			m_pPreviewLightObject->Set_Dead(true);
			m_pPreviewLightObject = nullptr;
			m_bChange = true;

			_uint iLightIndex;

			CLight* pLight = m_pGameInstance->Add_Light_AndGet(m_tEditLightDesc, iLightIndex);



			if (pLight == nullptr)
			{
				MSG_BOX("라이트 생성실패");
			}
			else
			{
				LIGHT_DESC OriginDesc = pLight->Get_LightDesc();

				OriginDesc.bEnable = m_tEditLightDesc.bEnable;
				OriginDesc.eType = m_tEditLightDesc.eType;
				OriginDesc.fCutOff = m_tEditLightDesc.fCutOff;
				OriginDesc.fOuterCutOff = m_tEditLightDesc.fOuterCutOff;
				OriginDesc.vDirection = m_tEditLightDesc.vDirection;
				OriginDesc.fRange = m_tEditLightDesc.fRange;
				OriginDesc.vDiffuse = m_tEditLightDesc.vDiffuse;
				OriginDesc.vSpecular = m_tEditLightDesc.vSpecular;
				OriginDesc.vAmbient = m_tEditLightDesc.vAmbient;
				OriginDesc.vPosition = m_tEditLightDesc.vPosition;

				pLight->Set_LightDesc(OriginDesc);


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
				LightObjectDesc.strModelTag = m_pPreviewLightObject->Get_ModelTag();
				LightObjectDesc.WorldMatrix = m_pPreviewLightObject->Get_Transform()->Get_WorldMatrix();

				m_pPreviewLightObject->Set_Dead(true);
				m_pPreviewLightObject = nullptr;
				m_bChange = true;

				
				LightObjectDesc.bAnimModel = m_bAnimType;
				LightObjectDesc.iShaderPassIndex = m_iShaderPassIndex;
				LightObjectDesc.bPreview = false;
				LightObjectDesc.iLightIndex = m_tEditLightDesc.iLightIndex;
				LightObjectDesc.LightDesc = m_tEditLightDesc;
				
				LightObjectDesc.iPlayAnimationIndex = m_iAnimIndex;

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
				LightObjectDesc.iPlayAnimationIndex = m_iAnimIndex;

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
			Desc.iPlayAnimationIndex = m_iInteractPlayAnimIndex;
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

				case 2:
				{
					Desc.eChangeLevel = LEVEL_SNOWMOUNTAINBOSS;
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
		Desc.iPlayAnimationIndex = m_iInteractPlayAnimIndex;
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
			case 2:
			{
				Desc.eChangeLevel = LEVEL_SNOWMOUNTAINBOSS;
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

void CWindow_MapTool::Special_CreateFunction()
{
	if (m_ePickingType == PICKING_TYPE::PICKING_NONE)
		return;

	if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_FIELD)
	{
		if (nullptr != m_pField && true == m_pField->MouseOnTerrain())
		{
			CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC Desc = {};

			Desc = *m_pPreviewSpecialObject->Get_EnvironmentDesc();

			Desc.bPreview = false;
			Desc.bAnimModel = m_bAnimType;
			Desc.iShaderPassIndex = m_iShaderPassIndex;
			Desc.eSpecialType = (CEnvironment_SpecialObject::SPECIALTYPE)m_eSpecialType;
			Desc.fRotationPerSec = XMConvertToRadians(90.f);
			Desc.WorldMatrix = m_pPreviewSpecialObject->Get_Transform()->Get_WorldMatrix();
			Desc.iSpecialGroupIndex = m_iSpecialGroupIndex;
			Desc.iPlayAnimationIndex = m_iAnimIndex;
		
			CEnvironment_SpecialObject* pObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &Desc));

			m_vecCreateSpecialObject.push_back(pObject);


			wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateSpecialObjectIndex));
			string strConvertTag;
			m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
			m_vecCreateSpecialObjectTag.push_back(strConvertTag);

			m_iCreateSpecialObjectIndex++;
		}
	}

	else if (m_ePickingType == CWindow_MapTool::PICKING_TYPE::PICKING_MESH)
	{

		CEnvironment_SpecialObject::ENVIRONMENT_SPECIALOBJECT_DESC Desc = {};

		Desc = *m_pPreviewSpecialObject->Get_EnvironmentDesc();

		Desc.bPreview = false;
		Desc.bAnimModel = m_bAnimType;
		Desc.iShaderPassIndex = m_iShaderPassIndex;
		Desc.eSpecialType = (CEnvironment_SpecialObject::SPECIALTYPE)m_eSpecialType;
		Desc.fRotationPerSec = XMConvertToRadians(90.f);
		Desc.WorldMatrix = m_pPreviewSpecialObject->Get_Transform()->Get_WorldMatrix();
		Desc.iSpecialGroupIndex = m_iSpecialGroupIndex;


		CEnvironment_SpecialObject* pObject = dynamic_cast<CEnvironment_SpecialObject*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_BackGround", L"Prototype_GameObject_Environment_SpecialObject", &Desc));

		m_vecCreateSpecialObject.push_back(pObject);


		wstring strCreateObjectTag = m_pGameInstance->SliceObjectTag(pObject->Get_ModelTag() + L"@" + to_wstring(m_iCreateSpecialObjectIndex));
		string strConvertTag;
		m_pGameInstance->WString_To_String(strCreateObjectTag, strConvertTag);
		m_vecCreateSpecialObjectTag.push_back(strConvertTag);

		m_iCreateSpecialObjectIndex++;
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

	switch (m_eTabType)
	{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_NORMALMONSTER:
		{
			Monster_CreateFunction();
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

void CWindow_MapTool::Monster_CreateFunction()
{
	
	CMonster_Character::MONSTER_DESC Desc;
	Desc.bPreview = false;
	Desc.WorldMatrix = m_pPreviewCharacter->Get_Transform()->Get_WorldMatrix();
	Desc.iMonsterGroupIndex = m_iMonsterSpawnGroupIndex;

	wstring strProtoTag;
	m_pGameInstance->String_To_WString(m_vecMonsterTag[m_iSelectModelTag], strProtoTag);

	Desc.strProtoTypeTag = strProtoTag;
	Desc.eDescType = CGameObject::MONSTER_DESC;

	CMonster_Character* pMonster = dynamic_cast<CMonster_Character*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Monster", strProtoTag, &Desc));

	m_vecCreateMonster.push_back(pMonster);


	string strCreateMonsterTag = m_vecMonsterTag[m_iSelectModelTag] + "@" + to_string(m_iCreateMonsterIndex);

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

void CWindow_MapTool::Add_Monster_ForTrigger(CMonster_Character* pMonster)
{
	if (pMonster != nullptr)
	{
		wstring strCreateMonsterTag = pMonster->Get_MonsterDesc()->strProtoTypeTag + L"@" + to_wstring(m_iCreateMonsterIndex);
		string strConvertMonsterTag;
		m_pGameInstance->WString_To_String(strCreateMonsterTag, strConvertMonsterTag);
		

		m_vecCreateMonsterTag.push_back(strConvertMonsterTag);
		m_vecCreateMonster.push_back(pMonster);
		m_iCreateMonsterIndex++;
		
		
	}
}





void CWindow_MapTool::Interact_SelectTab()
{
	_uint iObjectTagSize = (_uint)m_vecCreateInteractObject.size();

	if (m_pPreviewInteract != nullptr)
	{
		m_pPreviewInteract->Set_Dead(true);
		m_pPreviewInteract = nullptr;
	}
	
	if (true == m_vecCreateInteractObject.empty())
	{
		ImGui::Text(u8"생성한 상호작용 객체가 없습니다. ");
		return;
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

					CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC InteractDesc = *m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_EnvironmentDesc();
					
					m_eInteractType = InteractDesc.eInteractType;
					m_eInteractState = InteractDesc.eInteractState;
					m_vInteractRootMoveRate = InteractDesc.vPlayerRootMoveRate;
					m_iShaderPassIndex = InteractDesc.iShaderPassIndex;
					m_tSelectInteractDesc = InteractDesc;
					//if(InteractDesc.iInteractGroupIndex != -1)
					//	m_bInteractUseGroup = true;
					//else
					//	m_bInteractUseGroup = false;


					//m_bInteractUseRotate = InteractDesc.bRotate;
					//m_bInteractUseRootTranslate = InteractDesc.bRootTranslate;
					//m_bInteractUseArrivalMissionSetting = InteractDesc.bArrival;

					if (InteractDesc.bAnimModel == true)
					{
						m_iAnimIndex = InteractDesc.iPlayAnimationIndex;
					}

					if (3 == (_uint)InteractDesc.eChangeLevel)
					{
						m_eInteractLevel = 0;
					}
					else if (4 == (_uint)InteractDesc.eChangeLevel)
					{
						m_eInteractLevel = 1;
					}
					else
					{
						m_eInteractLevel = 0;
					}
					m_bInteractLevelChange = InteractDesc.bLevelChange;
					m_bInteractUseGravity = InteractDesc.bUseGravity;

					_float3 vColliderSize = InteractDesc.vColliderSize;
					_float3 vColliderCenter = InteractDesc.vColliderCenter;

					m_fSelectColliderSizeArray[0] = vColliderSize.x;
					m_fSelectColliderSizeArray[1] = vColliderSize.y;
					m_fSelectColliderSizeArray[2] = vColliderSize.z;

					m_fSelectColliderCenterArray[0] = vColliderCenter.x;
					m_fSelectColliderCenterArray[1] = vColliderCenter.y;
					m_fSelectColliderCenterArray[2] = vColliderCenter.z;

					m_fMoveColliderSizeArray[0] = InteractDesc.vMoveRangeColliderSize.x;
					m_fMoveColliderSizeArray[1] = InteractDesc.vMoveRangeColliderSize.y;
					m_fMoveColliderSizeArray[2] = InteractDesc.vMoveRangeColliderSize.z;

					m_fMoveColliderCenterArray[0] = InteractDesc.vMoveRangeColliderCenter.x;
					m_fMoveColliderCenterArray[1] = InteractDesc.vMoveRangeColliderCenter.y;
					m_fMoveColliderCenterArray[2] = InteractDesc.vMoveRangeColliderCenter.z;

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

		if (m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_EnvironmentDesc()->bAnimModel == true)
		{
			if (ImGui::InputInt(u8"애니메이션인덱스", &m_iAnimIndex))
			{
				m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_AnimationIndex(m_iAnimIndex);
			}
		}
		

		ImGui::SeparatorText(u8"상호작용 셋팅");
		{
			const char* InteractTypes[] = 
			{ 
				"INTERACT_JUMP100",
				"INTERACT_JUMP200", 
				"INTERACT_JUMP300", 
				"INTERACT_VAULT100", 
				"INTERACT_VAULT200", 
				"INTERACT_WAGONPUSH",
				"INTERACT_WAGONJUMP", 
				"INTERACT_WAGONEVENT",
				"INTEARCT_WAGONROPEJUMP",
				"INTERACT_CLIMB100",
				"INTERACT_CLIMB200",
				"INTERACT_CLIMB300",
				"INTERACT_CLIMB450",
				"INTERACT_SLIDE",
				"INTERACT_LEVER",
				"INTERACT_PLANK",
				"INTERACT_ROPECLIMB",
				"INTERACT_ROPEDOWN",
				"INTERACT_DOOROPEN",
				"INTERACT_LADDERUP",
				"INTERACT_WHIPSWING",
				"INTERACT_WHIPPULL",
				"INTERACT_ROTATIONVALVE",
				"INTERACT_NONE",
			};
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

			if (ImGui::InputFloat3(u8"플레이어루트무브레이트", &m_vInteractRootMoveRate.x))
			{
				#ifdef _DEBUG
					m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_PlayerRootMoveRate(m_vInteractRootMoveRate);
				#endif // _DEBUG
			}

			ImGui::SameLine();

			if (ImGui::Checkbox(u8"중력 사용", &m_tSelectInteractDesc.bUseGravity))
			{
				#ifdef _DEBUG
					m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_UseGravity(m_tSelectInteractDesc.bUseGravity);
				#endif // _DEBUG
			}

			
			ImGui::Checkbox(u8"스플라인 이벤트", &m_bInteractUseSpline);

			if (m_bInteractUseSpline == true)
			{
				Interact_SplineFucntion();
			}
			
			ImGui::SameLine();
			ImGui::Checkbox(u8"레벨 체인지", &m_tSelectInteractDesc.bLevelChange);

			if (m_bInteractLevelChange == true)
			{
				Interact_LevelChangeFunction();
			}

			ImGui::SameLine();
			ImGui::Checkbox(u8"상호작용 그룹", &m_bInteractUseGroup);

			ImGui::SameLine();
			ImGui::Checkbox(u8"콜라이더 셋팅", &m_bInteractColliderSetting);

			ImGui::NewLine();


			if (ImGui::Checkbox(u8"로테이션 셋팅", &m_tSelectInteractDesc.bRotate))
			{
				m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_Rotate(m_tSelectInteractDesc.bRotate);
			}

			ImGui::SameLine();

			if (ImGui::Checkbox(u8"플레이어루트모션에영향받기", &m_tSelectInteractDesc.bRootTranslate))
			{
				m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_RootTranslate(m_tSelectInteractDesc.bRootTranslate);
			}

			ImGui::SameLine();

			ImGui::Checkbox(u8"도착미션 셋팅", &m_tSelectInteractDesc.bArrival);

			ImGui::SameLine();

			static _bool bNavigationMode = false;

			ImGui::Checkbox(u8"상호작용 네비게이션 셀 추가", &bNavigationMode);

			if (bNavigationMode == true)
			{
				Interact_NavigationFunction();
			}

			if (m_bInteractUseGroup == true)
			{
				Interact_GroupFunction();
			}

			if (m_bInteractColliderSetting == true)
			{
				Interact_ColliderFunction();
			}

			if (m_tSelectInteractDesc.bRotate == true)
			{
				Interact_RotationFunction();
			}

			if (m_tSelectInteractDesc.bArrival == true)
			{
				Interact_ArrivalMissonFunction();
			}

			if (m_tSelectInteractDesc.bRootTranslate == true)
			{
				Interact_MoveColiderFunction();
			}

			
			if (ImGui::Button(u8"네비게이션 컴주기"))
			{
				m_vecCreateInteractObject[m_iSelectObjectIndex]->Set_Navigation(m_pNavigation);
			}

			
		}

		if (m_pGameInstance->Key_Down(DIK_HOME))
		{
			_bool bChange = false;
			bChange = true;
			if (iObjectTagSize - 1 > (_uint)m_iSelectObjectIndex)
			{
				bChange = true;
				m_iSelectObjectIndex++;
			}
			else
			{
				bChange = true;
				m_iSelectObjectIndex = 0;
			}

			if (bChange == true)
			{
				m_pPickingObject = m_vecCreateInteractObject[m_iSelectObjectIndex];

				CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC InteractDesc = *m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_EnvironmentDesc();

				m_eInteractType = InteractDesc.eInteractType;
				m_eInteractState = InteractDesc.eInteractState;
				m_vInteractRootMoveRate = InteractDesc.vPlayerRootMoveRate;
				m_iShaderPassIndex = InteractDesc.iShaderPassIndex;

				if (InteractDesc.iInteractGroupIndex != -1)
					m_bInteractUseGroup = true;
				else
					m_bInteractUseGroup = false;


				//m_bInteractUseRotate = InteractDesc.bRotate;
				//m_bInteractUseRootTranslate = InteractDesc.bRootTranslate;
				//m_bInteractUseArrivalMissionSetting = InteractDesc.bArrival;

				if (InteractDesc.bAnimModel == true)
				{
					m_iAnimIndex = InteractDesc.iPlayAnimationIndex;
				}

				if (3 == (_uint)InteractDesc.eChangeLevel)
				{
					m_eInteractLevel = 0;
				}
				else if (4 == (_uint)InteractDesc.eChangeLevel)
				{
					m_eInteractLevel = 1;
				}
				else
				{
					m_eInteractLevel = 0;
				}
				m_bInteractLevelChange = InteractDesc.bLevelChange;
				m_bInteractUseGravity = InteractDesc.bUseGravity;

				_float3 vColliderSize = InteractDesc.vColliderSize;
				_float3 vColliderCenter = InteractDesc.vColliderCenter;

				m_fSelectColliderSizeArray[0] = vColliderSize.x;
				m_fSelectColliderSizeArray[1] = vColliderSize.y;
				m_fSelectColliderSizeArray[2] = vColliderSize.z;

				m_fSelectColliderCenterArray[0] = vColliderCenter.x;
				m_fSelectColliderCenterArray[1] = vColliderCenter.y;
				m_fSelectColliderCenterArray[2] = vColliderCenter.z;
			}
		}

		if (m_pGameInstance->Key_Down(DIK_END))
		{
			_bool bChange = false;

			if (0 < m_iSelectObjectIndex)
			{
				bChange = true;
				m_iSelectObjectIndex--;
			}
			else
			{
				bChange = true;
				m_iSelectObjectIndex = iObjectTagSize - 1;
			}

			if (bChange == true)
			{
				m_pPickingObject = m_vecCreateInteractObject[m_iSelectObjectIndex];

				CEnvironment_Interact::ENVIRONMENT_INTERACTOBJECT_DESC InteractDesc = *m_vecCreateInteractObject[m_iSelectObjectIndex]->Get_EnvironmentDesc();

				m_eInteractType = InteractDesc.eInteractType;
				m_eInteractState = InteractDesc.eInteractState;
				m_vInteractRootMoveRate = InteractDesc.vPlayerRootMoveRate;
				m_iShaderPassIndex = InteractDesc.iShaderPassIndex;

				if (InteractDesc.iInteractGroupIndex != -1)
					m_bInteractUseGroup = true;
				else
					m_bInteractUseGroup = false;


				//m_bInteractUseRotate = InteractDesc.bRotate;
				//m_bInteractUseRootTranslate = InteractDesc.bRootTranslate;
				//m_bInteractUseArrivalMissionSetting = InteractDesc.bArrival;

				if (InteractDesc.bAnimModel == true)
				{
					m_iAnimIndex = InteractDesc.iPlayAnimationIndex;
				}

				if (3 == (_uint)InteractDesc.eChangeLevel)
				{
					m_eInteractLevel = 0;
				}
				else if (4 == (_uint)InteractDesc.eChangeLevel)
				{
					m_eInteractLevel = 1;
				}
				else
				{
					m_eInteractLevel = 0;
				}
				m_bInteractLevelChange = InteractDesc.bLevelChange;
				m_bInteractUseGravity = InteractDesc.bUseGravity;

				_float3 vColliderSize = InteractDesc.vColliderSize;
				_float3 vColliderCenter = InteractDesc.vColliderCenter;

				m_fSelectColliderSizeArray[0] = vColliderSize.x;
				m_fSelectColliderSizeArray[1] = vColliderSize.y;
				m_fSelectColliderSizeArray[2] = vColliderSize.z;

				m_fSelectColliderCenterArray[0] = vColliderCenter.x;
				m_fSelectColliderCenterArray[1] = vColliderCenter.y;
				m_fSelectColliderCenterArray[2] = vColliderCenter.z;
			}
		}

		Guizmo_Tick(m_pPickingObject);
		Interact_ShowInfoWindow();
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
		m_pPickingTrigger = nullptr;

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

void CWindow_MapTool::Trigger_GuizmoTick(CEvent_Trigger* pEventTrigger)
{
	if (nullptr == pEventTrigger)
		return;

	Set_GuizmoCamView();
	Set_GuizmoCamProj();


	m_pPickingObject = nullptr;
	m_pPickingInstanceInfo = nullptr;

	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


	if (ImGui::InputText(u8"트리거 네임태그", m_strSelectTriggerNameTag, IM_ARRAYSIZE(m_strSelectTriggerNameTag)))
	{
		pEventTrigger->Set_NameTag(m_strSelectTriggerNameTag);
	}

	if (ImGui::InputFloat3(u8"트리거 콜라이더 사이즈", m_fColliderSizeArray))
	{
		pEventTrigger->Set_ColliderSize(_float3(m_fColliderSizeArray[0], m_fColliderSizeArray[1], m_fColliderSizeArray[2]));
	}

	if (ImGui::InputFloat3(u8"트리거 콜라이더 센터", m_fColliderCenterArray))
	{
		pEventTrigger->Set_ColliderCenter(_float3(m_fColliderCenterArray[0], m_fColliderCenterArray[1], m_fColliderCenterArray[2]));
	}

	if (typeid(*pEventTrigger) == typeid(CEvent_MosnterSpawnTrigger))
	{
		CEvent_MosnterSpawnTrigger* pMonsterTrigger = dynamic_cast<CEvent_MosnterSpawnTrigger*>(pEventTrigger);

		if (ImGui::InputInt(u8"스폰그룹인덱스", &m_iMonsterSpawnGroupIndex))
		{
			pMonsterTrigger->Set_SpawnGroupIndex(m_iMonsterSpawnGroupIndex);
		}

		
		pMonsterTrigger->Set_ColliderOwnerPosition();
		m_pGameInstance->Add_DebugRender(pMonsterTrigger->Get_TriggerCollider());

		if(true == pMonsterTrigger->Activate_Condition() && pMonsterTrigger->Get_MonsterTriggerDesc()->bOnTrigger == false)
			pMonsterTrigger->Activate();
			
	}
	


	if (ImGui::IsKeyPressed(ImGuiKey_T))
		TriggerCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		TriggerCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		TriggerCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", TriggerCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		TriggerCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", TriggerCurrentGizmoOperation == ImGuizmo::ROTATE))
		TriggerCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", TriggerCurrentGizmoOperation == ImGuizmo::SCALE))
		TriggerCurrentGizmoOperation = ImGuizmo::SCALE;

	
	_float* arrView = m_arrView;
	_float* arrProj = m_arrProj;

	_float	matrixTranslation[3], matrixRotation[3], matrixScale[3];
	_matrix matWorld = pEventTrigger->Get_Transform()->Get_WorldMatrix();

	_float4x4 mat4X4;
	XMStoreFloat4x4(&mat4X4, matWorld);


	_float arrWorld[] = { mat4X4._11,mat4X4._12,mat4X4._13,mat4X4._14,
						  mat4X4._21,mat4X4._22,mat4X4._23,mat4X4._24,
						  mat4X4._31,mat4X4._32,mat4X4._33,mat4X4._34,
						  mat4X4._41,mat4X4._42,mat4X4._43,mat4X4._44 };




	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::DragFloat3("Tr", matrixTranslation);
	ImGui::DragFloat3("Rt", matrixRotation);
	ImGui::DragFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);


	ImGui::Checkbox("UseSnap", &InstanceuseSnap);
	ImGui::SameLine();

	switch (TriggerCurrentGizmoOperation)
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

	ImGuizmo::Manipulate(arrView, arrProj, TriggerCurrentGizmoOperation, TriggerCurrentGizmoMode, arrWorld, NULL, InstanceuseSnap ? &snap[0] : NULL);


	XMFLOAT4X4 matW = { arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3],
					   arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7],
					   arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11],
					   arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15] };


	pEventTrigger->Get_Transform()->Set_WorldMatrix(matW);

	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}
	
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
		
};