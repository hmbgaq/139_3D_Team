#include "stdafx.h"
#include "Window_MapTool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

#include "Environment_Instance.h"
#include "Environment_Object.h"
#include "Field.h"

//#include "LandObject.h"

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

	

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, L"Layer_BackGround", TEXT("Prototype_GameObject_Terrain")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, L"Layer_BackGround", TEXT("Prototype_GameObject_Sky")));


	//CLandObject::LANDOBJECT_DESC	LandObjectDesc{};
	//
	//LandObjectDesc.pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	//LandObjectDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	//
	//m_pTestObject = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, L"Layer_Building", TEXT("Prototype_GameObject_ForkLift"), &LandObjectDesc);

	
	

		//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Skydome"))))
		//	return E_FAIL;
	
	return S_OK;
}

void CWindow_MapTool::Tick(_float fTimeDelta)
{


	__super::Tick(fTimeDelta);

	__super::Begin();

	MouseInfo_Window(fTimeDelta);

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
	
	ImGui::SeparatorText(u8"세이브 / 로드");
	{
		if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); } ImGui::SameLine(); if (ImGui::Button(u8"불러오기")) { m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_MAPTOOL_WINDOW); }
	}ImGui::Separator(); ImGui::NewLine();

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


	__super::End();


	
}

void CWindow_MapTool::Render()
{

}

HRESULT CWindow_MapTool::Save_Function()
{
	switch (m_eTabType)
	{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_GROUND:
			{
				
				break;
			}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
			{
				
				break;
			}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT:
			{
				
				break;
			}
	}

	return S_OK;
}

HRESULT CWindow_MapTool::Load_Function()
{
	switch (m_eTabType)
	{
		case Client::CWindow_MapTool::TAP_TYPE::TAB_GROUND:
		{

			break;
		}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_INTERACT:
		{

			break;
		}

		case Client::CWindow_MapTool::TAP_TYPE::TAB_ENVIRONMENT:
		{

			break;
		}
	}

	return S_OK;
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

	}ImGui::Separator(); ImGui::NewLine();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;
	
	switch (m_eModeType)
	{
		case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
			{
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

				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
			{

				break;
			}

		case Client::CWindow_MapTool::MODE_TYPE::MODE_DELETE:
			{

				break;
			}
	}
	

	//Set_GuizmoCamView();
	//Set_GuizmoCamProj();
	//Set_Guizmo(m_pTestObject);
	

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

	}ImGui::Separator(); ImGui::NewLine();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
	case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
	{
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

		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
	{

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

	}ImGui::Separator(); ImGui::NewLine();

	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	switch (m_eModeType)
	{
	case Client::CWindow_MapTool::MODE_TYPE::MODE_CREATE:
	{
		ImGui::BeginChild("Create_LeftChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_Border, WindowFlag);

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

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Create_RightChild", ImVec2(0, 260), ImGuiChildFlags_Border, WindowFlag);

		ImGui::Text(u8"테스트2");

		ImGui::EndChild();

		Preview_Function();

		break;
	}

	case Client::CWindow_MapTool::MODE_TYPE::MODE_SELECT:
	{

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
	ImGui::Begin(u8"셋팅");

	if (nullptr != m_pField)
	{
		ImGui::SeparatorText(u8"마우스 인포");

		m_fRayUpdateTimeAcc += fTimeDelta;

		if (m_fRayUpdateTimeAcc >= m_fRayUpdateTime)
		{
			m_tWorldRay = m_pGameInstance->Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
			m_fRayPos = m_pField->GetMousePos(m_tWorldRay);

			m_fRayUpdateTimeAcc = 0.f;
		}

		// x, y, z 값을 출력
		ImGui::Text(u8"마우스 X: %.2f", m_fRayPos.x); ImGui::SameLine();	ImGui::Text(u8"마우스 Y: %.2f", m_fRayPos.y); ImGui::SameLine();	ImGui::Text(u8"마우스 Z: %.2f", m_fRayPos.z);

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

void CWindow_MapTool::Preview_Function()
{
	Change_PreViewObject(m_eTabType);

	if (m_bDeadComplete == true && nullptr != m_pPreviewObject)
	{
		m_pPreviewObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_fRayPos);
	}
}

void CWindow_MapTool::Change_PreViewObject(TAP_TYPE eTabType)
{	
	if (m_bChange == true && m_pPreviewObject != nullptr)
	{
		m_pPreviewObject->Set_Dead(true);
		//! Dead 처리되는 걸 잠시 기다려주자.
	
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

		m_bChange = false;
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
}