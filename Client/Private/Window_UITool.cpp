#include "stdafx.h"
#include "Window_UITool.h"
#include "UI_Anything.h"
#include "GameInstance.h"
#include "imgui_internal.h"

#include "Json_Utility.h"
#include "Target_Manager.h"
#include "RenderTarget.h"
#include "UI_Anything.h"
#include "UI_Text.h"
#include "Easing_Utillity.h"
#include "UI_Manager.h"
#include "Data_Manager.h"
#include "UI_Distortion.h"

/* error 외부참조 기호 : define 걸어줘야함 */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CWindow_UITool::CWindow_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
	, m_pUIManager(CUI_Manager::GetInstance())
	//, m_tSelectedKeyframe(CUI::UIKEYFRAME()) // &멤버변수는 생성자에서 초기화 시켜줘야함
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CWindow_UITool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("TOOL 로딩중."));

	/* 해당 경로안에 있는 모든 이미지들을 불러온다. */
	LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/Option"))));		// Option
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/Menu"))));			// Menu
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/Blood"))));			// Blood
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/DeathScreen"))));	// DeathScreen
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/PlayerHUD"))));		// PlayerHUD
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/WorldMap"))));		// WorldMap
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/Crosshairs"))));	// Crosshairs
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/EnemyHUD"))));		// EnemyHUD
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image/Misc"))));			// Misc
	//LoadImgPath(ConverCtoWC(ConverWStringtoC(TEXT("../Bin/Resources/Textures/UI/Image"))));				// All
	
	/* 툴 들어갈때 오래걸리니까 따로 버튼눌러서 불러오자.. */
	/* 저장해둔 이미지 경로들을 불러온다. */
	// Load_Paths();

	// 이미지 로드
	_int iSize = (_int)m_vecInitialPaths.size();
	for (_int i = 0; i < iSize; i++)
	{
		IMAGEINFO* tTexture = new IMAGEINFO;

		_bool bRet = LoadTextureFromFile(ConverWStringtoC(ConvertToWideString(m_vecInitialPaths[i]->strFilePath)), &tTexture->SRV_Texture, &tTexture->iImage_Width, &tTexture->iImage_Height);
		IM_ASSERT(bRet);

		/* Texture 크기를 임의로 조절하고 싶다면, 여기서 강제로 덮어씌우자. 값을 안주면 원래 텍스처 크기대로 나온다. 추 후 원본 크기를 이용해 비율만 줄여서 출력해도 좋을 것 같다. */
		//tTexture->iImage_Width = 100;
		//tTexture->iImage_Height = 100;
		tTexture->iTextureNum = i;
		m_vecTexture.push_back(tTexture);

		tTexture = nullptr;
		delete[] tTexture;
	}

	_int iPathSize = (_int)m_vecInitialPaths.size();
	//for (auto& iter : m_vecInitialPaths)
	for(_int i = 0; i < iPathSize; i++)
	{
		string strFileNameWithoutExtension = GetFileName(m_vecInitialPaths[i]->strFilePath);
		string strFileName = RemoveExtension(strFileNameWithoutExtension);

		PATHINFO* pPathInfo = new PATHINFO;

		pPathInfo->strFileName = strFileName;
		pPathInfo->strFilePath = m_vecInitialPaths[i]->strFilePath;

		/* 경로 잘라서 넣기 */
		m_vecImagePaths.push_back(pPathInfo);

		pPathInfo = nullptr;
		delete[] pPathInfo;
	}

#pragma region 경로저장
	//char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/Texture_Info/Texture_Info";

	//json Out_Json;
	//_ushort iIndex = 0;

	//for (PATHINFO* iter : m_vecInitialPaths)
	//{
	//	json object;
	//	object["PathNum"] = iter->iPathNum;
	//	object["FileName"] = iter->strFileName;
	//	object["FilePath"] = iter->strFilePath;
	//	Out_Json.emplace(to_string(iIndex++), object);
	//}

	//CJson_Utility::Save_Json(filePath, Out_Json);

	//CJson_Utility::Load_Json(filePath, Out_Json);
#pragma endregion 


#pragma region Start Setting
	// 시작하자마자 아무것도 안눌렀을 때 기본 테그 값 주고 시작하기.
	m_tChild_Desc.strLayerTag = "Layer_UI_Player";
	m_tChild_Desc.strCloneTag = "Prototype_GameObject_UI_Anything";
	m_tChild_Desc.strProtoTag = m_vecImagePaths[m_iSelectedPathIndex]->strFileName;
	m_tChild_Desc.strFilePath = m_vecImagePaths[m_iSelectedPathIndex]->strFilePath;
	m_tChild_Desc.fPositionX = g_iWinSizeX / 2;
	m_tChild_Desc.fPositionY = g_iWinSizeY / 2;
	m_tChild_Desc.fScaleX = 100.f;
	m_tChild_Desc.fScaleY = 100.f;

	m_tParent_Desc.strLayerTag = "Layer_UI_Player";
	m_tParent_Desc.strCloneTag = "Prototype_GameObject_UI_Player_Left_Interface";
	m_tParent_Desc.strProtoTag = m_vecImagePaths[m_iSelectedPathIndex]->strFileName;
	m_tParent_Desc.strFilePath = m_vecImagePaths[m_iSelectedPathIndex]->strFilePath;
	m_tParent_Desc.fPositionX = g_iWinSizeX / 2;
	m_tParent_Desc.fPositionY = g_iWinSizeY / 2;
	m_tParent_Desc.fScaleX = 100.f;
	m_tParent_Desc.fScaleY = 100.f;
#pragma endregion End

	SetWindowText(g_hWnd, TEXT("TOOL 로딩이 완료되었습니다."));

	ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	return S_OK;
}

void CWindow_UITool::Tick(_float fTimeDelta)
{
	/* Load */
	// SetUp_Initialize();

	IndexCheck();
	GetCursorPos(&m_pt);
	ScreenToClient(g_hWnd, &m_pt);
	Shortcut_Key(fTimeDelta);
	ShowDialog();

	__super::Tick(fTimeDelta);

	ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	// 프레임
	ImGui::Text(u8"프레임 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	switch (m_eToolType)
	{
	case Client::CWindow_UITool::TOOL_UI:
	{
		SetUp_ImGuiDESC("Parent", { 600.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoMove*/ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
		__super::Begin();
		m_bCheckImguiRect_Parent = Check_ImGui_Rect();
		// Parent
		Parent_List(fTimeDelta);
		if (ImGui::Button("Create_Terrain"))
			m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, TEXT("Layer_Grid"), TEXT("Prototype_GameObject_Grid"));
			//m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"));

		if(ImGui::Button("Create_Monster"))
			m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Infected_A"));
		ImGui::Spacing();
		__super::End();


		SetUp_ImGuiDESC("Info", { 600.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoMove*/ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
		__super::Begin();
		m_bCheckImguiRect_UI_Info = Check_ImGui_Rect();
		UI_Info();
		__super::End();

		//SetUp_ImGuiDESC("Text", { 600.f, 300.f }, 0, { 0.f, 0.f, 0.f, 0.f });
		//__super::Begin();
		//__super::End();


		SetUp_ImGuiDESC("Child", { 600.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoMove*/ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
		__super::Begin();
		m_bCheckImguiRect_Child = Check_ImGui_Rect();
		Menu_Info();
		if (ImGui::BeginTabBar("MyTabBar", m_Tab_bar_flags))
		{

			if (ImGui::BeginTabItem("Texture"))
			{

				ImGui::Dummy(ImVec2(0.f, 10.f));

				if (m_pGameInstance->Key_Down(DIK_HOME))
				{
					if (!m_bCreateOK)
					{
						m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"));
						m_bCreateOK = true;
					}
				}
				/* List */
				Child_List(fTimeDelta);

				ImGui::Dummy(ImVec2(0.f, 10.f));

				/* UI_2D 세팅 */

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Text"))
			{
				ImGui::Text("Tab");

				Text_List();

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("UI_Manager"))
			{
				UI_Preset();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		__super::End();
		break;
	}
	case Client::CWindow_UITool::TOOL_ANIMATION:
	{
		SetUp_ImGuiDESC("UI_Animation", { 600.f, 300.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoMove*/ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
#ifdef _DEBUG
		if (m_pCurrSelectUI)
			m_vecTimeline = m_pCurrSelectUI->Get_vecAnimation();
#endif // DEBUG
		__super::Begin();
		m_bCheckImguiRect_UI_Animation = Check_ImGui_Rect();
		if (ImGui::BeginTabBar("UI_AnimTab", m_Tab_bar_flags))
		{
			ImGuiKeyInput();

			if (ImGui::BeginTabItem("Keyframe"))
			{
				//PlayAnimation(fTimeDelta);
				//AnimationTimeLineBar();
				KeyframeAutomaticGeneration();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("AnimObject"))
			{
				/* List */
				Child_List(fTimeDelta);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Distortion"))
			{
				Setting_Distortion(fTimeDelta);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		__super::End();

		SetUp_ImGuiDESC("Info", { 600.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoMove*/ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
		__super::Begin();
		m_bCheckImguiRect_Info = Check_ImGui_Rect();
		UI_Info();
		__super::End();

		SetUp_ImGuiDESC("UI_TimeLine", { 1000.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoMove /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
		__super::Begin();
		m_bCheckImguiRect_UI_TimeLine = Check_ImGui_Rect();
		PlayAnimation(fTimeDelta);
		AnimationTimeLineBar(fTimeDelta);
		__super::End();

		SetUp_ImGuiDESC("ValueChange", { 1000.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoMove*/ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.8f));
		__super::Begin();
		m_bCheckImguiRect_ValueChange = Check_ImGui_Rect();
			CurKeyframe_ValueChange();
		__super::End();
		break;
	}
	case Client::CWindow_UITool::TOOL_END:
		break;
	default:
		break;
	}

	/* Default : LastNumber */
	UI_ToolTip(fTimeDelta); // Tip : 툴팁은 오버레이시 모든 출력중 가장 마지막에 호출되어야한다. (안그러면 다른 녀석들이 툴팁에 밀려서 출력됨)
}

void CWindow_UITool::Render()
{

}

void CWindow_UITool::Shortcut_Key(_float fTimeDelta)
{
	if (m_bShortOff)
		return;

	if (ImGui::IsKeyPressed(ImGuiKey_1))
	{
		m_eToolType = CWindow_UITool::TOOL_UI;
	}
	if (ImGui::IsKeyPressed(ImGuiKey_2))
	{
		m_eToolType = CWindow_UITool::TOOL_ANIMATION;
	}

	if (m_pGameInstance->Key_Down(DIK_DELETE))
	{
		++m_iSelected_ChildObjectIndex;

		if (!m_vecChildObject.empty())
		{
			_int iSize = (_int)m_vecChildObject.size();
			if (m_iSelected_ChildObjectIndex >= iSize)
				m_iSelected_ChildObjectIndex = iSize - 1;

			m_vecTimeline = nullptr;
			//delete m_vecTimeline;
			//if (m_vecTimeline != nullptr);
			//{
			//	Safe_Delete(m_vecTimeline);
			//}
		}
	}

	//if (m_pGameInstance->Key_Down(DIK_X))
	//{
	//	--m_iSelected_ChildObjectIndex;

	//	if (m_iSelected_ChildObjectIndex <= 0)
	//		m_iSelected_ChildObjectIndex = 0;
	//}

	/*if (m_pGameInstance->Key_Pressing(DIK_UP))
	{
		if(m_iChangeType == (_int)SCALE)
			m_pCurrChild->Change_SizeTop(m_fChangeValue);
		if (m_iChangeType == (_int)ROTATION)
			m_pCurrChild->Get_Transform()->Turn(_vector({ 1.f, 0.f, 0.f, 0.f }), m_fChangeValue);
		if (m_iChangeType == (_int)POSITION)
		{
			_vector vPos = m_pCurrChild->Get_Transform()->Get_Position();
			vPos.m128_f32[1] += m_fChangeValue;
			m_pCurrChild->Set_Pos(vPos.m128_f32[0], vPos.m128_f32[1]);
		}
	}
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		if (m_iChangeType == (_int)SCALE)
			m_pCurrChild->Change_SizeBottom(m_fChangeValue);
		if (m_iChangeType == (_int)ROTATION)
			m_pCurrChild->Get_Transform()->Turn(_vector({ 1.f, 0.f, 0.f, 0.f }), -m_fChangeValue);
		if (m_iChangeType == (_int)POSITION)
		{
			_vector vPos = m_pCurrChild->Get_Transform()->Get_Position();
			vPos.m128_f32[1] += -m_fChangeValue;
			m_pCurrChild->Set_Pos(vPos.m128_f32[0], vPos.m128_f32[1]);
		}
	}
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		if (m_iChangeType == (_int)SCALE)
			m_pCurrChild->Change_SizeLeft(m_fChangeValue);
		if (m_iChangeType == (_int)ROTATION)
			m_pCurrChild->Get_Transform()->Turn(_vector({ 0.f, 0.f, 1.f, 0.f }), -m_fChangeValue);
		if (m_iChangeType == (_int)POSITION)
		{
			_vector vPos = m_pCurrChild->Get_Transform()->Get_Position();
			vPos.m128_f32[0] += -m_fChangeValue;
			m_pCurrChild->Set_Pos(vPos.m128_f32[0], vPos.m128_f32[1]);
		}
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		if (m_iChangeType == (_int)SCALE)
			m_pCurrChild->Change_SizeRight(m_fChangeValue);
		if (m_iChangeType == (_int)ROTATION)
			m_pCurrChild->Get_Transform()->Turn(_vector({ 0.f, 0.f, 1.f, 0.f }), m_fChangeValue);
		if (m_iChangeType == (_int)POSITION)
		{
			_vector vPos = m_pCurrChild->Get_Transform()->Get_Position();
			vPos.m128_f32[0] += m_fChangeValue;
			m_pCurrChild->Set_Pos(vPos.m128_f32[0], vPos.m128_f32[1]);
		}
	}*/



	/* SHIFT_L */
	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LB))
		{
			m_tChild_Desc.fPositionX = (_float)m_pt.x;
			m_tChild_Desc.fPositionY = (_float)m_pt.y;
			Create_Child(m_tChild_Desc);
			
		}
		if (m_pGameInstance->Key_Pressing(DIK_0))
		{
			m_fChangeValue = 0.1f;
		}
		if (m_pGameInstance->Key_Pressing(DIK_MINUS))
		{
			m_fChangeValue += 0.1f;
		}
		if (m_pGameInstance->Key_Pressing(DIK_EQUALS))
		{
			m_fChangeValue -= 0.1f;
			if (m_fChangeValue <= 0.f)
				m_fChangeValue = 0.f;
		}
	}

	/* DIK_LALT */
	if (m_pGameInstance->Key_Pressing(DIK_LALT))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_LB))
		{
			if (m_pCurrSelectUI != nullptr)
			{
				m_pCurrSelectUI->Moving_Picking_Point(m_pt);

				if (m_pSelectedKeyframe != nullptr)
				{
					m_pSelectedKeyframe->vPos.x = m_pt.x - g_iWinSizeX * 0.5f;
					m_pSelectedKeyframe->vPos.y = -m_pt.y + g_iWinSizeY * 0.5f;

					// m_pCurrSelectUI->Moving_Picking_Point(m_pt);
				}
			}
		}

		/* 키입력하고 클릭하는 기능보다 아래 존재하게 해야한다. */
		if (m_pGameInstance->Mouse_Down(DIM_RB))
		{
			if (!m_vecParentObject.empty())
			{
				_int iParentSize = (_int)m_vecParentObject.size();
				for (_int i = 0; i < iParentSize; ++i)
				{
					if (dynamic_cast<CUI*>(m_vecParentObject[i])->Get_Pick())
					{
						m_iSelected_ParentObjectIndex = i;
						m_pCurrParent = dynamic_cast<CUI*>(m_vecParentObject[i]);
						m_pCurrSelectUI = m_pCurrParent;
						m_eUIType = PARENT;
						return;
					}
				}
			}
			if (!m_vecChildObject.empty())
			{
				_int iChildSize = (_int)m_vecChildObject.size();
				for (_int i = 0; i < iChildSize; ++i)
				{
					if (dynamic_cast<CUI*>(m_vecChildObject[i])->Get_Pick())
					{
						if (dynamic_cast<CUI*>(m_vecChildObject[i])->Get_Kind() == CUI::TEXT)
						{
							m_iSelected_ChildObjectIndex = i;
							m_pCurrChild = dynamic_cast<CUI*>(m_vecChildObject[i]);
							m_pCurrSelectUI = dynamic_cast<CUI_Text*>(m_pCurrChild);
							m_eUIType = TEXT;
							return;
						}

						m_iSelected_ChildObjectIndex = i;
						m_pCurrChild = dynamic_cast<CUI*>(m_vecChildObject[i]);
						m_pCurrSelectUI = m_pCurrChild;
						m_eUIType = CHILD;
						return;
					}
				}
			}
			if (m_vecParentGroup != nullptr)
			{
				_int iGroupSize = (_int)(*m_vecParentGroup).size();
				for (_int i = 0; i < iGroupSize; ++i)
				{
					if (dynamic_cast<CUI*>((*m_vecParentGroup)[i])->Get_Pick())
					{
						m_iSelected_GroupObjectIndex = i;
						m_pCurrGroup = dynamic_cast<CUI*>((*m_vecParentGroup)[i]);
						m_pCurrSelectUI = m_pCurrGroup;
						m_eUIType = GROUP;
						return;
					}
				}
			}
			m_pCurrSelectUI = nullptr;
		}
	}

	/* Control_L */
	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		if (m_pGameInstance->Key_Down(DIK_S))
		{
			m_bShortOff = true;
			m_strDialogPath = "../Bin/DataFiles/Data_UI/";
			m_eDialogType = CImgui_Window::SAVE_DIALOG;
			OpenDialog(CImgui_Window::IMGUI_UITOOL_WINDOW);
		}

		if (m_pGameInstance->Key_Down(DIK_D))
		{
			Create_Child(m_tChild_Desc);
		}

		if (m_pGameInstance->Key_Down(DIK_L))
		{
			m_strDialogPath = "../Bin/DataFiles/Data_UI/";
			m_eDialogType = CImgui_Window::LOAD_DIALOG;
			OpenDialog(CImgui_Window::IMGUI_UITOOL_WINDOW);
		}

		if (m_pGameInstance->Key_Down(DIK_M))
		{
			Create_TargetTexture();
		}

		if (m_vecTimeline == nullptr)
			return;

		// "G" 키 : 현재 마우스 위치에 키프레임을 생성
		if (m_pGameInstance->Key_Down(DIK_G))
		{
			// 새로운 키프레임을 추가할 때 현재 시간과 가장 가까운 키프레임을 찾는다.
			float minDist = FLT_MAX;
			int insertIndex = 0;

			for (int i = 0; i < (*m_vecTimeline).size(); ++i)
			{
				float dist = fabsf(currentTime - (*m_vecTimeline)[i].fTime);

				if (dist < minDist)
				{
					minDist = dist;
					insertIndex = i;
				}
			}

			// 현재 마우스 위치
			ImVec2 mousePos = ImGui::GetMousePos();

			// 애니메이션 타임 라인 위치
			ImVec2 CurtimelinePos = ImGui::GetCursorScreenPos();
			CurtimelinePos.x += 0.f;
			CurtimelinePos.y += 30.f;

			// 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
			float relativeMouseX = (mousePos.x - CurtimelinePos.x) / timelineSize.x;

			// 마우스 위치를 애니메이션 타임 라인 상의 상대적인 위치로 변환
			float relativeMouseY = (mousePos.y - CurtimelinePos.y) / timelineSize.y;

			// 벨류 값을 계산합니다. 예를 들어, 상대적인 마우스 위치를 벨류 값으로 사용할 수 있음
			float newValue = 0.7f - ImClamp(relativeMouseY, 0.0f, 1.0f); // 벨류 값 범위를 0에서 1로 클램핑

			// 시간 값을 계산하여 새로운 키프레임을 생성
			float newTime = relativeMouseX * MaxTime; // MaxTime은 최대 시간 값

			// 새로운 키프레임을 생성하고 삽입.
			CUI::UIKEYFRAME newKeyframe = { newTime, newValue, 0, false, false };
			newKeyframe.fTime = newTime;
			newKeyframe.fValue = newValue;
			newKeyframe.vKeyFramePos.x = relativeMouseX;
			newKeyframe.vKeyFramePos.y = relativeMouseY;
			(*m_vecTimeline).push_back(newKeyframe);

			// 새로운 키프레임의 시간과 가장 가까운 키프레임을 찾는다. ( 끝 번호 인덱스로 선택됨 일단 보류)
			float minDistNew = FLT_MAX;
			int insertIndexNew = 0;

			for (int i = 0; i < (*m_vecTimeline).size(); ++i)
			{
				float dist = fabsf(newTime - (*m_vecTimeline)[i].fTime);

				if (dist < minDistNew)
				{
					minDistNew = dist;

					insertIndexNew = i;
					closestKeyframeIndex = insertIndexNew;
				}
			}
		}

		// "B" 키 : 현재 시간 위치에 키프레임을 생성
		if (m_pGameInstance->Key_Down(DIK_B))
		{
			// 애니메이션 타임 라인 위치
			ImVec2 CurtimelinePos = ImGui::GetCursorScreenPos();
			CurtimelinePos.x += 0.f;
			CurtimelinePos.y += 30.f;

			// 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
			float relativeMouseX = (CurtimelinePos.x) / timelineSize.x;

			// 마우스 위치를 애니메이션 타임 라인 상의 상대적인 위치로 변환
			float relativeMouseY = (CurtimelinePos.y) / timelineSize.y;

			// 새로운 키프레임을 생성하고 삽입합니다.
			CUI::UIKEYFRAME newKeyframe;
			newKeyframe.fTime = currentTime;
			newKeyframe.fValue = 0.f;
			newKeyframe.vKeyFramePos.x = relativeMouseX;
			newKeyframe.vKeyFramePos.y = relativeMouseY;
			(*m_vecTimeline).push_back(newKeyframe);

			float minDist = FLT_MAX;
			int insertIndex = 0;

			for (int i = 0; i < (*m_vecTimeline).size(); ++i)
			{
				float dist = fabsf(currentTime - (*m_vecTimeline)[i].fTime);

				if (dist < minDist)
				{
					minDist = dist;
					insertIndex = i;
					closestKeyframeIndex = insertIndex;
				}
			}
		}
	}


	//if (false == m_bCheckImguiRect_Child &&
	//	false == m_bCheckImguiRect_UI_Animation &&
	//	false == m_bCheckImguiRect_ValueChange &&
	//	false == m_bCheckImguiRect_Info &&
	//	false == m_bCheckImguiRect_UI_Info &&
	//	false == m_bCheckImguiRect_UI_TimeLine)
	//{

	//}
}

void CWindow_UITool::Layer_List()
{

	if (ImGui::BeginListBox("Layer"))
	{
		for (_int i = 0; i < 3; i++)
		{
			const bool is_selected = (m_iCurrLayerNum == i);
			if (ImGui::Selectable(m_strLayer[i].c_str(), is_selected))
				m_iCurrLayerNum = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
				m_tChild_Desc.strLayerTag = m_strLayer[is_selected].c_str();
				m_tParent_Desc.strLayerTag = m_strLayer[is_selected].c_str();
			}
		}

		ImGui::EndListBox();
	}
}

void CWindow_UITool::Texture_List()
{
	static int	Texture_idx = 0;
	_int		iTextureTagSize = (_int)m_vecImagePaths.size();

	if (ImGui::BeginListBox("Texture"))
	{
		for (_int i = 0; i < iTextureTagSize; i++)
		{
			const bool is_selected = (m_iSelectedPathIndex == i);
			if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(m_vecImagePaths[i]->strFileName)), is_selected))
			{
				m_iSelectedPathIndex = i;

				//// 문자열 중복 비교
				//AddIndexNumber(*m_vecImagePaths[m_iSelectedPathIndex]); // 오브젝트 테그 결정
				/* 프로토 테그 설정 */
				m_tChild_Desc.strProtoTag = m_vecImagePaths[m_iSelectedPathIndex]->strFileName;
				m_tParent_Desc.strProtoTag = m_vecImagePaths[m_iSelectedPathIndex]->strFileName;
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	///* 미리보기 */
	//ImGui::Image((void*)m_vecTexture[m_iSelectedPathIndex]->SRV_Texture, ImVec2((_float)m_vecTexture[m_iSelectedPathIndex]->iImage_Width, (_float)m_vecTexture[m_iSelectedPathIndex]->iImage_Height));
	//m_tChild_Desc.strProtoTag = m_vecImagePaths[m_iSelectedPathIndex]->strFileName; // 리스트 박스 밖으로 뺀 이유 : 리스트에서 선택안했을 때도 생성 가능하도록 계속 갱신하게함
}

void CWindow_UITool::Setting_Parent()
{
	//ImGui::CollapsingHeader("Setting_Parent");

	///* Mod */
	//ImGui::SeparatorText(u8"변경 모드 설정");
	//ImGui::RadioButton("Scale", &m_iChangeType, 1);
	//ImGui::RadioButton("Rotation", &m_iChangeType, 2);
	//ImGui::RadioButton("Position", &m_iChangeType, 3);

	///* Scale */
	//ImGui::SeparatorText(u8"크기 변경");
	//ImGui::InputFloat("ScaleX", &m_fParent_Scale.x);
	//ImGui::InputFloat("ScaleY", &m_fParent_Scale.y);
	//m_tParent_Desc.fScaleX = m_fParent_Scale.x;
	//m_tParent_Desc.fScaleY = m_fParent_Scale.y;

	///* Rotation */
	//ImGui::SeparatorText(u8"회전 변경");
	//ImGui::InputFloat("RotationZ", &m_fParent_Rotation.z);
	//m_tParent_Desc.fRotationZ = m_fParent_Rotation.z;

	///* Position*/
	//ImGui::SeparatorText(u8"위치 변경");
	//ImGui::InputFloat("PositionX", &m_fParent_Position.x);
	//ImGui::InputFloat("PositionY", &m_fParent_Position.y);
	//m_tParent_Desc.fPositionX = m_fParent_Position.x;
	//m_tParent_Desc.fPositionY = m_fParent_Position.y;


	//ImGui::Separator();


	if (m_pCurrParent != nullptr)
	{
		_vector vPosition = dynamic_cast<CUI*>(m_pCurrParent)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vOrthoPos = m_pGameInstance->Convert_Orthogonal(vPosition);

		Set_GuizmoCamView();
		Set_GuizmoCamProj();
		Set_GuizmoUI(m_pCurrParent);
	}

	ImGui::Dummy(ImVec2(0, 5)); // 공백

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
}

HRESULT CWindow_UITool::Create_Parent(CUI::UI_DESC pUIDesc)
{
	wstring strCloneProto = TEXT("");
	m_pGameInstance->String_To_WString(m_tParent_Desc.strCloneTag, strCloneProto);

	PATHINFO* tChild_Desc = new PATHINFO;

	// 문자열 중복 비교
	m_tParent_Desc.strObjectName;
	m_tParent_Desc.bParent = true;
	CUI* m_pCurrObject = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, ConvertToWideString(m_strLayer[m_iCurrLayerNum]), strCloneProto, &m_tParent_Desc));
	Add_ParentList(m_tParent_Desc);
	m_vecParentObject.push_back(m_pCurrObject);
	m_pCurrParent = m_pCurrObject;

	return S_OK;
}

void CWindow_UITool::Delete_Parent()
{
	if (m_vecParentObject.empty())
		return;
	if (m_vecParentObjectName.empty())
		return;

	dynamic_cast<CUI*>(m_vecParentObject[m_iSelected_ParentObjectIndex])->Set_Dead(true);
	m_vecParentObject.erase(m_vecParentObject.begin() + m_iSelected_ParentObjectIndex);

	if (!m_vecParentObjectName.empty())
		m_vecParentObjectName.erase(m_vecParentObjectName.begin() + m_iSelected_ParentObjectIndex); // 오브젝트 목록 이름 삭제

	if (m_iSelected_ParentObjectIndex > 0 &&
		m_iSelected_ParentObjectIndex <= m_vecParentObject.size())
		--m_iSelected_ParentObjectIndex;

	if (m_vecParentObject.empty())
	{
		m_pCurrParent = nullptr;
	}
	else
	{
		m_pCurrParent = dynamic_cast<CUI*>(m_vecParentObject[m_iSelected_ParentObjectIndex]);
	}

	if (m_pCurrSelectUI)
		m_pCurrSelectUI = nullptr; // 현재 선택돼있는 녀석이 죽었으니 주소 비워주자.
}

void CWindow_UITool::Parent_Class(_float fTimeDelta)
{
	if (m_vecParent.empty())
		return;

	_int iParentClassTagSize = (_int)m_vecParent.size();

	if (ImGui::BeginListBox("Parent Class"))
	{
		for (_int i = 0; i < iParentClassTagSize; i++)
		{
			const bool is_selected = (m_iSelected_ParentClassIndex == i);
			if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(m_vecParent[i].c_str())), is_selected))
			{
				m_iSelected_ParentClassIndex = i;

				/* 클래스 테그 설정 */
				m_tParent_Desc.strCloneTag = "Prototype_GameObject_UI_";
				m_tParent_Desc.strCloneTag += m_vecParent[m_iSelected_ParentClassIndex].c_str();
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
}

void CWindow_UITool::Parent_Object(_float fTimeDelta)
{

	if (ImGui::Button("Parent_Create"))
	{
		Create_Parent(m_tParent_Desc);	// 부모생성
	}

	ImGui::SameLine();

	if (ImGui::Button("Parent_Delete"))
	{
		Delete_Parent();
	}

	if (ImGui::Button("Parent_Delete_ALL")) 
	{
		if (!m_vecParentObject.empty())
		{
			m_vecParentObject.clear();
			m_vecParentObjectName.clear();
			m_iSelected_ParentObjectIndex = 0;

			if (m_vecParentObject.empty())
			{
				m_pCurrParent = nullptr;
				m_pCurrSelectUI = nullptr;
			}
		}
	}


	/* 지워지고 갱신 */
	_int		iParentObjectTagSize = (_int)m_vecParentObjectName.size();

	if (ImGui::BeginListBox("Parent Object"))
	{
		for (_int i = 0; i < iParentObjectTagSize; i++)
		{
			const bool is_selected = (m_iSelected_ParentObjectIndex == i);
			if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(m_vecParentObjectName[i]->strFileName)), is_selected))
			{
				m_iSelected_ParentObjectIndex = i;
				m_pCurrParent = dynamic_cast<CUI*>(m_vecParentObject[m_iSelected_ParentObjectIndex]);
				m_pCurrSelectUI = m_pCurrParent;
				m_pCurrSelectGameObject = m_vecParentObject[m_iSelected_ParentObjectIndex];
#ifdef _DEBUG
				/* (컨테이너의 주소를 받아오는건 릴리즈 모드에서 터지는 버그가있음. 툴용) */
				m_vecParentGroup = m_pCurrParent->Get_vecUIParts();
#endif // DEBUG
				m_iSelected_GroupObjectIndex = 0; // 새로 선택했으니까 초기화 해주자.
				m_eUIType = PARENT;
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	ImGui::Spacing();

	if (ImGui::Button("Group_Delete"))
	{
		Delete_Group();
	}
	
	if (m_vecParentGroup != nullptr)
	{
		/* 지워지고 갱신 */
		if (!m_vecParentGroup->empty())
		{
			_int		iGroupTagSize = (_int)m_vecParentGroup->size();

			if (ImGui::BeginListBox("Group Object"))
			{
				for (_int i = 0; i < iGroupTagSize; i++)
				{
					string Name = (*m_vecParentGroup)[i]->Get_ObjectNameTag();
					const bool is_selected = (m_iSelected_GroupObjectIndex == i);
					if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString((*m_vecParentGroup)[i]->Get_ObjectNameTag())), is_selected))
					{
						//m_iSelected_GroupObjectIndex = is_selected; // bug : is_selected는 bool인데 인덱스로 주고있었네..
						m_iSelected_GroupObjectIndex = i;
						m_pCurrGroup = (*m_vecParentGroup)[m_iSelected_GroupObjectIndex];
						m_pCurrSelectUI = m_pCurrGroup;
						m_eUIType = GROUP;
					}
					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
	}
	else
	{
		ImGui::Text("Not Group");
	}
	ImGui::Spacing();
}

void CWindow_UITool::Setting_Child()
{
	//ImGui::CollapsingHeader("2D_Setting");

	///* Mod */
	//ImGui::SeparatorText(u8"변경 모드 설정");
	//ImGui::RadioButton("Scale", &m_iChangeType, 1);
	//ImGui::RadioButton("Rotation", &m_iChangeType, 2);
	//ImGui::RadioButton("Position", &m_iChangeType, 3);

	///* Rotation */
	//ImGui::SeparatorText(u8"회전 변경");
	//ImGui::InputFloat("ScaleX", &m_fParent_Scale.x);
	//ImGui::InputFloat("ScaleY", &m_fParent_Scale.y);

	///* Scale */
	//ImGui::SeparatorText(u8"크기 변경");
	//ImGui::InputFloat("ScaleX", &m_fChild_Scale.x);
	//ImGui::InputFloat("ScaleY", &m_fChild_Scale.y);

	///* Position*/
	//ImGui::SeparatorText(u8"위치 변경");
	//ImGui::InputFloat("PositionX", &m_fChild_Possition.x);
	//ImGui::InputFloat("PositionY", &m_fChild_Possition.y);

	//ImGui::DragFloat("PosZ", (*m_pCurrSelectUIGet_UIDesc().fPositionZ))
	//m_pCurrSelectUI->Set_PosZ()

	if (m_pCurrSelectUI != nullptr)
	{
		ImGui::Text(ConverWStringtoC(ConvertToWideString(m_pCurrSelectUI->Get_UIDesc().strUIName)));
	}

	if (ImGui::Button("PlayAnim ALL"))
	{
		if (!m_vecChildObject.empty())
		{
			for (auto& iter : m_vecChildObject)
			{
				dynamic_cast<CUI*>(iter)->Set_AnimPlay(true);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("StopAnim ALL"))
	{
		if (!m_vecChildObject.empty())
		{
			for (auto& iter : m_vecChildObject)
			{
				dynamic_cast<CUI*>(iter)->Set_AnimPlay(false);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("ResetAnim ALL"))
	{
		if (!m_vecChildObject.empty())
		{
			for (auto& iter : m_vecChildObject)
			{
				dynamic_cast<CUI*>(iter)->Set_AnimPlay(false);
				dynamic_cast<CUI*>(iter)->Set_CurrTime(0.f);
			}
		}
	}

	if (ImGui::Button("Play ALL"))
	{
		if (!m_vecChildObject.empty())
		{
			for (auto& iter : m_vecChildObject)
			{
				dynamic_cast<CUI*>(iter)->Set_Active(true);
				dynamic_cast<CUI*>(iter)->Set_AnimPlay(true);
				dynamic_cast<CUI*>(iter)->Set_CurrTime(0.f);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop ALL"))
	{
		if (!m_vecChildObject.empty())
		{
			for (auto& iter : m_vecChildObject)
			{
				dynamic_cast<CUI*>(iter)->Set_Active(false);
				dynamic_cast<CUI*>(iter)->Set_AnimPlay(false);
				dynamic_cast<CUI*>(iter)->Set_CurrTime(0.f);
			}
		}
	}

	if (ImGui::Button("STATE UI"))
		CData_Manager::GetInstance()->Set_GameState(GAME_STATE::UI);
	ImGui::SameLine();
	if (ImGui::Button("STATE GAMEPLAY"))
		CData_Manager::GetInstance()->Set_GameState(GAME_STATE::GAMEPLAY);

	if (m_pCurrSelectUI != nullptr)
	{
		/*					Render				*/
		/* Back == 9, Center == 10, Front == 11 */
		ImGui::Text("RenderGroup");
		ImGui::RadioButton("Back", m_pCurrSelectUI->Get_RenderGroup(), 9);
		ImGui::SameLine();
		ImGui::RadioButton("Center", m_pCurrSelectUI->Get_RenderGroup(), 10);
		ImGui::SameLine();
		ImGui::RadioButton("Front", m_pCurrSelectUI->Get_RenderGroup(), 11);

		ImGui::RadioButton("First", m_pCurrSelectUI->Get_RenderGroup(), 12);
		ImGui::SameLine();
		ImGui::RadioButton("Second", m_pCurrSelectUI->Get_RenderGroup(), 13);
		ImGui::SameLine();
		ImGui::RadioButton("Third", m_pCurrSelectUI->Get_RenderGroup(), 14);
		ImGui::SameLine();
		ImGui::RadioButton("Fourth", m_pCurrSelectUI->Get_RenderGroup(), 15);
		ImGui::SameLine();
		ImGui::RadioButton("PopUP", m_pCurrSelectUI->Get_RenderGroup(), 16);

		if (ImGui::InputText("UI_NAME", m_cName, sizeof(m_cName))) // 문자열 저장해야함
		{
			wstring str;
			str = wstring(m_cName, &m_cName[strlen(m_cName)]);
			
			m_pCurrSelectUI->Set_UIName(WStringToString(str));

			//m_pCurrSelectUI->Set_CharToStringUIName(m_cName);
			//m_pGameInstance->WString_To_String(ConverCtoWC(), m_pCurrSelectUI->Get_UIDesc().strUIName.c_str());
		}
		
		if (ImGui::Button("ShowInterface"))
			m_bShowInterface = !m_bShowInterface;

		CData_Manager::GetInstance()->Set_ShowInterface(m_bShowInterface);

		if (ImGui::InputFloat("PosZ", &m_fPosZ, 0.01f, 0.01f))
		{
			m_pCurrSelectUI->Set_PosZ(m_fPosZ);
		}

		ImGui::Checkbox("World", m_pCurrSelectUI->Get_WorldUI());
		ImGui::SameLine();
		if (ImGui::Button("Proj_Change"))
			m_pCurrSelectUI->ChangeProj();


		if (ImGui::InputInt("MaskNum", &m_iMaskNum))
		{
			m_pCurrSelectUI->Set_MaskNum(m_iMaskNum);
		}

		if (ImGui::InputInt("NoiseNum", &m_iNoiseNum))
		{
			m_pCurrSelectUI->Set_NoiseNum(m_iNoiseNum);
		}

		if (m_pCurrSelectUI != nullptr)
		{
			if (m_pCurrSelectUI->Get_UIDesc().strCloneTag == "Prototype_GameObject_UI_MoveEffect")
			{
				ImGui::InputFloat(u8"LeftSize", m_pCurrSelectUI->Get_LeftSize());

				ImGui::InputFloat(u8"BottomSize", m_pCurrSelectUI->Get_BottomSize());

				ImGui::InputFloat(u8"Speed", m_pCurrSelectUI->Get_ChangeValue());

				ImGui::InputFloat(u8"OriginPoint", m_pCurrSelectUI->Get_OriginPoint());

			}
		}

		/* 디퓨즈 색상혼합 모드_Mesh */
		if (ImGui::CollapsingHeader(" Color Blend_Mesh "))
		{
			ImGui::RadioButton(u8"곱하기_Mesh", &m_iColor_Mode_Mesh, 0);
			ImGui::RadioButton(u8"스크린_Mesh", &m_iColor_Mode_Mesh, 1);
			ImGui::RadioButton(u8"오버레이_Mesh", &m_iColor_Mode_Mesh, 2);
			ImGui::RadioButton(u8"더하기_Mesh", &m_iColor_Mode_Mesh, 3);
			ImGui::RadioButton(u8"번_Mesh", &m_iColor_Mode_Mesh, 4);
			ImGui::RadioButton(u8"혼합안함_Mesh", &m_iColor_Mode_Mesh, 5);
			if (0 == m_iColor_Mode_Mesh)
				m_pCurrSelectUI->Set_ColorMode(MODE_COLOR::MUL);
			else if (1 == m_iColor_Mode_Mesh)
				m_pCurrSelectUI->Set_ColorMode(MODE_COLOR::SCREEN);
			else if (2 == m_iColor_Mode_Mesh)
				m_pCurrSelectUI->Set_ColorMode(MODE_COLOR::OVERLAY);
			else if (3 == m_iColor_Mode_Mesh)
				m_pCurrSelectUI->Set_ColorMode(MODE_COLOR::ADD);
			else if (4 == m_iColor_Mode_Mesh)
				m_pCurrSelectUI->Set_ColorMode(MODE_COLOR::BURN);
			else if (5 == m_iColor_Mode_Mesh)
				m_pCurrSelectUI->Set_ColorMode(MODE_COLOR::MODE_COLOR_END);


			// 디퓨즈에 곱할 색 (색 변경)
			if (ImGui::ColorEdit4("Color_Mul_Mesh", m_fColor_Mul_Mesh, ImGuiColorEditFlags_None))
			{
				m_pCurrSelectUI->Set_DiffuseColor(m_fColor_Mul_Mesh[0], m_fColor_Mul_Mesh[1], m_fColor_Mul_Mesh[2], m_fColor_Mul_Mesh[3]);
			}

			ImGui::InputFloat("Alpha", &m_fColor_Mul_Mesh[3], 0.01f, 0.01f);

			ImGui::SeparatorText("");
		}

		string strPath = "";
		//if (ImGui::InputText("AnimPath", m_cPath, sizeof(m_cPath)))
		//	strPath = string(m_cPath, &m_cPath[strlen(m_cPath)]);

		ImGui::SameLine();

		
		if (ImGui::InputText("AnimFileName", m_cFileName, sizeof(m_cFileName)))
			m_strFileName = string(m_cFileName, &m_cFileName[strlen(m_cFileName)]);
	
		if (ImGui::Button("Save_Animation"))
			Save_Animation(strPath, m_strFileName);

		if (ImGui::Button("Change_Animation"))
			Change_Animation(m_strFileName);
	}


	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백

	if (m_pCurrSelectUI != nullptr)
	{
		Set_GuizmoCamView();
		Set_GuizmoCamProj();

		if (*m_pCurrSelectUI->Get_WorldUI() == true)
		{
			if (m_pCurrSelectGameObject != nullptr)
				Set_Guizmo(m_pCurrSelectGameObject);
		}
		else
		{
			Set_GuizmoUI(m_pCurrSelectUI);
		}
		
	}

	//ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
}

void CWindow_UITool::Current_Info()
{
	//ImGui::CollapsingHeader("2D_Setting");

	//ImGui::Separator();

	//ImGui::Dummy(ImVec2(0, 5)); // 공백

	//ImGui::Separator();

	//ImGui::Dummy(ImVec2(0, 5)); // 공백
}

void CWindow_UITool::Parent_List(_float fTimeDelta)
{
	Parent_Class(fTimeDelta);
	Parent_Object(fTimeDelta);
	Setting_Parent();
}

void CWindow_UITool::Child_List(_float fTimeDelta)
{
	if (m_vecImagePaths.empty() ||
		m_vecTexture.empty())
		return;

	//ImGui::RadioButton("UI", &m_iRenderGroup, (_int)CRenderer::RENDERGROUP::RENDER_UI); ImGui::SameLine();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;
	ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_HorizontalScrollbar;

	// Layer
	Layer_List();
	ImGui::Spacing();

	// Textuer
	Texture_List();
	ImGui::Spacing();

	// Child
	Child_Class(fTimeDelta);
	Child_Object(fTimeDelta);
	Setting_Child();
}

void CWindow_UITool::Child_Class(_float fTimeDelta)
{
	_int		ClassTagSize = (_int)m_vecClass.size();

	if (ImGui::BeginListBox("Child Class"))
	{
		for (_int i = 0; i < ClassTagSize; i++)
		{
			const bool is_selected = (m_iSelected_ChildClassIndex == i);
			if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(m_vecClass[i].c_str())), is_selected))
			{
				m_iSelected_ChildClassIndex = i;
				/* 클래스 테그 설정 */
				m_tChild_Desc.strCloneTag = "Prototype_GameObject_UI_";
				m_tChild_Desc.strCloneTag += m_vecClass[m_iSelected_ChildClassIndex].c_str();
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
}

void CWindow_UITool::Child_Object(_float fTimeDelta)
{

#pragma region Create/Delete
	if (ImGui::Button("Create_Child"))
	{
		Create_Child(m_tChild_Desc);
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete_Child"))
	{
		Delete_Child(fTimeDelta);
	}

	if (ImGui::Button("Delete_All"))
	{
		Delete_Child_All(fTimeDelta);
	}
#pragma endregion End

	/* 삭제후 인덱스 갱신 */
	_int		ObjectTagSize = (_int)m_vecChildObject.size();
	if (ImGui::BeginListBox("Child Object"))
	{
		for (_int i = 0; i < ObjectTagSize; i++)
		{
			const bool is_selected = (m_iSelected_ChildObjectIndex == i);
			if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(dynamic_cast<CUI*>(m_vecChildObject[i])->Get_ObjectNameTag())), is_selected))
			{
				m_iSelected_ChildObjectIndex = i;
				m_pCurrChild = dynamic_cast<CUI*>(m_vecChildObject[m_iSelected_ChildObjectIndex]);
				if (m_pCurrChild->Get_Kind() == CUI::TEXT)
				{
					m_pCurrSelectUI = dynamic_cast<CUI_Text*>(m_pCurrChild);
					m_pCurrSelectGameObject = m_vecChildObject[m_iSelected_ChildObjectIndex];
					m_eUIType = TEXT;
				}
				else
				{
					m_pCurrSelectUI = m_pCurrChild;
					m_pCurrSelectGameObject = m_vecChildObject[m_iSelected_ChildObjectIndex];
					m_eUIType = CHILD;
				}
				
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	ImGui::Spacing();
}

void CWindow_UITool::UI_ToolTip(_float fTimeDelta)
{
	/* 테스트 값 */
	_int	iTestNum = 1;
	_float	fTestValue1 = 33.f;
	_float	fTestValue2 = 44.f;

	/* 마우스 오버할 위치 */
	_float	fTestX = 50.f;
	_float	fTestY = 500.f;

	/* 마우스 오버할 범위 */
	_float	fHoverRangeX = 200.f;
	_float	fHoverRangeY = 200.f;

	/* 마우스 오버시 나오는 정보창의 크기 */
	_float	fScreenPosX = 20.f;
	_float	fScreenPosY = 40.f;

	//// 마우스가 해당 위치 위에 있는 경우에만 툴팁 표시 (마우스 오버)
	//if (ImGui::IsMouseHoveringRect(
	//	ImVec2(fTestX - fHoverRangeX, fTestY - fHoverRangeY),
	//	ImVec2(fTestX + fHoverRangeX, fTestY + fHoverRangeY)))
	//{

	//	// 표시할 정보
	//	ImGui::SetCursorScreenPos(ImVec2(fTestX + fScreenPosX, fTestY - fScreenPosY));
	//	ImGui::BeginTooltip();

	//	ImGui::EndTooltip(); // 툴팁 종료
	//}
}

// 이미지를 메모리에 로드하는 함수
std::vector<unsigned char> CWindow_UITool::UI_LoadImage(const std::string& filename, int& width, int& height, int& channels)
{
	// 파일 내용 읽기 (미리보기로 최대 100바이트까지만 출력)
	std::ifstream file(filename, std::ios::binary);
	if (file.is_open()) {
		char buffer[101]; // 100바이트 + 널 문자
		if (file.read(buffer, 100)) {
			buffer[file.gcount()] = '\0'; // 널 문자 추가
			std::cout << "Preview: " << buffer << std::endl;
		}
		file.close();
	}
	else {
		std::cerr << "Failed to open file: " << filename << std::endl;
	}

	// 파일 크기 확인
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// 파일 내용을 저장할 벡터 생성
	std::vector<unsigned char> imageData(fileSize);
	if (!file.read(reinterpret_cast<char*>(imageData.data()), fileSize)) {
		std::cerr << "Failed to read file: " << filename << std::endl;
		return {};
	}

	// 이미지 너비, 높이, 채널 수 설정 (예시로 고정 값 사용)
	width = 256;
	height = 256;
	channels = 4; // RGBA

	return imageData;
}

// Simple helper function to load an image into a DX11 texture with common settings
bool CWindow_UITool::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

// std::string 벡터를 const char* 배열로 변환하는 함수
std::vector<const char*> CWindow_UITool::ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector)
{
	std::vector<const char*> charArray;
	charArray.reserve(stringVector.size());
	for (const std::string& str : stringVector)
	{
		charArray.push_back(str.c_str());
	}
	return charArray;
}

std::wstring CWindow_UITool::ConvertToWideString(const std::string& ansiString)
{
	int wideStrLen = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
	if (wideStrLen == 0)
	{
		// 변환 실패 처리
		return L"Fail";
	}

	std::wstring wideString(wideStrLen, L'\0');
	MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideStrLen);

	return wideString;
}

std::string CWindow_UITool::WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(wstr);
}

char* CWindow_UITool::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* result = new char[size_needed];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);

	char* newResult = result;

	result = nullptr;
	delete[] result;

	return newResult;
}

wchar_t* CWindow_UITool::ConverCtoWC(char* str)
{
	_tchar* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, str, (_int)strlen(str) + (size_t)1, pStr, strSize);

	_tchar* newResult = pStr;

	pStr = nullptr;
	delete[] pStr;

	return newResult;
}

std::string CWindow_UITool::WideStringToString(const wchar_t* wideStr) 
{
	char ch[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wideStr, -1, ch, 260, &DefChar, NULL);

	string ss(ch);
	return ss;
	//// std::wstring으로부터 std::string으로 변환
	//std::wstring wstr(wideStr);
	//// std::string으로 변환
	//return std::string(wstr.begin(), wstr.end());

}

// 파일 이름만 추출하는 함수
std::string CWindow_UITool::GetFileName(const std::string& filePath) 
{
	size_t lastSlashPos = filePath.find_last_of("/");
	if (lastSlashPos != std::string::npos) 
	{
		return filePath.substr(lastSlashPos + 1);
	}
	else 
	{
		// 경로 구분자가 없을 경우 전체 경로를 반환
		return filePath;
	}
}

// _기준으로 이름만 추출하는 함수
std::string CWindow_UITool::GetUnderbarName(const std::string& filePath)
{
	size_t lastSlashPos = filePath.find_last_of("_");
	if (lastSlashPos != std::string::npos)
	{
		return filePath.substr(lastSlashPos + 1);
	}
	else
	{
		// 경로 구분자가 없을 경우 전체 경로를 반환
		return filePath;
	}
}

std::string CWindow_UITool::RemoveExtension(const std::string& filePath)
{
	size_t lastDotPos = filePath.find_last_of(".");
	if (lastDotPos != std::string::npos) 
	{
		return filePath.substr(0, lastDotPos);
	}
	else 
	{
		// 확장자가 없는 경우 그대로 반환
		return filePath;
	}
}

WCHAR* CWindow_UITool::StringTowchar(const std::string& str)
{
	// std::wstring으로 변환
	std::wstring wstr(str.begin(), str.end());
	// c_str() 함수를 사용하여 WCHAR* 포인터로 변환
	return const_cast<WCHAR*>(wstr.c_str());
}

void CWindow_UITool::LoadImgPath(const _tchar* folderPath)
{
	// 찾은 이미지 데이터를 받을 변수
	WIN32_FIND_DATA findData;

	// 폴더 경로
	wstring wfolderPath = (wstring)folderPath + L"/*.*";

	// 폴더 경로 저장 및 핸들 반환
	HANDLE hFind = FindFirstFile(wfolderPath.c_str(), &findData);

	// 핸들이 있을 경우
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// 하위 디렉토리
		vector<wstring> subDirectories;
		// 이미지 파일
		vector<wstring> imageFiles;
		do
		{
			//  파일의 속성 정보가 입력된다. (디렉토리인지 파일인지 등)
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
				{
					// 폴더 경로 = 경로 + / + 파일명
					wstring subFolderPath = (wstring)folderPath + L"/" + findData.cFileName;
					subDirectories.push_back(subFolderPath);
					LoadImgPath(subFolderPath.c_str());
				}
			}
			else
			{
				// 파일인 경우, 이미지 파일인지 확인하고 로드
				wstring filePath = (wstring)folderPath + L"/" + findData.cFileName;

				// 파일 확장자 확인
				if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
					/*wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||*/
					wcsstr(findData.cFileName, L".dds"))
				{
					PATHINFO* pPathInfo = new PATHINFO;

					/* 순서 확인하기 */
					pPathInfo->strFileName = RemoveExtension(WStringToString(findData.cFileName));
					pPathInfo->strFilePath = WStringToString(filePath);
					pPathInfo->iPathNum = m_iTextureNum;
					m_vecInitialPaths.push_back(pPathInfo);

					pPathInfo = nullptr;
					delete[] pPathInfo;

					m_iTextureNum++;
				}
			}
		} while (FindNextFile(hFind, &findData));

		FindClose(hFind);
	}
}

void CWindow_UITool::ImagePreview(_float fTimeDelta)
{

}

void CWindow_UITool::IndexCheck()
{
	if (m_iSelected_ChildObjectIndex <= 0)
		m_iSelected_ChildObjectIndex = 0;

	if (m_iSelectedPathIndex <= 0)
		m_iSelectedPathIndex = 0;
}

#pragma region Parent
void CWindow_UITool::Add_Create_Parts(CUI::UI_DESC tUIDesc)
{
	if (m_vecParentObject.empty())
		return;

		dynamic_cast<CUI*>(m_vecParentObject[m_iSelected_ParentObjectIndex])->Add_Create_Parts(&tUIDesc);
}

void CWindow_UITool::Add_Parts(CUI* pUI)
{
	if (m_vecParentObject.empty())
		return;

	CUI* pParent = dynamic_cast<CUI*>(m_vecParentObject[m_iSelected_ParentObjectIndex]);
	CUI* pChild = dynamic_cast<CUI*>(m_vecChildObject[m_iSelected_ChildObjectIndex]);

	if (pParent == nullptr ||
		pChild == nullptr)
		return;

	CTransform* pParentTransform = pParent->Get_Transform();
	pChild->Set_ParentTransform(pParentTransform);

	pParent->Add_Parts(pChild);
	//m_vecParentGroup.push_back(pChild);
	//if (!m_vecChildObjectName.empty())
	//	m_vecChildObjectName.erase(m_vecChildObjectName.begin() + m_iSelected_ChildObjectIndex); // 오브젝트 목록 이름 삭제
	if (!m_vecChildObject.empty())
		m_vecChildObject.erase(m_vecChildObject.begin() + m_iSelected_ChildObjectIndex); // 오브젝트 삭제

	if (m_iSelected_ChildObjectIndex > 0 &&
		m_iSelected_ChildObjectIndex <= m_vecChildObject.size())
		--m_iSelected_ChildObjectIndex;
}

void CWindow_UITool::Delete_Group()
{
	if (m_vecParentGroup == nullptr)
		return;

	if (m_vecParentGroup->empty())
		return;

	//m_vecChildObjectName.push_back(tChild_Desc); // 이름 중복 검사 후 처리된 테그값으로 넣어주자.
	if((*m_vecParentGroup)[m_iSelected_GroupObjectIndex] != nullptr)
		m_vecChildObject.push_back((*m_vecParentGroup)[m_iSelected_GroupObjectIndex]); //


	if (!(*m_vecParentGroup).empty())
	{
		(*m_vecParentGroup).erase((*m_vecParentGroup).begin() + m_iSelected_GroupObjectIndex); // 오브젝트 그룹에서 제외

		if (m_iSelected_GroupObjectIndex > 0 &&
			m_iSelected_GroupObjectIndex <= m_vecParentGroup->size())
		{
			--m_iSelected_GroupObjectIndex;
		}
	}

	if (m_pCurrSelectUI)
		m_pCurrSelectUI = nullptr; // 현재 선택돼있는 녀석이 죽었으니 주소 비워주자.


	//(*m_vecParentGroup)[m_iSelected_GroupObjectIndex]->Set_Dead(true);
}
#pragma endregion

HRESULT CWindow_UITool::Create_Child(CUI::UI_DESC pUIDesc)
{
	CGameObject* pGameObject = nullptr;
	CUI* pUI = nullptr;
	wstring strCloneProto = TEXT("");
	m_pGameInstance->String_To_WString(m_tChild_Desc.strCloneTag, strCloneProto);

	Add_ChildList(pUIDesc); // 먼저 중복검사 후 이름 지정해주기
	pUIDesc.iObjectNum = m_tChild_Desc.iObjectNum;
	pUIDesc.iShaderNum = m_tChild_Desc.iShaderNum;
	pUIDesc.strObjectName = m_tChild_Desc.strObjectName;
	pUIDesc.strCloneTag = m_tChild_Desc.strCloneTag;
	pUIDesc.strFilePath = m_tChild_Desc.strFilePath;
	pUIDesc.strLayerTag = m_strLayer[m_iCurrLayerNum];
	pUIDesc.strProtoTag = m_tChild_Desc.strProtoTag;
	pUIDesc.strMapTextureTag = m_tChild_Desc.strMapTextureTag;
	pUIDesc.bParent = false;
	pUIDesc.bWorld = m_tChild_Desc.bWorld;
	CUI* pCurrObject = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, ConvertToWideString(m_strLayer[m_iCurrLayerNum]), strCloneProto, &pUIDesc));


	m_vecChildObject.push_back(pCurrObject);
	m_pCurrChild = pCurrObject;

	//if(m_pCurrChild->Get_Kind() == CUI::TEXT)
	//	m_tTextInfo = dynamic_cast<CUI_Text*>(m_pCurrChild)->Get_TextInfo();

	return S_OK;
}

void CWindow_UITool::Delete_Child(_float fTimeDelta)
{
	if (m_vecChildObject.empty())
		return;

	// 오브젝트 삭제
	m_vecChildObject[m_iSelected_ChildObjectIndex]->Set_Dead(true); // Set_Dead로 이녀석을 없애줘야 Layer에 담겨있는 Object가 죽는다.

	/* Set_Dead를 요청하면 매니저에게 알아서 해당 객체를 지워달라고 한거니까 그 객체에 대한 삭제처리는 내가 따로 또 하면 안된다. (근데 왜 됐지?) */
	//Safe_Release(m_vecChildObject[m_iSelected_ChildObjectIndex]);
	m_vecChildObject.erase(m_vecChildObject.begin() + m_iSelected_ChildObjectIndex);

	/* 부모의 컨테이너에 담겨있는 녀석중 Dead가 true인 녀석은 지워준다. */
	//if (!m_vecParentObject.empty())
	//	dynamic_cast<CUI*>(m_vecParentObject[m_iSelected_ChildObjectIndex])->Parts_Delete();

	//if(!m_vecChildObjectName.empty())
	//	m_vecChildObjectName.erase(m_vecChildObjectName.begin() + m_iSelected_ChildObjectIndex); // 오브젝트 목록 이름 삭제

	if (m_vecTimeline)
		m_vecTimeline = nullptr;

	if (m_pCurrChild)
		m_pCurrChild = nullptr; // 현재 선택돼있는 녀석이 죽었으니 주소 비워주자.

	if (m_pCurrSelectUI)
		m_pCurrSelectUI = nullptr; // 현재 선택돼있는 녀석이 죽었으니 주소 비워주자.


	m_iSelected_ChildObjectIndex = m_iSelected_ChildObjectIndex - 1; // 현재 선택한 인덱스번째의 오브젝트를 삭제했으니, 선택된 인덱스도 뒤로 한칸 돌려주자.
}

void CWindow_UITool::Delete_Child_All(_float fTimeDelta)
{
	if (m_vecChildObject.empty())
		return;

	// 오브젝트 선택 해제
	m_pCurrSelectUI = nullptr;
	m_iSelected_ChildObjectIndex = 0;

	// 오브젝트 삭제
	for (auto& iter : m_vecChildObject)
		iter->Set_Dead(true);

	/* Set_Dead를 요청하면 매니저에게 알아서 해당 객체를 지워달라고 한거니까 그 객체에 대한 삭제처리는 내가 따로 또 하면 안된다. (근데 왜 됐지?) */
	//Safe_Release(m_vecChildObject[m_iSelected_ChildObjectIndex]);

	if (!m_vecChildObject.empty())
		m_vecChildObject.clear();

}

void CWindow_UITool::Add_ParentIndexNumber(PATHINFO& UI_Info)
{
	int		index = 0;
	_bool	isPath = false;

	if (!m_vecParentObjectName.empty())
	{
		for (auto& iter : m_vecParentObjectName)
		{
			if (iter->strFilePath == UI_Info.strFilePath)
			{
				isPath = true;
				break;
			}
		}

		// 문자열이 이미 존재하는 경우
		if (isPath)
		{
			for (auto& strFilePath : m_vecParentObjectName)
			{
				if (strFilePath->strFilePath == UI_Info.strFilePath)
				{
					/* 모두 순회하고 */
					if (strFilePath->iPathNum >= UI_Info.iPathNum) // 기존 컨테이너에 있는 마지막으로 비교한 녀석의 숫자보다 작거나 같을 경우
					{
						index = strFilePath->iPathNum + 1; // 마지막 녀석의 번호 + 1로 저장
					}

				}
			}
		}
	}
	else
	{
		// 새로운 문자열인 경우
		index = 0;
	}


	/* 테그 */
	UI_Info.strFileName = m_vecParent[m_iSelected_ParentClassIndex].c_str() + to_string(index);
	UI_Info.strObjectName = m_vecParent[m_iSelected_ParentClassIndex].c_str() + to_string(index);
	m_tParent_Desc.strObjectName = UI_Info.strObjectName;
	UI_Info.iPathNum = index;
}

//										현재 선택한 녀석의 전체 경로를 받아온다.
void CWindow_UITool::Add_ChildIndexNumber(PATHINFO& UI_Info)
{
	int		index = 0;
	_bool	isPath = false;

	if (!m_vecChildObject.empty())
	{
		for (auto& iter : m_vecChildObject)
		{
			if (dynamic_cast<CUI*>(iter)->Get_FilePathTag() == UI_Info.strFilePath)
			{
				isPath = true;
				break;
			}
		}
		// 문자열이 이미 존재하는 경우
		if (isPath)
		{
			for (auto& strFilePath : m_vecChildObject)
			{
				if (dynamic_cast<CUI*>(strFilePath)->Get_FilePathTag() == UI_Info.strFilePath)
				{
					/* 모두 순회하고 */
					if (dynamic_cast<CUI*>(strFilePath)->Get_ObjectNum() >= UI_Info.iPathNum) // 기존 컨테이너에 있는 마지막으로 비교한 녀석의 숫자보다 작거나 같을 경우
					{
						index = dynamic_cast<CUI*>(strFilePath)->Get_ObjectNum() + 1; // 마지막 녀석의 번호 + 1로 저장
					}

				}
			}
		}
	}
	else 
	{
		// 새로운 문자열인 경우
		index = 0;
		m_tChild_Desc.iObjectNum = 0;
	}

	{
		m_tChild_Desc.iObjectNum = index;
		UI_Info.strFileName = m_vecClass[m_iSelected_ChildClassIndex] + to_string(index);
		UI_Info.strObjectName = m_vecClass[m_iSelected_ChildClassIndex] + to_string(index);
		m_tChild_Desc.strObjectName = UI_Info.strObjectName;
		UI_Info.iPathNum = index;
	}
}

void CWindow_UITool::Add_ParentList(CUI::UI_DESC tIn_UI_Desc)
{
	PATHINFO* tUI_Desc = new PATHINFO;

	// 문자열 중복 비교
	tUI_Desc->strFileName = m_vecParent[m_iSelected_ParentClassIndex];
	tUI_Desc->strFilePath = tIn_UI_Desc.strFilePath;
	Add_ParentIndexNumber(*tUI_Desc); // 오브젝트 테그 결정

	m_vecParentObjectName.push_back(tUI_Desc); // 이름 중복 검사 후 처리된 테그값으로 넣어주자.

	tUI_Desc = nullptr;
	delete[] tUI_Desc;
}

void CWindow_UITool::Add_ChildList(CUI::UI_DESC tIn_UI_Desc)
{
	// error : 아래 Get_CloneGameObjects로 오브젝트를 가져올때 기존 오브젝트까지 모두 다시들고 오기 때문에, 함수를 따로 만들거나 클리어하고 담아주자
	//if (!m_vecChildObject.empty())
	//	m_vecChildObject.clear();

	PATHINFO* tChild_Desc = new PATHINFO;

	// 문자열 중복 비교
	tChild_Desc->strFileName = m_vecClass[m_iSelected_ChildClassIndex];
	tChild_Desc->strFilePath = tIn_UI_Desc.strFilePath;
	Add_ChildIndexNumber(*tChild_Desc); // 오브젝트 테그 결정

	//m_vecChildObjectName.push_back(tChild_Desc); // 이름 중복 검사 후 처리된 테그값으로 넣어주자.
	//m_pGameInstance->Get_CloneGameObjects(LEVEL_STATIC, &m_vecChildObject);

	tChild_Desc = nullptr;
	delete[] tChild_Desc;
}

void CWindow_UITool::Text_List()
{
	/* bug : Label 이름 중복되면 다 묶여버리니까 확인해주기 */

	if (ImGui::InputTextWithHint(u8"키값", u8"키값을 입력하세요.", ConverWStringtoC(ConvertToWideString(m_tTextInfo.strTextKey)), IM_ARRAYSIZE(ConverWStringtoC(ConvertToWideString(m_tTextInfo.strTextKey)))))
		m_tTextInfo.strTextKey = m_tTextInfo.strTextKey;

	//ImGui::Text(m_tTextInfo.strTextKey);

	if (ImGui::InputTextWithHint(u8"폰트", u8"폰트를 입력하세요.", ConverWStringtoC(ConvertToWideString(m_tTextInfo.strFontTag)), IM_ARRAYSIZE(ConverWStringtoC(ConvertToWideString(m_tTextInfo.strFontTag)))))
		m_tTextInfo.strFontTag = m_tTextInfo.strFontTag;
	//ImGui::Text(m_cTextFont);

	if(ImGui::InputTextWithHint(u8"텍스트", u8"텍스트를 입력하세요.", ConverWStringtoC(ConvertToWideString(m_tTextInfo.strText)), IM_ARRAYSIZE(ConverWStringtoC(ConvertToWideString(m_tTextInfo.strText)))))
		m_tTextInfo.strText = m_tTextInfo.strText;
	//ImGui::Text(m_cText);

	if (ImGui::Checkbox("Free Move", &m_bFreeMove))
	{
		if (m_pCurrSelectUI)
			dynamic_cast<CUI_Text*>(m_pCurrSelectUI)->Set_FreeMove(m_bFreeMove);
	}

	if (ImGui::DragFloat("Text PositionX", &m_tTextInfo.fPosX))
	{
		m_tTextInfo.fPosX = m_tTextInfo.fPosX;
	}

	if (ImGui::DragFloat("Text PositionY", &m_tTextInfo.fPosY))
	{
		m_tTextInfo.fPosY = m_tTextInfo.fPosY;
	}
	if (ImGui::DragFloat("Text Scale", &m_tTextInfo.fScale))
	{
		m_tTextInfo.fScale = m_tTextInfo.fScale;
	}
	if (ImGui::DragFloat("Text OriginX", &m_tTextInfo.vOrigin.x))
		m_tTextInfo.vOrigin.x = m_tTextInfo.vOrigin.x;
	if (ImGui::DragFloat("Text OriginY", &m_tTextInfo.vOrigin.y))
		m_tTextInfo.vOrigin.y = m_tTextInfo.vOrigin.y;
	if (ImGui::DragFloat("Text Rotation", &m_tTextInfo.fRotation))
		m_tTextInfo.fRotation = m_tTextInfo.fRotation;

	if (ImGui::DragFloat("Text ColorR", &m_tTextInfo.vColor.m128_f32[0]))
		m_tTextInfo.vColor.m128_f32[0] = m_tTextInfo.vColor.m128_f32[0];
	if (ImGui::DragFloat("Text ColorG", &m_tTextInfo.vColor.m128_f32[1]))
		m_tTextInfo.vColor.m128_f32[1] = m_tTextInfo.vColor.m128_f32[1];
	if (ImGui::DragFloat("Text ColorB", &m_tTextInfo.vColor.m128_f32[2]))
		m_tTextInfo.vColor.m128_f32[2] = m_tTextInfo.vColor.m128_f32[2];
	if (ImGui::DragFloat("Text ColorA", &m_tTextInfo.vColor.m128_f32[3]))
		m_tTextInfo.vColor.m128_f32[3] = m_tTextInfo.vColor.m128_f32[3];

	/* 갱신 */
	Change_Text(m_tTextInfo.strTextKey, m_tTextInfo.strFontTag, m_tTextInfo.strText, m_tTextInfo.fPosX, m_tTextInfo.fPosY, m_tTextInfo.vColor, m_tTextInfo.fScale, m_tTextInfo.vOrigin, m_tTextInfo.fRotation);

	if (ImGui::Button("Text Add"))
	{
		Text_Add();
	}

	ImGui::Spacing();

	if (!m_vecTextList.empty())
	{
		/* 삭제후 인덱스 갱신 */
		_int		iTextListSize = (_int)m_vecTextList.size();

		if (ImGui::BeginListBox("Text List"))
		{
			for (_int i = 0; i < iTextListSize; i++)
			{
				const bool is_selected = (m_iSelected_TextListIndex == i);
				if (ImGui::Selectable(ConverWStringtoC(ConvertToWideString(m_vecTextList[i].c_str())), is_selected))
				{
					m_iSelected_TextListIndex = i;
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		ImGui::Spacing();
	}

	if (ImGui::InputTextWithHint(u8"텍스트 테그 : ", u8"변경하고싶은 텍스트 테그 값을 입력하세요.", m_cChangeTextTag, IM_ARRAYSIZE(m_cChangeTextTag)))
		ImGui::Text(m_cChangeTextTag);

	if (ImGui::Button("Find Change"))
	{
		Find_Change(m_cChangeTextTag);
	}
}

void CWindow_UITool::Text_Add()
{
	if (m_pCurrSelectUI == nullptr)
		return;

	dynamic_cast<CUI_Text*>(m_pCurrSelectUI)->Add_Text(m_tTextInfo.strTextKey, m_tTextInfo.strFontTag, m_tTextInfo.strText,
														m_tTextInfo.fPosX, m_tTextInfo.fPosY, m_tTextInfo.vColor,
														m_tTextInfo.fScale, m_tTextInfo.vOrigin, m_tTextInfo.fRotation);
	m_vecTextList.push_back(m_tTextInfo.strTextKey); // 테그 목록
}

void CWindow_UITool::Find_Change(string strTextTag)
{
	if (m_pCurrSelectUI == nullptr)
		return;

	dynamic_cast<CUI_Text*>(m_pCurrSelectUI)->Find_Change(strTextTag);
	m_tTextInfo = *dynamic_cast<CUI_Text*>(m_pCurrSelectUI)->Find_Text(strTextTag);

	*m_cTextFont = *ConverWStringtoC(ConvertToWideString(m_tTextInfo.strFontTag));
	*m_cText = *ConverWStringtoC(ConvertToWideString(m_tTextInfo.strText));
	*m_cTextKey = *ConverWStringtoC(ConvertToWideString(m_tTextInfo.strTextKey));

	m_fPosX = m_tTextInfo.fPosX;
	m_fPosY = m_tTextInfo.fPosY;
	m_fScale = m_tTextInfo.fScale;
	m_vOrigin.x = m_tTextInfo.vOrigin.x;
	m_vOrigin.y = m_tTextInfo.vOrigin.y;
	m_fRotation = m_tTextInfo.fRotation;

	m_vColor.m128_f32[0] = m_tTextInfo.vColor.m128_f32[0];
	m_vColor.m128_f32[1] = m_tTextInfo.vColor.m128_f32[1];
	m_vColor.m128_f32[2] = m_tTextInfo.vColor.m128_f32[2];
	m_vColor.m128_f32[3] = m_tTextInfo.vColor.m128_f32[3];
}

void CWindow_UITool::Add_TextList(CUI::UI_DESC tUIDesc)
{

	// 문자열 중복 비교
	int		index = 0;
	_bool	isPath = false;

	if (!m_vecChildObject.empty())
	{
		for (auto& iter : m_vecChildObject)
		{
			if (dynamic_cast<CUI*>(iter)->Get_FilePathTag() == tUIDesc.strFilePath)
			{
				isPath = true;
				break;
			}
		}
		// 문자열이 이미 존재하는 경우
		if (isPath)
		{
			for (auto& strFilePath : m_vecChildObject)
			{
				if (dynamic_cast<CUI*>(strFilePath)->Get_FilePathTag() == tUIDesc.strFilePath)
				{
					index = dynamic_cast<CUI*>(strFilePath)->Get_ObjectNum() + 1; // 마지막 녀석의 번호 + 1로 저장
				}
			}
		}
	}
	else
	{
		// 새로운 문자열인 경우
		index = 0;
		m_tChild_Desc.iObjectNum = 0;
	}

	{
		tUIDesc.iObjectNum = index;
		tUIDesc.strObjectName = m_vecClass[m_iSelected_ChildClassIndex] + to_string(index);
	}

}

void CWindow_UITool::Change_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	if (m_pCurrSelectUI == nullptr)
		return;

	if (m_eUIType == CWindow_UITool::TEXT)
	{
		dynamic_cast<CUI_Text*>(m_pCurrSelectUI)->Change_Text(strTextKey, strFontTag, strText, fPosX, fPosY, vColor, fScale, vOrigin, fRotation);
	}
}

void CWindow_UITool::UI_Info()
{
	Menu_Info();
	Curr_Info();
}

HRESULT CWindow_UITool::Menu_Info()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Save"))
			{
				m_strDialogPath = "../Bin/DataFiles/Data_UI/";
				m_eDialogType = CImgui_Window::SAVE_DIALOG;
				OpenDialog(CImgui_Window::IMGUI_UITOOL_WINDOW);
			}
			if (ImGui::MenuItem("Load"))
			{
				m_strDialogPath = "../Bin/DataFiles/Data_UI/";
				m_eDialogType = CImgui_Window::LOAD_DIALOG;
				OpenDialog(CImgui_Window::IMGUI_UITOOL_WINDOW);
			}
			if (ImGui::MenuItem("Target_Save"))
			{
				Create_TargetTexture();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	return S_OK;
}

void CWindow_UITool::Curr_Info()
{
	ImGui::Text("Mouse Pos : %d, %d", m_pt.x, m_pt.y);

	if (m_pCurrSelectUI != nullptr)
	{
		if (ImGui::Button("Active"))
		{
			m_pCurrSelectUI->Set_Active(!m_pCurrSelectUI->Get_Active());
		}
		if (ImGui::Button("Set_Tool"))
		{
			m_pCurrSelectUI->Set_Tool(!m_pCurrSelectUI->Get_Active());
		}
	}

	CTransform* pTransformCom = nullptr;

	switch (m_eUIType)
	{
	case Client::CWindow_UITool::CHILD:
	{
		if (m_pCurrSelectUI != nullptr)
		{
			if (!m_vecChildObject.empty())
			{
				pTransformCom = dynamic_cast<CUI*>(m_pCurrSelectUI)->Get_Transform();
				ImGui::Text("Selected Child : ");
				ImGui::TextColored({ 1.f, 1.f, 1.f, 1.f }, dynamic_cast<CUI*>(m_vecChildObject[m_iSelected_ChildObjectIndex])->Get_ObjectNameTag().c_str());
				ImGui::SameLine();
				if (ImGui::Button("Parts_Add"))
				{
					Add_Parts(m_pCurrSelectUI); // 부모  <- 자식포함
				}
			}
		}
		break;
	}
	case Client::CWindow_UITool::PARENT:
	{
		if (m_pCurrSelectUI != nullptr)
		{
			if (!m_vecParentObjectName.empty())
			{
				pTransformCom = dynamic_cast<CUI*>(m_pCurrSelectUI)->Get_Transform();
				ImGui::Text("Selected Parent : ");
				ImGui::TextColored({ 1.f, 1.f, 1.f, 1.f }, m_vecParentObjectName[m_iSelected_ParentObjectIndex]->strFileName.c_str());
			}
		}
		break;
	}
	case Client::CWindow_UITool::GROUP:
	{
		if (m_pCurrSelectUI != nullptr)
		{
			if (m_vecParentGroup == nullptr)
				break;

			if (!m_vecParentGroup->empty())
			{
				pTransformCom = dynamic_cast<CUI*>(m_pCurrSelectUI)->Get_Transform();
				ImGui::Text("Group Child : ");
				if (m_iSelected_GroupObjectIndex < (*m_vecParentGroup).size() &&
					m_iSelected_GroupObjectIndex >= 0)
				{
					if (m_iSelected_GroupObjectIndex >= (*m_vecParentGroup).size())
						m_iSelected_GroupObjectIndex = _int((*m_vecParentGroup).size() - 1);
					if (m_iSelected_GroupObjectIndex < 0)
						m_iSelected_GroupObjectIndex = 0;

					ImGui::TextColored({ 1.f, 1.f, 1.f, 1.f }, ConverWStringtoC(ConvertToWideString((*m_vecParentGroup)[m_iSelected_GroupObjectIndex]->Get_ObjectNameTag())));

				}
				else
				{
					ImGui::Text("Not Selected");
				}
				ImGui::SameLine();
			}
		}
		break;
	}
	case Client::CWindow_UITool::TEXT:
	{
		if (m_pCurrSelectUI != nullptr)
		{
			string strName = dynamic_cast<CUI_Text*>(m_pCurrSelectUI)->Get_ObjectNameTag();

			ImGui::Text(ConverWStringtoC(ConvertToWideString(strName)));

			Text_List();
		}
		else
		{
		ImGui::Text("Not Selected");
		}
		break;
	}
	case Client::CWindow_UITool::TYPE_END:
	{
		if (m_pCurrSelectUI == nullptr)
			ImGui::Text("Not Selected");
		break;
	}
	default:
		break;
	}

	if (pTransformCom != nullptr)
	{
		//ImGui::InputFloat("PositionX", &m_pCurrSelectUI->m_iTextureNum); // z값 변경 추가

		_vector vPosition = pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vRotation = pTransformCom->Get_Rotated();
		_vector vScale = pTransformCom->Get_Scaled();

		_vector vOrthoPos = m_pGameInstance->Convert_Orthogonal(vPosition);

		ImGui::InputFloat("PositionX", &vPosition.m128_f32[0]);
		ImGui::InputFloat("PositionY", &vPosition.m128_f32[1]);
		ImGui::InputFloat("PositionZ", &vPosition.m128_f32[2]);

		ImGui::InputFloat("RotationX", &vRotation.m128_f32[0]);
		ImGui::InputFloat("RotationY", &vRotation.m128_f32[1]);
		ImGui::InputFloat("RotationZ", &vRotation.m128_f32[2]);

		ImGui::InputFloat("ScaleX", &vScale.m128_f32[0]);
		ImGui::InputFloat("ScaleY", &vScale.m128_f32[1]);
		ImGui::InputFloat("ScaleZ", &vScale.m128_f32[2]);
	}

	/* 미리보기 */
	if(!m_vecTexture.empty())
		ImGui::Image((void*)m_vecTexture[m_iSelectedPathIndex]->SRV_Texture, ImVec2((_float)m_vecTexture[m_iSelectedPathIndex]->iImage_Width, (_float)m_vecTexture[m_iSelectedPathIndex]->iImage_Height));
}

void CWindow_UITool::Create_TargetTexture()
{
	/* error : Find함수로 랜더타겟을 찾아온 뒤, 그녀석으로 Create함수를 호출하면 외부참조기호 에러가 발생함.. 직접 게임인스턴스로 Create까지 직결되는 함수를 새로 만들어서 사용하며 해결 */
	m_pGameInstance->Create_RenderTarget(TEXT("Target_UI_Diffuse"));
}

void CWindow_UITool::KeyframeList()
{
	// 키프레임 목록 창
	ImGui::Begin(u8"키프레임 정보");

	for (int i = 0; i < m_vecTimeline->size(); ++i) {
		CUI::UIKEYFRAME& keyframe = (*m_vecTimeline)[i];
		char keyframeName[64];
		snprintf(keyframeName, sizeof(keyframeName), u8"키프레임 %d", i);

		// 키프레임 정보를 목록으로 표시
		if (ImGui::TreeNode(keyframeName)) {
			ImGui::Text(u8"값: %.2f", keyframe.fValue);
			ImGui::Text(u8"시간: %.2f", keyframe.fTime);
			ImGui::Text(u8"크기: (%.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y);
			ImGui::Text(u8"회전:(%.2f)", keyframe.fRot);
			ImGui::Text(u8"이동: (%.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y);
			ImGui::Checkbox(u8"이징 In", &keyframe.isEaseIn);
			ImGui::Checkbox(u8"이징 Out", &keyframe.isEaseOut);

			// 키프레임 삭제 버튼
			if (ImGui::Button(u8"삭제")) {
				m_vecTimeline->erase(m_vecTimeline->begin() + i);
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void CWindow_UITool::KeyframeChangeWindow()
{

	
}

void CWindow_UITool::KeyframeRender_ValueChange()
{
	if (m_vecTimeline == nullptr)
		return;

	if ((*m_vecTimeline).empty())
		return;

	ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 그리기

	// 키프레임 렌더링 및 편집
	for (int i = 0; i < (*m_vecTimeline).size(); ++i)
	{
		//CUI::UIKEYFRAME& keyframe = timeline[m_eKeyType][i];

		// 타임 라인 상의 위치를 계산
		float xPos = timelinePos.x + ((*m_vecTimeline)[i].fTime / 20.0f) * timelineSize.x;
		float yPos = timelinePos.y + timelineSize.y - timelineSize.y * (*m_vecTimeline)[i].fValue;

		// 키프레임 위치 값이 0(초기값)일 경우
		if ((*m_vecTimeline)[i].vKeyFramePos.x <= 1 &&
			(*m_vecTimeline)[i].vKeyFramePos.y <= 1)
		{
			// 현재 선택된 키프레임 위치를 넣어준다.
			(*m_vecTimeline)[i].vKeyFramePos.x = xPos;
			(*m_vecTimeline)[i].vKeyFramePos.y = yPos;
		}

		// 현재 시간에 해당하는 키프레임 표시 (빨간색 원)
		if ((*m_vecTimeline)[i].fTime >= currentTime - 0.1 &&
			(*m_vecTimeline)[i].fTime <= currentTime + 0.1) 
		{
			draw_list->AddCircleFilled(
				ImVec2(xPos, yPos),
				5.0f,
				IM_COL32(255, 0, 0, 255) // 빨간색으로 표시
			);
		}
		else 
		{
			// 다른 시간의 키프레임 표시 (노란색 원)
			draw_list->AddCircleFilled(
				ImVec2(xPos, yPos),
				5.0f,
				IM_COL32(255, 255, 0, 255) // 노란색으로 표시
			);
		}

		// 마우스가 키프레임 위에 있는 경우에만 툴팁 표시 (마우스 오버)
		if (ImGui::IsMouseHoveringRect(
			ImVec2(xPos - 5.0f, yPos - 5.0f),
			ImVec2(xPos + 5.0f, yPos + 5.0f)))
		{

			// 키프레임 정보 표시
			ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
			ImGui::BeginTooltip();
			ImGui::Text(u8"키프레임 %d", i);
			ImGui::Separator();
			ImGui::Text(u8"시간 : %.2f", (*m_vecTimeline)[i].fTime);
			ImGui::Text(u8"값 : %.2f", (*m_vecTimeline)[i].fValue);
			ImGui::Text(u8"크기 X, Y : (%.2f, %.2f, %.2f)", (*m_vecTimeline)[i].vScale.x, (*m_vecTimeline)[i].vScale.y);
			ImGui::Text(u8"회전 Z :(%.2f)", (*m_vecTimeline)[i].fRot);
			ImGui::Text(u8"이동 X, Y : (%.2f, %.2f, %.2f)", (*m_vecTimeline)[i].vPos.x, (*m_vecTimeline)[i].vPos.y);

			if ((*m_vecTimeline)[i].isEaseIn)
				ImGui::Text(u8"이징 In: 켜짐");
			else
				ImGui::Text(u8"이징 In: 꺼짐");
			if ((*m_vecTimeline)[i].isEaseOut)
				ImGui::Text(u8"이징 Out: 켜짐");
			else
				ImGui::Text(u8"이징 Out: 꺼짐");
			ImGui::EndTooltip();
		}

		// 마우스 릴리즈 시 드래그 종료
		if (ImGui::IsMouseReleased(0))
		{
			isDraggingTimeline = false;
		}

		if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
		{
			// 오른쪽 마우스 클릭으로 키프레임 삭제
			if (ImGui::IsMouseHoveringRect(
				ImVec2(xPos - 5.0f, yPos - 5.0f),
				ImVec2(xPos + 5.0f, yPos + 5.0f))
				&& ImGui::IsMouseClicked(1))
			{
				(*m_vecTimeline).erase((*m_vecTimeline).begin() + i);
				break; // 삭제한 후에 루프를 빠져나갑니다.
			}
		}

	}
}

// 최소, 최대 값 및 크기, 회전, 이동 값을 기반으로 선형 보간된 키프레임을 자동으로 생성하는 함수
void CWindow_UITool::CreateKeyframesWithLinearInterpolation(
	_float minTime, _float maxTime,
	_float minValue, _float maxValue,
	_float2 minScaleValue, _float2 maxScaleValue,
	_float minRotationValue, _float maxRotationValue,
	_float2 minTranslationValue, _float2 maxTranslationValue,
	_float _minTexture, _float _maxTexture,
	_int numKeyframes)
{
	if (numKeyframes < 2) 
	{
		// 최소 2개 이상의 키프레임이 필요합니다.
		return;
	}

	if (m_pCurrSelectUI == nullptr)
		return;

	// 키프레임 간의 시간 간격을 계산
	float timeStep = 1.0f / static_cast<float>(numKeyframes - 1);

	// 첫 번째와 마지막 키프레임을 수동으로 추가
	CUI::UIKEYFRAME firstKeyframe;

	// 최소 값 보간
	firstKeyframe.fTime = minTime;
	firstKeyframe.fValue = minValue;
	firstKeyframe.vScale = minScaleValue; // Check CheckCheckCheckCheckCheckCheck
	firstKeyframe.fRot = minRotationValue;
	firstKeyframe.vPos = minTranslationValue;
	firstKeyframe.iTexureframe = (_int)_minTexture;

	if (m_pCurrSelectUI)
		m_pCurrSelectUI->Add_Keyframe(firstKeyframe);
	//(*m_vecTimeline).push_back(firstKeyframe);

	CUI::UIKEYFRAME lastKeyframe;

	// 최대 값 보간
	lastKeyframe.fTime = maxTime;
	lastKeyframe.fValue = maxValue;
	lastKeyframe.vScale = maxScaleValue;
	lastKeyframe.fRot = maxRotationValue;
	lastKeyframe.vPos = maxTranslationValue;
	lastKeyframe.iTexureframe = (_int)_maxTexture;

	if (m_pCurrSelectUI)
		m_pCurrSelectUI->Add_Keyframe(lastKeyframe);
	//(*m_vecTimeline).push_back(lastKeyframe);

	int num_decimal_places = 2; // 소수점 단위 설

	// 소수점 단위 표현
	int multiplier = static_cast<int>(pow(10, num_decimal_places));


	// 중간 키프레임 생성 및 보간
	for (int i = 1; i < numKeyframes - 1; ++i)
	{
		float time = i * timeStep;
		float t = time;  // 시간을 보간 계수로 사용

		int itextureRange =int( _maxTexture - _minTexture + 1); // 텍스처 범위 계산
		int iTextureNum =int(_minTexture + (i % itextureRange)); // 현재 인덱스에 따라 텍스처 값 설정

		// 매 키프레임마다 순차적으로 증가 및 감소한 값을 생성하여 추가
		float step = (maxValue - minValue) / (numKeyframes - 1); // 증가/감소 단계


		// 각각의 속성에 대해 보간값을 계산
		_float2 scale = Lerp_float2(minScaleValue, maxScaleValue, t);

		_float rotation = Lerp_float(minRotationValue, maxRotationValue, t);

		_float2 translation = Lerp_float2(minTranslationValue, maxTranslationValue, t);

		_float ftime = Lerp_float(minTime, maxTime, t);

		_float value = Lerp_float(minValue, maxValue, t);
		 
		_int iTexture = (_int)Lerp_float(_minTexture, _maxTexture, t);

		CUI::UIKEYFRAME keyframe;

		// 값 보간
		keyframe.fTime = ftime;

		if (m_bRendomValue)
		{
			// 순차적으로 증가 및 감소한 소수점 값 생성
			if (i < numKeyframes / 2)
			{
				keyframe.fValue = minValue + i * step; // 최소값에서 증가
			}
			else
			{
				keyframe.fValue = maxValue - (i - numKeyframes / 2) * step; // 최대값에서 감소
			}
		}
		else
		{
			keyframe.fValue = value;
		}


		keyframe.vScale = scale;
		keyframe.fRot = rotation;
		keyframe.vPos = translation;

		if (m_bIndividualTexture)
		{
			keyframe.iTexureframe = iTextureNum;
		}
		else
		{
			keyframe.iTexureframe = iTexture;
		}

		// 기존 timeline 벡터에 키프레임을 이어서 추가
		if (m_pCurrSelectUI)
			m_pCurrSelectUI->Emplaceback_Keyframe(keyframe);
		//(*m_vecTimeline).emplace_back(keyframe);
	}

	// 시간에 따라 키프레임 정렬
	std::sort((*m_vecTimeline).begin(), (*m_vecTimeline).end(), [](const CUI::UIKEYFRAME& a, const CUI::UIKEYFRAME& b)
		{
			return a.fTime < b.fTime;
		});
}

void CWindow_UITool::KeyframeAutomaticGeneration()
{
	//ImGui::Begin(u8"키프레임 자동 생성");

	ImGui::Text(u8"키프레임 자동 생성 세팅");

	// 입력값을 받는 UI 요소 생성
	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"시간 설정");
	ImGui::InputFloat2(u8"시작/끝 시간 입력", _v2Time);
	ImGui::Separator();

	//ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"벨류 설정");
	ImGui::InputFloat2(u8"시작/끝 벨류 입력", _v2Value);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"크기 설정");
	ImGui::InputFloat2(u8"시작 프레임 크기 입력", minScale);

	//ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::InputFloat2(u8"끝 프레임 크기 입력", maxScale);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"회전 설정");
	ImGui::InputFloat(u8"시작 프레임 회전 입력", &minRot);

	//ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::InputFloat(u8"끝 프레임 회전 입력", &maxRot);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"이동 설정");
	ImGui::InputFloat2(u8"시작 프레임 이동 입력", minPos);

	//ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::InputFloat2(u8"끝 프레임 이동 입력", maxPos);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"텍스처 설정");
	ImGui::InputFloat(u8"시작 프레임 텍스처 입력", &fMin_Texture);

	//ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::InputFloat(u8"끝 프레임 텍스처 입력", &fMax_Texture);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"원하는 키프레임 수 설정");
	ImGui::InputFloat(u8"키프레임 수 입력", &numKeyframes);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"개별 텍스처 설정");
	ImGui::Checkbox(u8"개별 텍스처", &m_bIndividualTexture);
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"랜덤 벨류 설정");
	ImGui::Checkbox(u8"랜덤 벨류", &m_bRendomValue);
	ImGui::Separator();
	
	if (m_pCurrSelectUI == nullptr)
		return;

	if (ImGui::Button(u8"자동 생성")) 
	{
		// 버튼이 클릭되면 timeline 벡터를 비우고 CreateKeyframesWithLinearInterpolation 함수 호출
		//timeline[m_eKeyType].clear();

		/* float2로 변환해서 넘겨주기 */
		_float2 fminScale = { minScale[0], minScale[1] };
		_float2 fmaxScale = { maxScale[0], maxScale[1] };
		_float2 fminPos = { minPos[0], minPos[1] };
		_float2 fmaxPos = { maxPos[0], maxPos[1] };

		CreateKeyframesWithLinearInterpolation(
			_v2Time[0], _v2Time[1],
			_v2Value[0], _v2Value[1],
			fminScale, fmaxScale,
			minRot, maxRot,
			fminPos, fmaxPos,
			fMin_Texture, fMax_Texture,
			(_int)numKeyframes);
	}

	if (ImGui::Button("clear"))
	{
		// 버튼이 클릭되면 timeline 벡터를 비우고 CreateKeyframesWithLinearInterpolation 함수 호출
		(*m_vecTimeline).clear();
	}
}

void CWindow_UITool::PlayAnimation(_float fTimeDelta)
{
	/* Set UI AnimTime */
	//if (m_pCurrSelectUI)
	//	m_pCurrSelectUI->Set_CurrTime(currentTime);

	// 애니메이션 재생/일시 정지 버튼
	if (m_isPlayAnim)
	{
		if (ImGui::Button(u8"정지"))
		{
			m_isPlayAnim = false;
			if (m_pCurrSelectUI)
				m_pCurrSelectUI->Set_AnimPlay(false);
		}
	}
	else 
	{
		if (ImGui::Button(u8"재생"))
		{
			m_isPlayAnim = true;
			if (m_pCurrSelectUI)
				m_pCurrSelectUI->Set_AnimPlay(true);
		}
	}
	ImGui::SameLine();

	// 애니메이션 재생/일시 정지 버튼
	if (m_bRepetition)
	{
		if (ImGui::Button(u8"반복"))
		{
			m_bRepetition = !m_bRepetition;
			if (m_pCurrSelectUI)
				m_pCurrSelectUI->Set_Repetition(m_bRepetition);
		}
	}
	else
	{
		if (ImGui::Button(u8"한번"))
		{
			m_bRepetition = !m_bRepetition;
			if (m_pCurrSelectUI)
				m_pCurrSelectUI->Set_Repetition(m_bRepetition);
		}
	}


	ImGui::SameLine();
	if (ImGui::Button(u8"초기화"))
	{
		m_fPlayTime = 0.0f;
		currentTime = 0.0f;
		if (m_pCurrSelectUI)
			m_pCurrSelectUI->Set_CurrTime(0.0f);
	}

	// 애니메이션 시간 설정
	if (m_isPlayAnim)
	{
		m_fPlayTime += m_fPlaybackSpeed * ImGui::GetIO().DeltaTime;
		//currentTime += m_fPlaybackSpeed * ImGui::GetIO().DeltaTime;
		if (m_pCurrSelectUI)
			currentTime = m_pCurrSelectUI->Get_CurrTime();
		//else
		//	currentTime += m_fPlaybackSpeed * ImGui::GetIO().DeltaTime;

		if (m_vecTimeline == nullptr)
			return;

		if (m_bRepetition)
		{
			if (!m_vecTimeline->empty())
			{
				if (currentTime > m_vecTimeline->back().fTime)  // 현재 시간이 최대 시간 값 보다 클 경우
				{
					m_fPlayTime = 0.0f;		// 현재 시간을 최대 시간 값으로 고정
					currentTime = 0.0f;		// 현재 시간을 최대 시간 값으로 고정
					m_isPlayAnim = true;	// 플레이 계속
					if (m_pCurrSelectUI)
					{
						m_pCurrSelectUI->Set_AnimPlay(true);
						m_pCurrSelectUI->Set_Repetition(true);
					}
				}
			}
			else
			{
				if (currentTime > MaxTime)  // 현재 시간이 최대 시간 값 보다 클 경우
				{
					m_fPlayTime = 0.0f;		// 현재 시간을 최대 시간 값으로 고정
					currentTime = 0.0f;		// 현재 시간을 최대 시간 값으로 고정
					m_isPlayAnim = true;	// 플레이 중지
					if (m_pCurrSelectUI)
					{
						m_pCurrSelectUI->Set_AnimPlay(true);
						m_pCurrSelectUI->Set_Repetition(true);
					}
				}
			}
		}
		else
		{
			if (!m_vecTimeline->empty())
			{
				if (currentTime > m_vecTimeline->back().fTime) // 현재 시간이 키프레임 최대 시간 값 보다 클 경우
				{
					m_fPlayTime = m_vecTimeline->back().fTime; // 현재 시간을 키프레임 최대 시간 값으로 고정
					currentTime = m_vecTimeline->back().fTime; // 현재 시간을 키프레임 최대 시간 값으로 고정
					if (m_pCurrSelectUI)
						m_pCurrSelectUI->Set_AnimPlay(false);
					m_isPlayAnim = false;  // 플레이 중지
				}
			}
			else
			{
				if (currentTime > MaxTime) // 현재 시간이 최대 시간 값 보다 클 경우
				{
					m_fPlayTime = MaxTime; // 현재 시간을 최대 시간 값으로 고정
					currentTime = MaxTime; // 현재 시간을 최대 시간 값으로 고정
					if (m_pCurrSelectUI)
						m_pCurrSelectUI->Set_AnimPlay(false);
					m_isPlayAnim = false;  // 플레이 중지
				}
			}
		}
	}
}

// 애니메이션 타임라인 바
void CWindow_UITool::AnimationTimeLineBar(_float fTimeDelta)
{
#pragma region 애니메이션 타임 라인
	// 타임 라인 렌더링
	// ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 그리기
	timelinePos = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 그리기
	// 애니메이션 타임 라인 위치 설정
	timelinePos.x += 0.f;
	timelinePos.y += 0.f;

	ImGui::SameLine();
	// 시간
	ImGui::Text(u8"시간 : %.2f", currentTime);
	ImGui::SameLine();

	ImGui::PushItemWidth(150);
	// 시간 그래프
	for (int n = 0; n < 100; n++)
		m_fFrame[n] = sinf(n * 0.2f + (_float)ImGui::GetTime() * m_fPlaybackSpeed);
	ImGui::PlotLines(u8"시간 그래프", m_fFrame, 100);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (m_pCurrSelectUI)
	{
		char* Test = "Select Keyframe %d", closestKeyframeIndex;
		strcmp(Test, m_pCurrSelectUI->Get_UIDesc().strObjectName.c_str());

		ImGui::TextColored({ 255.f, 255.f, 255.f, 255.f }, Test);

		//ImGui::TextColored({ 1.f, 1.f, 1.f, 1.f, }, u8"현재 선택 : 키프레임%d", closestKeyframeIndex);
	}
	// 애니메이션 타임 라인 그리기
	draw_list->AddRectFilled(
		timelinePos,
		ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y),
		IM_COL32(50, 50, 50, 255) // 타임라인 배경 색상
	);

	// 현재 시간을 최소와 최대 시간 비율로 변환
	float minTime = 0.0f; // 최소 시간
	//float maxTime = 100.0f; // 최대 시간

	// 애니메이션 타임 라인 눈금 그리기 (1단위 표시 기준 : 최대 시간 값 만큼)
	for (int i = 0; i <= MaxTime; ++i)
	{
		// 눈금 시간의 간격                                최대 시간값으로 나눠 간격 설정
		float xPos = timelinePos.x + i * (timelineSize.x / MaxTime);

		float relativeStartTimeXPos = timelinePos.x;    //애니메이션 타임 라인 그래프의 시작
		float relativeEndTimeXPos = timelinePos.x + timelineSize.x; // 애니메이션 타임 라인 그래프의 끝

		// 그래프 데이터 배열을 현재 시간에 맞게 업데이트
		for (int n = 0; n < 100; n++) 
		{
			float time = n / 100.0f * MaxTime;
			m_fFrame[n] = EvaluateAnimationAtTime(time);
		}

		// 현재 시간을 최대 시간 범위 내로 클램핑
		currentTime = ImClamp(currentTime, 0.0f, MaxTime);

		// 현재 시간을 최소와 최대 시간 비율로 변환
		float relativeCurrentTime = (currentTime - minTime) / (MaxTime - minTime);

		// 현재 시간 값으로 애니메이션 타임 라인에 맞는 위치값을 계산
		float relativeCurrentTimeXPos = ImLerp(relativeStartTimeXPos, relativeEndTimeXPos, relativeCurrentTime);

		// 빨간 선(현재 시간 위치) 그리기
		draw_list->AddLine(
			ImVec2(relativeCurrentTimeXPos, timelinePos.y), // 라인 시작점
			ImVec2(relativeCurrentTimeXPos, timelinePos.y + timelineSize.y), // 라인 끝점
			IM_COL32(255, 0, 0, 255) // 빨간 선으로 현재 시간 표시
		);

		// 눈금 아래에 숫자로 시간 표시
		float timeLabel = i * fDisplayTime * (MaxTime / MaxTime); // 0.1 간격으로 0부터 MaxTime까지 표시
		char timeLabelStr[16];
		snprintf(timeLabelStr, sizeof(timeLabelStr), "%.1f", timeLabel);
		ImVec2 textPos(xPos - 10, timelinePos.y + timelineSize.y + 5.f);
		draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), timeLabelStr);
	}

	// graphPos : 애니메이션 타임 라인 그래프를 그리기 위해 계산된 위치를 나타냄
	//  graphPos = ImVec2(왼쪽 위 모서리 화면상의 위치x, y, 타임라인의 크기)
	ImVec2 graphPos = ImVec2(timelinePos.x, timelinePos.y + timelineSize.y + 10);

	/* 키프레임 랜더 */
	KeyframeRender_ValueChange();
	/* 키프레임 조절 */
	KeyframeValueChange(fTimeDelta);
#pragma endregion
}

_float CWindow_UITool::EvaluateAnimationAtTime(float time)
{
	if (m_vecTimeline == nullptr)
		return 0.0f;

	if (m_vecTimeline->empty())
	{
		// 만약 키프레임이 없다면 기본값 0.0을 반환하거나,
		// 다른 처리를 수행.
		return 0.0f;
	}

	// 키프레임을 시간에 따라 정렬.
	std::sort(m_vecTimeline->begin(), m_vecTimeline->end(), [](const CUI::UIKEYFRAME& tFirst_UI, const CUI::UIKEYFRAME& tSecond_UI)
		{
			return tFirst_UI.fTime < tSecond_UI.fTime;
		});

	// 현재 시간과 가장 가까운 이전과 이후의 키프레임을 찾기
	CUI::UIKEYFRAME* prevKeyframe = nullptr;
	CUI::UIKEYFRAME* nextKeyframe = nullptr;
	
	for (int i = 0; i < (*m_vecTimeline).size(); ++i)
	{
		if ((*m_vecTimeline)[i].fTime <= time)
		{
			prevKeyframe = &(*m_vecTimeline)[i];
		}
		if ((*m_vecTimeline)[i].fTime >= time)
		{
			nextKeyframe = &(*m_vecTimeline)[i];
			break;
		}
	}

	// 이전과 다음 키프레임을 찾지 못하면 첫 번째 키프레임을 사용.
	if (!prevKeyframe) {
		prevKeyframe = &(*m_vecTimeline)[0];
	} 

	// 다음 키프레임을 찾지 못하면 마지막 키프레임을 사용.
	if (!nextKeyframe) {
		nextKeyframe = &(*m_vecTimeline).back();
	}

	// 이전과 다음 키프레임 사이에서 시간을 보간하여 값을 계산.
	float t = (time - prevKeyframe->fTime) / (nextKeyframe->fTime - prevKeyframe->fTime);
	float value = Lerp_float(prevKeyframe->fValue, nextKeyframe->fValue, t);

	// 이징(Easing)을 적용합니다.
	if (t < 0.5f) {
		if (prevKeyframe->isEaseIn) 
		{
			float easedT = t / 0.5f;
			value = ImEaseInQuad(prevKeyframe->fValue, nextKeyframe->fValue, easedT);
		}
	}
	else {
		if (nextKeyframe->isEaseOut) 
		{
			float easedT = (t - 0.5f) / 0.5f;
			value = ImEaseOutQuad(prevKeyframe->fValue, nextKeyframe->fValue, easedT);
		}
	}

	return value;
}

void CWindow_UITool::KeyframeValueChange(_float fTimeDelta)
{
#pragma region 타임라인 키프레임 조절

	if (m_vecTimeline == nullptr)
		return;
	if (m_vecTimeline->empty())
		return;
	// 타임라인에서 키프레임을 드래그하여 순서 변경
	for (int i = 0; i < (*m_vecTimeline).size(); ++i)
	{
		CUI::UIKEYFRAME& keyframe = (*m_vecTimeline)[i];
		//float xPos = timelinePos.x + timelineSize.x * ImClamp(keyframe.time, 0.0f, MaxTime);

		// 타임 라인 상의 위치를 계산
		float xPos = timelinePos.x + (keyframe.fTime / 20.0f) * timelineSize.x;
		float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.fValue; // 벨류에 맞는 높이 위치값을 조절해야한다.

		if (ImGui::IsMouseHoveringRect(
			ImVec2(xPos - 5.0f, yPos - 5.0f), // 좌표 설정
			ImVec2(xPos + 5.0f, yPos + 5.0f)) // 좌표 설정
			&& ImGui::IsMouseClicked(0))
		{
			isDraggingKeyframe = true;
			draggingKeyframeIndex = i;
			initialKeyframeX = xPos; // 드래그를 시작하는 위치 저장
		}
	}

	if (isDraggingKeyframe)
	{
		// 키프레임의 시간을 드래그한 위치로 설정
		CUI::UIKEYFRAME& keyframe = (*m_vecTimeline)[draggingKeyframeIndex];
		float xPos = ImGui::GetMousePos().x - timelinePos.x;

		// 마우스 드래그로 키프레임의 시간을 조정
		float newXPos = ImGui::GetMousePos().x;
		float deltaTime = (newXPos - initialKeyframeX) / timelineSize.x * MaxTime;

		// 클릭한 위치에 해당하는 value 값
		keyframe.fValue = 1.0f - (ImGui::GetMousePos().y - timelinePos.y) / timelineSize.y;

		// 시간 값
		(*m_vecTimeline)[draggingKeyframeIndex].fTime += deltaTime;

		// 시간과 값을 적절하게 클램핑할 수도 있음.
		// 드래그 했을 때 시간과 벨류 값 조정 (키프레임 time과 value, 최소 값, 최대 값);
		keyframe.fTime = ImClamp(keyframe.fTime, 0.0f, MaxTime); // 최대 값을 MaxTime으로 지정
		keyframe.fValue = ImClamp(keyframe.fValue, 0.0f, MaxTime); // 최대 값을 MaxTime으로 지정

		// 현재 위치를 갱신
		initialKeyframeX = newXPos;

		// 키프레임 순서 변경
		if (draggingKeyframeIndex > 0 && keyframe.fTime < (*m_vecTimeline)[draggingKeyframeIndex - 1].fTime) {
			std::swap((*m_vecTimeline)[draggingKeyframeIndex], (*m_vecTimeline)[draggingKeyframeIndex - 1]);
			draggingKeyframeIndex--;
		}
		else if (draggingKeyframeIndex < (m_vecTimeline)->size() - 1 && keyframe.fTime > (*m_vecTimeline)[draggingKeyframeIndex + 1].fTime) {
			std::swap((*m_vecTimeline)[draggingKeyframeIndex], (*m_vecTimeline)[draggingKeyframeIndex + 1]);
			draggingKeyframeIndex++;
		}
	}
#pragma endregion

	// 마우스 릴리즈 시 드래그 종료
	if (ImGui::IsMouseReleased(0))
	{
		isDraggingTimeline = false;
		isDraggingKeyframe = false;
	}

	SelectKeyframeMouse();

	ImGui::SameLine();

	KeyframeList(); // 키프레임 목록
}

void CWindow_UITool::ImGuiKeyInput()
{
	if (m_bShortOff)
		return;

	if (m_vecTimeline == nullptr)
		return;

	//if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	//{

	//}

	// "R" 키 : 현재 시간(시간선)을 0으로 초기화 [처음으로 이동]
	if (m_pGameInstance->Key_Down(DIK_R))
	{
		if (m_pCurrSelectUI)
			m_pCurrSelectUI->Set_CurrTime(0.0f);
		currentTime = 0.f;
	}

	// "T" 키 : 현재 시간(시간 선)을 마우스 위치로 변경
	if (m_pGameInstance->Key_Pressing(DIK_T))
	{
		// 현재 마우스 위치
		ImVec2 mousePos = ImGui::GetMousePos();

		// 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
		float relativeMouseX = (mousePos.x - timelinePos.x) / timelineSize.x;

		// 시간 값을 계산하여 새로운 키프레임을 생성
		float newTime = relativeMouseX * MaxTime; // MaxTime은 최대 시간 값

		// 시간을 현재 마우스 위치에 맞게 설정합니다.
		currentTime = newTime;
	}

	// "Space" 키 : 애니메이션 재생/정지
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_isPlayAnim = !m_isPlayAnim;
		if(m_pCurrSelectUI)
			m_pCurrSelectUI->Set_AnimPlay(m_isPlayAnim);
	}

	// "0" 키 : 재생 속도 초기화
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		m_fPlaybackSpeed = 1.f;
	}

	// "-" 키 : 재생 속도 -
	if (m_pGameInstance->Key_Down(DIK_MINUS))
	{
		m_fPlaybackSpeed -= 1.f;
	}

	// "+" 키 : 재생 속도 +
	if (m_pGameInstance->Key_Down(DIK_EQUALS))
	{
		m_fPlaybackSpeed += 1.f;
	}

	// " ` ~ " 키 : 변경 모드 초기화(없음)
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GraveAccent)))
	{
		eEditMode = EDITMODE_NONE;
	}
	
	// "F1" 키 : 크기 변경 모드
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F1)))
	{
		eEditMode = EDITMODE_SCALE;
	}
	
	// "F2" 키 : 회전 변경 모드
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F2)))
	{
		eEditMode = EDITMODE_ROTATION;
	}
	
	// "F3" 키 : 이동 변경 모드
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F3)))
	{
		eEditMode = EDITMODE_TRANSLATION;
	}
	
	// "F4" 키 : 시간 및 벨류 변경 모드
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F4)))
	{
		eEditMode = EDITMODE_TIME_VALUE;
	}
	
	// "F5" 키 : 키프레임 초기화
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F5)))
	{
		(*m_vecTimeline).clear();
	}
	
	// "F6" 키 : 속성 값 변경 사이즈 조절 초기화
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F6)))
	{
		vValueSize = 1.f;
	}
	
	// "F7" 키 : 속성 값 변경 사이즈 조절 Minus
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F7)))
	{
		vValueSize -= 1.f;
	}
	
	// "F8" 키 : 속성 값 변경 사이즈 조절 Plus
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F8)))
	{
		vValueSize += 1.f;
	}
	
	// "P" 키 : 변경 모드 초기화(없음)
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P)))
	{
		eEditMode = EDITMODE_TEXTURE;
	}
	
	// "Z" 키 : 이전 키프레임 선택
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
	{
		// 현재 키프레임이 0보다 작지 않을 경우만
		if (closestKeyframeIndex > 0)
		{
			closestKeyframeIndex -= 1;
		}
	
		if (!m_vecTimeline->empty()) // 애니메이션이 있고
		{
			if (closestKeyframeIndex <= -1.f) // 아직 아무것도 선택하지 않았을 경우
			{
				closestKeyframeIndex = 0; // 0번째 선택
			}
		}
	}
	
	// "X" 키 : 다음 키프레임 선택
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
	{
		// 현재 키프레임이 최대 키프레임을 넘지 않았을 때만
		if (closestKeyframeIndex < m_vecTimeline->size() - 1)
		{
			closestKeyframeIndex += 1;
		}
	
		if (!m_vecTimeline->empty()) // 애니메이션이 있고
		{
			if (closestKeyframeIndex <= -1.f) // 아직 아무것도 선택하지 않았을 경우
			{
				closestKeyframeIndex = 0; // 0번째 선택
			}
		}
	}
}

void CWindow_UITool::CurKeyframe_ValueChange()
{
	if (m_vecTimeline == nullptr)
		return;

	// 키프렘 목록을 보여주는 ImGui 윈도우를 만든다.
	if (ImGui::Begin(u8"키프레임 수정"))
	{
		// 키프렘 목록을 루프를 돌며 표시.
		for (int i = 0; i < m_vecTimeline->size(); ++i)
		{
			CUI::UIKEYFRAME& keyframe = (*m_vecTimeline)[i];

			// 각 키프렘을 버튼 또는 다른 ImGui 위젯으로 표시.
			// 여기에서는 버튼을 사용.
			if (ImGui::Button((u8"키프레임" + std::to_string(i)).c_str()))
			{
				// 키프렘이 클릭되면 해당 인덱스를 선택된 키프렘 인덱스로 설정.
				closestKeyframeIndex = i;
			}
		}
	}
	ImGui::End();

#pragma region 선택한 키프레임 값 변경 (창)
	// 선택된 키프렘의 인덱스를 사용하여 키프레임을 편집. (선택한 인덱스가 범위 내에 있을 경우)
	if (!m_vecTimeline->empty() && closestKeyframeIndex >= 0 && closestKeyframeIndex < m_vecTimeline->size())
	{
		m_pSelectedKeyframe = &(*m_vecTimeline)[closestKeyframeIndex];
		m_bSelectKeyframe = true;

		// 선택된 키프렘의 애니메이션 속성 편집
		DrawSelectedKeyframeEditor(*m_pSelectedKeyframe);
	}
	else
	{
		m_pSelectedKeyframe = nullptr;
	}
#pragma endregion
}

// 선택한 키프레임 값을 변경하는 함수
void CWindow_UITool::DrawSelectedKeyframeEditor(CUI::UIKEYFRAME& selectedKeyframe)
{
	if (m_vecTimeline == nullptr)
		return;
	if (m_vecTimeline->empty())
		return;

	// 타임라인 툴
	ImGui::Begin(u8"선택한 키프레임 속성 편집");

	//Keyframe& selectedKeyframe = timeline.back();
	ImGui::TextColored({ 1.f, 1.f, 1.f, 1.f, }, u8"현재 선택 : 키프레임%d", closestKeyframeIndex);

	ImGuiStyle& style = ImGui::GetStyle(); // 스타일 불러오기
	float originalItemWidth = style.ItemInnerSpacing.x; // 원래의 ItemInnerSpacing 값을 저장

	// 원하는 크기로 ItemInnerSpacing을 설정
	style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절

	ImGui::SeparatorText(u8"시간 및 벨류 설정");

	// 시간 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
	ImGui::PushItemWidth(40);

	// 시간 입력 필드
	if (ImGui::DragFloat(u8"시간", &selectedKeyframe.fTime, fMin_Time, fMax_Time))
	{
		// 원래 키프레임의 인덱스 찾기
		for (int i = 0; i < m_vecTimeline->size(); ++i) 
		{
			if (&(*m_vecTimeline)[i] == &selectedKeyframe) 
			{
				originalIndex = (_float)i;
				break;
			}
		}

		// 키프레임을 시간에 따라 정렬
		std::sort(m_vecTimeline->begin(), m_vecTimeline->end(), [](const CUI::UIKEYFRAME& tFirstKeyframe, const CUI::UIKEYFRAME& tSecondKeyframe) 
			{
				return tFirstKeyframe.fTime < tSecondKeyframe.fTime;
			});

		// 원래 키프레임의 인덱스 찾기
		for (int i = 0; i < m_vecTimeline->size(); ++i) 
		{
			if (&(*m_vecTimeline)[i] == &selectedKeyframe) 
			{
				closestKeyframeIndex = i;
				break;
			}
		}

		// 키프레임 순서 변경
		if (originalIndex != closestKeyframeIndex) 
		{
			// 키프레임이 원래 위치에서 이동한 경우
			// 이동한 위치에 따라 인덱스 업데이트
			draggingKeyframeIndex = closestKeyframeIndex;
		}
	}

	ImGui::PopItemWidth();

	ImGui::SameLine(); // 같은 라인

	// 벨류 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
	ImGui::PushItemWidth(40);
	// 벨류 입력 필드
	if (ImGui::DragFloat(u8"벨류", &selectedKeyframe.fValue, fMin_Value, fMax_Value))
	{

	}
	ImGui::PopItemWidth();

	if (ImGui::DragFloat(u8"알파", &selectedKeyframe.fAlpha))
	{

	}

	//ImGui::SameLine(); // 같은 라인
	ImGui::Dummy(ImVec2(0, 5)); // 공백

	// 텍스처 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
	ImGui::PushItemWidth(80);
	//ImGui::Text("%d", selectedKeyframe.texureframe);
	if (ImGui::InputInt(u8"텍스처 번호", &selectedKeyframe.iTexureframe))
	{

	}
	ImGui::PopItemWidth();

	// 원래의 ItemInnerSpacing 값으로 복원
	style.ItemInnerSpacing.x = originalItemWidth;

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	// 애니메이션 타입 설정
	ImGui::SeparatorText(u8"애니메이션 타입 설정");
	ImGui::RadioButton(u8"크기", &selectedKeyframe.iType, 0); // 크기 애니메이션

	ImGui::SameLine();
	ImGui::RadioButton(u8"회전", &selectedKeyframe.iType, 1); // 회전 애니메이션

	ImGui::SameLine();
	ImGui::RadioButton(u8"이동", &selectedKeyframe.iType, 2); // 이동 애니메이션


	ImGui::Dummy(ImVec2(0, 5)); // 공백

	//style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절
	//// 키프레임 타입에 따른 추가 설정 (슬라이더)
	//if (selectedKeyframe.iType == 0) { // 크기 애니메이션
	//	ImGui::DragFloat(u8"크기(슬라이더) X", &selectedKeyframe.vScale.x, fMin_Scale, fMax_Scale);
	//	ImGui::SameLine();
	//	ImGui::DragFloat(u8"크기(슬라이더) Y", &selectedKeyframe.vScale.y, fMin_Scale, fMax_Scale);
	//}
	//else if (selectedKeyframe.iType == 1) { // 회전 애니메이션
	//	ImGui::DragFloat(u8"회전(슬라이더) Z", &selectedKeyframe.fRot, fMin_Rot, fMax_Rot);
	//}
	//else if (selectedKeyframe.iType == 2) { // 이동 애니메이션
	//	ImGui::DragFloat(u8"이동(슬라이더) X", &selectedKeyframe.vPos.x, fMin_Pos, fMax_Pos);
	//	ImGui::SameLine();
	//	ImGui::DragFloat(u8"이동(슬라이더) Y", &selectedKeyframe.vPos.y, fMin_Pos, fMax_Pos);
	//}

	style.ItemInnerSpacing.x = originalItemWidth;// 원래의 ItemInnerSpacing 값으로 복원

	//ImGui::Dummy(ImVec2(0, 10));
	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 5)); // 공백

	style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절
	// 키프레임 타입에 따른 추가 설정 (입력)
	if (selectedKeyframe.iType == 0) // 크기 애니메이션
	{
		if (*m_pCurrSelectUI->Get_WorldUI() == true)
		{
			ImGui::DragFloat(u8"월드 크기 X", &selectedKeyframe.vWorld_Scale.x, 0.0001f, -3.f, 3.f);
			ImGui::DragFloat(u8"월드 크기 Y", &selectedKeyframe.vWorld_Scale.y, 0.0001f, -3.f, 3.f);
			ImGui::DragFloat(u8"월드 크기 Z", &selectedKeyframe.vWorld_Scale.z, 0.0001f, -3.f, 3.f);
		}
		else
		{
			// 크기 값 입력
			ImGui::DragFloat(u8"크기 X", &selectedKeyframe.vScale.x);
			ImGui::DragFloat(u8"크기 Y", &selectedKeyframe.vScale.y);
		}

		// 입력된 값의 범위를 제한(수동)
		selectedKeyframe.vScale.x = (selectedKeyframe.vScale.x < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.x > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.x);
		selectedKeyframe.vScale.y = (selectedKeyframe.vScale.y < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.y > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.y);
	}
	else if (selectedKeyframe.iType == 1) // 회전 애니메이션
	{
		// 회전 값 입력
		if (*m_pCurrSelectUI->Get_WorldUI() == true)
		{
			ImGui::DragFloat(u8"월드 회전 X", &selectedKeyframe.vWorld_Rot.x, 0.001f, -4.f, 4.f);
			ImGui::DragFloat(u8"월드 회전 Y", &selectedKeyframe.vWorld_Rot.y, 0.001f, -4.f, 4.f);
			ImGui::DragFloat(u8"월드 회전 Z", &selectedKeyframe.vWorld_Rot.z, 0.001f, -4.f, 4.f);
		}
		else
		{
			ImGui::DragFloat(u8"회전 Z", &selectedKeyframe.fRot, 0.1f);
			// 입력된 값의 범위를 제한(함수)
			selectedKeyframe.fRot = ImClamp(selectedKeyframe.fRot, fMin_Rot, fMax_Rot); // Z 값의 범위 제한
		}
	

	}
	else if (selectedKeyframe.iType == 2) // 이동 애니메이션
	{
		// 회전 값 입력
		if (*m_pCurrSelectUI->Get_WorldUI() == true)
		{
			ImGui::DragFloat(u8"월드 이동 X", &selectedKeyframe.vWorld_Pos.x, 0.0001f, -3.f, 3.f);
			ImGui::DragFloat(u8"월드 이동 Y", &selectedKeyframe.vWorld_Pos.y, 0.0001f, -3.f, 3.f);
			ImGui::DragFloat(u8"월드 이동 Z", &selectedKeyframe.vWorld_Pos.z, 0.0001f, -3.f, 3.f);
		}
		else
		{
			// 이동 값 입력
			ImGui::DragFloat(u8"이동 X", &selectedKeyframe.vPos.x);
			ImGui::DragFloat(u8"이동 Y", &selectedKeyframe.vPos.y);
		}

		// 입력된 값의 범위를 제한(함수)
		//selectedKeyframe.vPos.x = ImClamp(selectedKeyframe.vPos.x, fMin_Pos, fMax_Pos); // X 값의 범위 제한
		//selectedKeyframe.vPos.y = ImClamp(selectedKeyframe.vPos.y, fMin_Pos, fMax_Pos); // Y 값의 범위 제한
	}
	style.ItemInnerSpacing.x = originalItemWidth;// 원래의 ItemInnerSpacing 값으로 복원

#pragma region 디스토션
	if (ImGui::Button("Test Button"))
		dynamic_cast<CUI_Distortion*>(m_pCurrSelectUI)->Set_Restore(true);

	if (ImGui::DragFloat("Distortion_TimeAcc", &selectedKeyframe.fTimeAcc)) // UV좌표 조절
	{
		//m_pCurrSelectUI->Set_SequenceTerm(m_fSequenceTerm_Distortion);
	}

	if (ImGui::DragFloat("Distortion_Term", &selectedKeyframe.fSequenceTerm)) // UV좌표 조절
	{
		//m_pCurrSelectUI->Set_SequenceTerm(m_fSequenceTerm_Distortion);
	}

	_float fScrollSpeeds[3];
	fScrollSpeeds[0] = selectedKeyframe.vScrollSpeeds.x;
	fScrollSpeeds[1] = selectedKeyframe.vScrollSpeeds.y;
	fScrollSpeeds[2] = selectedKeyframe.vScrollSpeeds.z;

	if (ImGui::DragFloat3("ScrollSpeeds", fScrollSpeeds))
	{
		_float3 vScrollSpeeds = { 0.f, 0.f, 0.f };
		selectedKeyframe.vScrollSpeeds.x = fScrollSpeeds[0];
		selectedKeyframe.vScrollSpeeds.y = fScrollSpeeds[1];
		selectedKeyframe.vScrollSpeeds.z = fScrollSpeeds[2];
		//vScrollSpeeds.y = m_vScrollSpeeds[1];
		//vScrollSpeeds.z = m_vScrollSpeeds[2];

		//m_pCurrSelectUI->Set_ScrollSpeeds(vScrollSpeeds);
	}

	_float fScales_Distortion[3];
	fScales_Distortion[0] = selectedKeyframe.vScales.x;
	fScales_Distortion[1] = selectedKeyframe.vScales.y;
	fScales_Distortion[2] = selectedKeyframe.vScales.z;
	if (ImGui::DragFloat3("Distortion_Scales", fScales_Distortion))
	{
		_float3 vScales_Distortion = { 0.f, 0.f, 0.f };
		selectedKeyframe.vScales.x = fScales_Distortion[0];
		selectedKeyframe.vScales.y = fScales_Distortion[1];
		selectedKeyframe.vScales.z = fScales_Distortion[2];
		//vScales_Distortion.y = m_vScales_Distortion[1];
		//vScales_Distortion.z = m_vScales_Distortion[2];

		//m_pCurrSelectUI->Set_DistortionScales(vScales_Distortion);
	}

	_float fDistortion1[3];
	fDistortion1[0] = selectedKeyframe.vDistortion1.x;
	fDistortion1[1] = selectedKeyframe.vDistortion1.y;
	if (ImGui::DragFloat2("Distortion1", fDistortion1))
	{
		//_float2 vDistortion1 = { 0.f, 0.f };
		selectedKeyframe.vDistortion1.x = fDistortion1[0];
		selectedKeyframe.vDistortion1.y = fDistortion1[1];

		//m_pCurrSelectUI->Set_Distortion1(vDistortion1);
	}

	_float fDistortion2[3];
	fDistortion2[0] = selectedKeyframe.vDistortion2.x;
	fDistortion2[1] = selectedKeyframe.vDistortion2.y;
	if (ImGui::DragFloat2("Distortion2", fDistortion2))
	{
		//_float2 vDistortion2 = { 0.f, 0.f };
		selectedKeyframe.vDistortion2.x = fDistortion2[0];
		selectedKeyframe.vDistortion2.y = fDistortion2[1];

		//m_pCurrSelectUI->Set_Distortion2(vDistortion2);
	}

	_float fDistortion3[3];
	fDistortion3[0] = selectedKeyframe.vDistortion3.x;
	fDistortion3[1] = selectedKeyframe.vDistortion3.y;
	if (ImGui::DragFloat2("Distortion3", fDistortion3))
	{
		//_float2 vDistortion3 = { 0.f, 0.f };
		selectedKeyframe.vDistortion3.x = fDistortion3[0];
		selectedKeyframe.vDistortion3.y = fDistortion3[1];

		//m_pCurrSelectUI->Set_Distortion3(vDistortion3);
	}
	if (ImGui::DragFloat("Distortion_Scale", &selectedKeyframe.fDistortionScale))
	{
		selectedKeyframe.fDistortionScale = selectedKeyframe.fDistortionScale;
		//m_pCurrSelectUI->Set_DistortionScale(m_fDistortionScale);
	}
	if (ImGui::DragFloat("DistortionBias", &selectedKeyframe.fDistortionBias))
	{
		selectedKeyframe.fDistortionBias = selectedKeyframe.fDistortionBias;
		//m_pCurrSelectUI->Set_DistortionBias(m_fDistortionBias);
	}

#pragma region 설정

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"이징 설정");
	// 이징 설정 (Ease In, Ease Out)
	ImGui::Checkbox("Ease In", &selectedKeyframe.isEaseIn);
	ImGui::SameLine();
	ImGui::Checkbox("Ease Out", &selectedKeyframe.isEaseOut);
	ImGui::SameLine();

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::SeparatorText(u8"Event 설정");

	ImGui::Dummy(ImVec2(0, 5)); // 공백
	ImGui::Checkbox(u8"Active 설정", &selectedKeyframe.bActive);
	ImGui::Dummy(ImVec2(0, 5)); // 공백

	ImGui::Checkbox(u8"Appear 설정", &selectedKeyframe.bAppear);

	ImGui::Checkbox(u8"Disappear 설정", &selectedKeyframe.bDisappear);

	ImGui::Checkbox(u8"Trigger 설정", &selectedKeyframe.bTrigger);

	ImGui::Checkbox(u8"LoopSection 설정", &selectedKeyframe.bLoopSection);

	ImGui::Checkbox(u8"StopPlay 설정", &selectedKeyframe.bStopPlay);

	ImGui::Checkbox(u8"ReversePlay 설정", &selectedKeyframe.bReversePlay);
	 
	ImGui::Checkbox(u8"MaskChange 설정", &selectedKeyframe.bMaskChange);
	ImGui::SameLine();
	ImGui::InputInt("MaskNum", &selectedKeyframe.iMaskNum);

	ImGui::Checkbox(u8"NoiseChange 설정", &selectedKeyframe.bNoiseChange);
	ImGui::SameLine();
	ImGui::InputInt("NoiseNum", &selectedKeyframe.iNoiseNum);

	//ImGui::PopItemWidth(); error

	//// 차징 입력 필드의 가로 길이를 조절
	//ImGui::PushItemWidth(80);
	//ImGui::Checkbox(u8"풀차징시 키프레임", &selectedKeyframe.bFullChargeKeyframe);
	//ImGui::PopItemWidth();

#pragma endregion

	ImGui::End(); // 키프레임 수정 창 끝

	// 선택된 키프레임만 업데이트
	(*m_vecTimeline)[closestKeyframeIndex] = selectedKeyframe;

	//timeline[m_iCurType][closestKeyframeIndex].time = selectedKeyframe.time;
	//timeline[m_iCurType][closestKeyframeIndex].type = selectedKeyframe.type;
	//timeline[m_iCurType][closestKeyframeIndex].value = selectedKeyframe.value;
	//timeline[m_iCurType][closestKeyframeIndex].vPos = selectedKeyframe.vPos;
	//timeline[m_iCurType][closestKeyframeIndex].vRot = selectedKeyframe.vRot;
	//timeline[m_iCurType][closestKeyframeIndex].vScale = selectedKeyframe.vScale;

	//timeline[m_iCurType][closestKeyframeIndex].bChargePossible = selectedKeyframe.bChargePossible;
	//timeline[m_iCurType][closestKeyframeIndex].bFullChargeKeyframe = selectedKeyframe.bFullChargeKeyframe;
	//timeline[m_iCurType][closestKeyframeIndex].bShieldKeyFrame = selectedKeyframe.bShieldKeyFrame;
	//timeline[m_iCurType][closestKeyframeIndex].bShieldPossible = selectedKeyframe.bShieldPossible;
	//timeline[m_iCurType][closestKeyframeIndex].texureframe = selectedKeyframe.texureframe;


	m_iOldIndex = closestKeyframeIndex; // 현재 인덱스 번호를 저장
}

void CWindow_UITool::SelectKeyframeMouse()
{
	ImGuiIO& io = ImGui::GetIO(); // ImGui의 시스템 기본 제공 값들 (ex : 시간, 마우스 등)

	float mouseX = io.MousePos.x; // 마우스 위치x
	float mouseY = io.MousePos.y; // 마우스 위치y

	if (mouseX >= timelinePos.x && mouseX <= timelinePos.x + timelineSize.x &&
		mouseY >= timelinePos.y && mouseY <= timelinePos.y + timelineSize.y)
	{
		// 마우스 왼클릭 감지
		if (io.MouseDown[0])
		{
			// 인덱스 값 초기화
			closestKeyframeIndex = -1;

			// 가장 가까운 키프레임을 찾을 때 초기 거리를 설정하고, 나중에 더 작은 거리를 발견하면 업데이트.
			_float closestDistance = FLT_MAX; // 가장 큰 부동 소수점 (초기 최소 거리 값 성정)

			for (int i = 0; i < m_vecTimeline->size(); ++i)
			{
				CUI::UIKEYFRAME& keyframe = (*m_vecTimeline)[i];

				// 키프레임의 위치
				float keyframeX = keyframe.vKeyFramePos.x;
				float keyframeY = keyframe.vKeyFramePos.y;

				// 마우스 좌표와 키프레임 위치 간의 거리 계산
				float distance = std::sqrt((mouseX - keyframeX) * (mouseX - keyframeX) +
					(mouseY - keyframeY) * (mouseY - keyframeY));

				// 타임 라인 상의 키프레임 위치를 계산
				float xPos = timelinePos.x + (keyframe.fTime / 20.0f) * timelineSize.x;
				float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.fValue;

				// 가장 가까운 키프레임 찾기
				if (distance < closestDistance)
				{
					// 해당 키프레임의 위치 안에 마우스가 있는지 판단
					if (mouseX >= xPos - 5.0f && mouseY >= yPos - 5.0f &&
						mouseX <= xPos + 5.0f && mouseY <= yPos + 5.0f)
					{
						closestDistance = distance;
						closestKeyframeIndex = i; // 현재 선택한 인덱스를 마우스가 클릭한 인덱스로 설정
					}
				}
			}

		}

		// 마우스 이벤트에서 얻은 마우스 좌표
		float mouseRX = io.MousePos.x; // 마우스 위치x
		float mouseRY = io.MousePos.y; // 마우스 위치y

		// 마우스 우클릭 감지
		if (ImGui::IsMouseClicked(1))
		{
			for (int i = 0; i < m_vecTimeline->size(); ++i)
			{
				CUI::UIKEYFRAME& keyframe = (*m_vecTimeline)[i];

				// 키프레임의 위치
				float keyframeX = keyframe.vKeyFramePos.x;
				float keyframeY = keyframe.vKeyFramePos.y;

				// 마우스 좌표와 키프레임 위치 간의 거리 계산
				float distance = std::sqrt((mouseRX - keyframeX) * (mouseRX - keyframeX) +
					(mouseRY - keyframeY) * (mouseRY - keyframeY));

				// 타임 라인 상의 키프레임 위치를 계산
				float xPos = timelinePos.x + (keyframe.fTime / 20.0f) * timelineSize.x;
				float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.fValue;


				if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
				{
					// 오른쪽 마우스 클릭으로 키프레임 삭제
					if (mouseRX >= xPos - 5.0f && mouseRY >= yPos - 5.0f &&
						mouseRX <= xPos + 5.0f && mouseRY <= yPos + 5.0f &&
						ImGui::IsMouseClicked(1))
					{
						// 현재 인덱스가 최대 사이즈 일 경우
						if (i == m_vecTimeline->size() - 1)
						{
							m_vecTimeline->erase(m_vecTimeline->begin() + i); // 현재 클릭한 인덱스 삭제
							closestKeyframeIndex = i - 1;

							break; // 삭제한 후에 루프를 빠져나감.
						}
						else // 최대 사이즈가 아닐 경우
						{
							// 현재 인덱스가 0이거나 0보다 크고 사이즈보다 작을 경우
							if (i < m_vecTimeline->size() - 2 && 0 <= i)
							{
								// 0보다 작거나 같을 경우
								if (i <= 0)
								{
									m_vecTimeline->erase(m_vecTimeline->begin() + i); // 현재 클릭한 인덱스 삭제
									closestKeyframeIndex = -1;

									break; // 삭제한 후에 루프를 빠져나감.
								}
								else
								{
									m_vecTimeline->erase(m_vecTimeline->begin() + i); // 현재 클릭한 인덱스 삭제
									closestKeyframeIndex = i;
								}
							}
						}
						break; // 삭제한 후에 루프를 빠져나감.
					}
				}
			}
		}
	}
}

void CWindow_UITool::Setting_Distortion(_float fTimeDelta)
{
	// 선택한 UI가 없을 경우
	if (nullptr == m_pCurrSelectUI)
		return;

	// 선택한 UI가 Distortion이 있는 UI일 경우
	if (m_pCurrSelectUI->Get_DistortionUI())
	{
		if (ImGui::Button("Test Button"))
			dynamic_cast<CUI_Distortion*>(m_pCurrSelectUI)->Set_Restore(true);

		if (ImGui::DragFloat("Distortion_TimeAcc", &m_fTimeAcc)) // UV좌표 조절
		{
			m_pCurrSelectUI->Set_TimeAcc(m_fTimeAcc);
		}
		if (ImGui::DragFloat("Distortion_Term", &m_fSequenceTerm_Distortion)) // UV좌표 조절
		{
			m_pCurrSelectUI->Set_SequenceTerm(m_fSequenceTerm_Distortion);
		}
		if (ImGui::DragFloat3("ScrollSpeeds", m_vScrollSpeeds))
		{
			_float3 vScrollSpeeds = { 0.f, 0.f, 0.f };
			vScrollSpeeds.x = m_vScrollSpeeds[0];
			vScrollSpeeds.y = m_vScrollSpeeds[1];
			vScrollSpeeds.z = m_vScrollSpeeds[2];
			//vScrollSpeeds.y = m_vScrollSpeeds[1];
			//vScrollSpeeds.z = m_vScrollSpeeds[2];

			m_pCurrSelectUI->Set_ScrollSpeeds(vScrollSpeeds);
		}
		if (ImGui::DragFloat3("Distortion_Scales", m_vScales_Distortion))
		{
			_float3 vScales_Distortion = { 0.f, 0.f, 0.f };
			vScales_Distortion.x = m_vScales_Distortion[0];
			vScales_Distortion.y = m_vScales_Distortion[1];
			vScales_Distortion.z = m_vScales_Distortion[2];
			//vScales_Distortion.y = m_vScales_Distortion[1];
			//vScales_Distortion.z = m_vScales_Distortion[2];

			m_pCurrSelectUI->Set_DistortionScales(vScales_Distortion);
		}
		if (ImGui::DragFloat2("Distortion1", m_vDistortion1))
		{
			_float2 vDistortion1 = { 0.f, 0.f };
			vDistortion1.x = m_vDistortion1[0];
			vDistortion1.y = m_vDistortion1[1];

			m_pCurrSelectUI->Set_Distortion1(vDistortion1);
		}
		if (ImGui::DragFloat2("Distortion2", m_vDistortion2))
		{
			_float2 vDistortion2 = { 0.f, 0.f };
			vDistortion2.x = m_vDistortion2[0];
			vDistortion2.y = m_vDistortion2[1];

			m_pCurrSelectUI->Set_Distortion2(vDistortion2);
		}
		if (ImGui::DragFloat2("Distortion3", m_vDistortion3))
		{
			_float2 vDistortion3 = { 0.f, 0.f };
			vDistortion3.x = m_vDistortion3[0];
			vDistortion3.y = m_vDistortion3[1];

			m_pCurrSelectUI->Set_Distortion3(vDistortion3);
		}
		if (ImGui::DragFloat("Distortion_Scale", &m_fDistortionScale))
		{
			m_pCurrSelectUI->Set_DistortionScale(m_fDistortionScale);
		}
		if (ImGui::DragFloat("DistortionBias", &m_fDistortionBias))
		{
			m_pCurrSelectUI->Set_DistortionBias(m_fDistortionBias);
		}
	}
	CUI::UI_DESC tUI_Desc = m_pCurrSelectUI->Get_UIDesc();
}

_float2 CWindow_UITool::Lerp_float2(const _float2& a, const _float2& b, float t)
{
	_float2 result;
	result.x = a.x + t * (b.x - a.x);
	result.y = a.y + t * (b.y - a.y);
	return result;
}

_float CWindow_UITool::Lerp_float(const _float& a, const _float& b, float t)
{
	_float result;
	result = a + t * (b - a);
	return result;
}

// 쿼드라틱 이징(InQuad) 함수 (시작)
_float CWindow_UITool::ImEaseInQuad(float start, float end, float t)
{
	return start + (end - start) * t * t;
}

// 쿼드라틱 이징(OutQuad) 함수 (끝)
_float CWindow_UITool::ImEaseOutQuad(float start, float end, float t)
{
	return start + (end - start) * (-t * (t - 2));
}

HRESULT CWindow_UITool::Save_Function(string strPath, string strFileName)
{
	_ushort iIndex = 0;

	strPath = strPath + "\\" + strFileName;

	json Out_Json;

	if (!m_vecParentObject.empty())
	{
		for (auto& Parent : m_vecParentObject)
		{
			json Out_Object;

			dynamic_cast<CUI*>(Parent)->Save_Desc(Out_Object);
			Out_Json.emplace(to_string(iIndex++), Out_Object);
		}
	}

	if (m_vecParentGroup != nullptr)
	{
		if (!m_vecParentGroup->empty())
		{
			for (auto& Group : m_vecParentObject)
			{
				json Out_Object;

				dynamic_cast<CUI*>(Group)->Save_Desc(Out_Object);
				Out_Json.emplace(to_string(iIndex++), Out_Object);
			}
		}
	}

	if (!m_vecChildObject.empty())
	{
		for (auto& Child : m_vecChildObject)
		{
			json Out_Object;

			dynamic_cast<CUI*>(Child)->Save_Desc(Out_Object);
			Out_Json.emplace(to_string(iIndex++), Out_Object);
		}
	}
	CJson_Utility::Save_Json(ConverWStringtoC(ConvertToWideString(strPath)), Out_Json);

	m_bShortOff = false;

	return S_OK;
}

HRESULT CWindow_UITool::Load_Function(string strPath, string strFileName)
{
	json json_in;

	CGameObject* pGameObject = nullptr;
	CUI* pUI_Object = nullptr;

	//char filePath[MAX_PATH];

	string strFile;

	strFile = strPath + "\\" + strFileName;
	
	CJson_Utility::Load_Json(strFile.c_str(), json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		if (object.contains("Parent"))
			tUI_Info.bParent = object["Parent"];					// 1. Parent
		if (object.contains("World"))
			tUI_Info.bWorld = object["World"];						// 2. World
		if (object.contains("Group"))
			tUI_Info.bGroup = object["Group"];						// 3. Group
		if (object.contains("Alpha"))
			tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		if (object.contains("AlphaTrue"))
			tUI_Info.fAlphaTrue = object["AlphaTrue"];				// 4. Alpha
		if (object.contains("ObjectNum"))
			tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		if (object.contains("ShaderNum"))
			tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		if (object.contains("UINum"))								// "ObjectName" 키가 있으면
			tUI_Info.iUINum = object["UINum"];
		if (object.contains("UIName"))								// "ObjectName" 키가 있으면
			tUI_Info.strUIName = object["UIName"];
		if (object.contains("ObjectName"))							// "ObjectName" 키가 있으면
			tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		if (object.contains("LayerTag"))
			tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		if (object.contains("CloneTag"))
			tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		if (object.contains("ProtoTag"))
			tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		if (object.contains("FilePath"))
			tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		if (object.contains("MapTextureTag"))
			tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		if (object.contains("ColorR"))
			tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		if (object.contains("ColorG"))
			tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		if (object.contains("ColorB"))
			tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		if (object.contains("ColorA"))
			tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A
		if (object.contains("ColorMode"))
			tUI_Info.eColorMode = object["ColorMode"];				// 17. Mode
		if (object.contains("RenderGroup"))
			tUI_Info.iRenderGroup = object["RenderGroup"];			// 18. RenderGroup
		if (object.contains("Level"))
			tUI_Info.iLevel = object["Level"];						// 19. RenderGroup
		if (object.contains("MaxLevel"))
			tUI_Info.iMaxLevel = object["MaxLevel"];				// 20. RenderGroup


		wstring wstrLayer = TEXT("");
		if (m_strLayer[m_iCurrLayerNum] != "")
			m_pGameInstance->String_To_WString(m_strLayer[m_iCurrLayerNum], wstrLayer);

		wstring wstrClonetag = TEXT("");
		if(tUI_Info.strCloneTag != "")
			m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag = TEXT("");
		if (tUI_Info.strProtoTag != "")
			m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath = TEXT("");
		if (tUI_Info.strFilePath != "")
			m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);


		if (wstrClonetag != TEXT(""))
			pGameObject = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, wstrLayer, wstrClonetag, &tUI_Info);
		

		if (pGameObject != nullptr)
			m_pCurrSelectUI = dynamic_cast<CUI*>(pGameObject);

		if (m_pCurrSelectUI == nullptr)
			return E_FAIL;

		m_pCurrSelectUI->Get_Transform()->Load_FromJson(object); // 17. TransformCom

		//tUI_Info.iKeyframeNum = object["KeyframeNum"];	// 18. KeyframeNum
		
		// "KeyframeNum" 키가 없으면 기본값 사용
		_bool bKeyframeNum = object.contains("KeyframeNum");
		tUI_Info.iKeyframeNum = bKeyframeNum ? object["KeyframeNum"] : 0;

		if (wstrClonetag == TEXT("") &&
			tUI_Info.iKeyframeNum > 0)
		{
			m_pCurrSelectUI->Clear_Keyframe();
		}

		for (_int i = 0; i < tUI_Info.iKeyframeNum; ++i) // 19. Keyframe
		{
			/* Keyframe */
			tUI_Info.tKeyframe.fTime = object["Keyframe"][i]["Time"];
			tUI_Info.tKeyframe.fValue = object["Keyframe"][i]["Value"];
			tUI_Info.tKeyframe.fAnimSpeed = object["Keyframe"][i]["AnimSpeed"];
			tUI_Info.tKeyframe.iType = object["Keyframe"][i]["Type"];
			tUI_Info.tKeyframe.isEaseIn = object["Keyframe"][i]["EaseIn"];
			tUI_Info.tKeyframe.isEaseOut = object["Keyframe"][i]["EaseOut"];
			tUI_Info.tKeyframe.iTexureframe = object["Keyframe"][i]["Texureframe"];

			/* 2D */
			tUI_Info.tKeyframe.vScale.x = object["Keyframe"][i]["ScaleX"];
			tUI_Info.tKeyframe.vScale.y = object["Keyframe"][i]["ScaleY"];
			tUI_Info.tKeyframe.vPos.x = object["Keyframe"][i]["PosX"];
			tUI_Info.tKeyframe.vPos.y = object["Keyframe"][i]["PosY"];
			tUI_Info.tKeyframe.fRot = object["Keyframe"][i]["Rot"];

			/* 3D */
			if (object["Keyframe"][i].contains("World_ScaleX")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Scale.x = object["Keyframe"][i]["World_ScaleX"];
			if (object["Keyframe"][i].contains("World_ScaleY")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Scale.y = object["Keyframe"][i]["World_ScaleY"];
			if (object["Keyframe"][i].contains("World_ScaleZ")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Scale.z = object["Keyframe"][i]["World_ScaleZ"];
			if (object["Keyframe"][i].contains("World_PosX")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Pos.x = object["Keyframe"][i]["World_PosX"];
			if (object["Keyframe"][i].contains("World_PosY")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Pos.y = object["Keyframe"][i]["World_PosY"];
			if (object["Keyframe"][i].contains("World_PosZ")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Pos.z = object["Keyframe"][i]["World_PosZ"];
			if (object["Keyframe"][i].contains("World_RotX")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Rot.x = object["Keyframe"][i]["World_RotX"];
			if (object["Keyframe"][i].contains("World_RotY")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Rot.y = object["Keyframe"][i]["World_RotY"];
			if (object["Keyframe"][i].contains("World_RotZ")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.vWorld_Rot.z = object["Keyframe"][i]["World_RotZ"];

			tUI_Info.tKeyframe.vKeyFramePos.x = object["Keyframe"][i]["KeyFramePosX"];
			tUI_Info.tKeyframe.vKeyFramePos.y = object["Keyframe"][i]["KeyFramePosY"];

			tUI_Info.tKeyframe.fAlpha = object["Keyframe"][i]["Alpha"];
			tUI_Info.tKeyframe.bActive = object["Keyframe"][i]["Active"];
			tUI_Info.tKeyframe.bAppear = object["Keyframe"][i]["Appear"];
			tUI_Info.tKeyframe.bTrigger = object["Keyframe"][i]["Trigger"];

			if (object["Keyframe"][i].contains("Disappear")) // "Disappear" 키가 있으면
				tUI_Info.tKeyframe.bDisappear = object["Keyframe"][i]["Disappear"];

			if (object["Keyframe"][i].contains("LoopSection"))// "LoopSection" 키가 있으면
				tUI_Info.tKeyframe.bLoopSection = object["Keyframe"][i]["LoopSection"];

			if (object["Keyframe"][i].contains("DistortionUI")) // 키가 있으면
				tUI_Info.tKeyframe.bDistortionUI = object["Keyframe"][i]["DistortionUI"];
			if (object["Keyframe"][i].contains("Restore")) // 키가 있으면
				tUI_Info.tKeyframe.bRestore = object["Keyframe"][i]["Restore"];
			if (object["Keyframe"][i].contains("TimeAcc")) // 키가 있으면
				tUI_Info.tKeyframe.fTimeAcc = object["Keyframe"][i]["TimeAcc"];
			if (object["Keyframe"][i].contains("SequenceTerm")) // 키가 있으면
				tUI_Info.tKeyframe.fSequenceTerm = object["Keyframe"][i]["SequenceTerm"];
			if (object["Keyframe"][i].contains("ScrollSpeedsX")) // 키가 있으면
				tUI_Info.tKeyframe.vScrollSpeeds.x = object["Keyframe"][i]["ScrollSpeedsX"];
			if (object["Keyframe"][i].contains("ScrollSpeedsY")) // 키가 있으면
				tUI_Info.tKeyframe.vScrollSpeeds.y = object["Keyframe"][i]["ScrollSpeedsY"];
			if (object["Keyframe"][i].contains("ScrollSpeedsZ")) // 키가 있으면
				tUI_Info.tKeyframe.vScrollSpeeds.z = object["Keyframe"][i]["ScrollSpeedsZ"];
			if (object["Keyframe"][i].contains("ScalesX")) // 키가 있으면
				tUI_Info.tKeyframe.vScales.x = object["Keyframe"][i]["ScalesX"];
			if (object["Keyframe"][i].contains("ScalesY")) // 키가 있으면
				tUI_Info.tKeyframe.vScales.y = object["Keyframe"][i]["ScalesY"];
			if (object["Keyframe"][i].contains("ScalesZ")) // 키가 있으면
				tUI_Info.tKeyframe.vScales.z = object["Keyframe"][i]["ScalesZ"];
			if (object["Keyframe"][i].contains("Distortion1X")) // 키가 있으면
				tUI_Info.tKeyframe.vDistortion1.x = object["Keyframe"][i]["Distortion1X"];
			if (object["Keyframe"][i].contains("Distortion1Y")) // 키가 있으면
				tUI_Info.tKeyframe.vDistortion1.y = object["Keyframe"][i]["Distortion1Y"];
			if (object["Keyframe"][i].contains("Distortion2X")) // 키가 있으면
				tUI_Info.tKeyframe.vDistortion2.x = object["Keyframe"][i]["Distortion2X"];
			if (object["Keyframe"][i].contains("Distortion2Y")) // 키가 있으면
				tUI_Info.tKeyframe.vDistortion2.y = object["Keyframe"][i]["Distortion2Y"];
			if (object["Keyframe"][i].contains("Distortion3X")) // 키가 있으면
				tUI_Info.tKeyframe.vDistortion3.x = object["Keyframe"][i]["Distortion3X"];
			if (object["Keyframe"][i].contains("Distortion3Y")) // 키가 있으면
				tUI_Info.tKeyframe.vDistortion3.y = object["Keyframe"][i]["Distortion3Y"];
			if (object["Keyframe"][i].contains("DistortionScale")) // 키가 있으면
				tUI_Info.tKeyframe.fDistortionScale = object["Keyframe"][i]["DistortionScale"];
			if (object["Keyframe"][i].contains("DistortionBias")) // 키가 있으면
				tUI_Info.tKeyframe.fDistortionBias = object["Keyframe"][i]["DistortionBias"];

			if (object["Keyframe"][i].contains("StopPlay")) // 키가 있으면
				tUI_Info.tKeyframe.bStopPlay = object["Keyframe"][i]["StopPlay"];
			if (object["Keyframe"][i].contains("ReversePlay")) // 키가 있으면
				tUI_Info.tKeyframe.bReversePlay = object["Keyframe"][i]["ReversePlay"];
			if (object["Keyframe"][i].contains("MaskChange")) // 키가 있으면
				tUI_Info.tKeyframe.bMaskChange = object["Keyframe"][i]["MaskChange"];
			if (object["Keyframe"][i].contains("NoiseChange")) // 키가 있으면
				tUI_Info.tKeyframe.bNoiseChange = object["Keyframe"][i]["NoiseChange"];
			if (object["Keyframe"][i].contains("MaskNum")) // 키가 있으면
				tUI_Info.tKeyframe.iMaskNum = object["Keyframe"][i]["MaskNum"];
			if (object["Keyframe"][i].contains("NoiseNum")) // 키가 있으면
				tUI_Info.tKeyframe.iNoiseNum = object["Keyframe"][i]["NoiseNum"];

			m_pCurrSelectUI->Add_Keyframe(tUI_Info.tKeyframe);
		}

		if (object.contains("Distortion")) // 키가 있으면
		{

			if (object["Distortion"].contains("TimeAcc")) // 키가 있으면
				tUI_Info.fTimeAcc = object["Distortion"]["TimeAcc"];
			if (object["Distortion"].contains("ScrollSpeedsX")) // 키가 있으면
				tUI_Info.vScrollSpeeds.x = object["Distortion"]["ScrollSpeedsX"];
			if (object["Distortion"].contains("ScrollSpeedsY")) // 키가 있으면
				tUI_Info.vScrollSpeeds.y = object["Distortion"]["ScrollSpeedsY"];
			if (object["Distortion"].contains("ScrollSpeedsZ")) // 키가 있으면
				tUI_Info.vScrollSpeeds.z = object["Distortion"]["ScrollSpeedsZ"];
			if (object["Distortion"].contains("ScalesX")) // 키가 있으면
				tUI_Info.vScales.x = object["Distortion"]["ScalesX"];
			if (object["Distortion"].contains("ScalesY")) // 키가 있으면
				tUI_Info.vScales.y = object["Distortion"]["ScalesY"];
			if (object["Distortion"].contains("ScalesZ")) // 키가 있으면
				tUI_Info.vScales.z = object["Distortion"]["ScalesZ"];
			if (object["Distortion"].contains("Distortion1X")) // 키가 있으면
				tUI_Info.vDistortion1.x = object["Distortion"]["Distortion1X"];
			if (object["Distortion"].contains("Distortion1Y")) // 키가 있으면
				tUI_Info.vDistortion1.y = object["Distortion"]["Distortion1Y"];
			if (object["Distortion"].contains("Distortion2X")) // 키가 있으면
				tUI_Info.vDistortion2.x = object["Distortion"]["Distortion2X"];
			if (object["Distortion"].contains("Distortion2Y")) // 키가 있으면
				tUI_Info.vDistortion2.y = object["Distortion"]["Distortion2Y"];
			if (object["Distortion"].contains("Distortion3X")) // 키가 있으면
				tUI_Info.vDistortion3.x = object["Distortion"]["Distortion3X"];
			if (object["Distortion"].contains("Distortion3Y")) // 키가 있으면
				tUI_Info.vDistortion3.y = object["Distortion"]["Distortion3Y"];
			if (object["Distortion"].contains("DistortionScale")) // 키가 있으면
				tUI_Info.fDistortionScale = object["Distortion"]["DistortionScale"];

			if (object["Distortion"].contains("DistortionUI")) // 키가 있으면
				tUI_Info.bDistortionUI = object["Distortion"]["DistortionUI"];
			if (object["Distortion"].contains("MaskNum")) // 키가 있으면
				tUI_Info.iMaskNum = object["Distortion"]["MaskNum"];
			if (object["Distortion"].contains("NoiseNum")) // 키가 있으면
				tUI_Info.iNoiseNum = object["Distortion"]["NoiseNum"];
		}

		if (wstrClonetag != TEXT(""))
			m_pCurrSelectUI->Set_UIDesc(tUI_Info);

		if (tUI_Info.bParent == true)
		{
			if (wstrClonetag != TEXT(""))
			{
				m_vecParentObject.push_back(m_pCurrSelectUI);
				Add_ParentList(tUI_Info);
			}
		}
		else
		{
			if (wstrClonetag != TEXT(""))
			{
				m_vecChildObject.push_back(m_pCurrSelectUI);
				Add_ChildList(tUI_Info);
			}
		}
	}

	return S_OK;
}

void CWindow_UITool::Save_Animation(string& strPath, string& strFileName)
{
	_ushort iIndex = 0;

	//strPath = strPath + "\\" + strFileName;
	string strFilePath = "../Bin/DataFiles/Data_UI/Animation/" + strFileName + ".json";

	json Out_Json;

	if (!m_vecChildObject.empty())
	{
		for (auto& Child : m_vecChildObject)
		{
			json Out_Object;

			m_pCurrSelectUI->Save_Animation(Out_Object);
			Out_Json.emplace(to_string(iIndex++), Out_Object);
		}
	}

	CJson_Utility::Save_Json(ConverWStringtoC(ConvertToWideString(strFilePath)), Out_Json);
}

void CWindow_UITool::Change_Animation(const string& strAnimation)
{
	json json_in;
	string strFile;

	// FilePath
	strFile = "../Bin/DataFiles/Data_UI/Animation/" + strAnimation + ".json";

	CJson_Utility::Load_Json(strFile.c_str(), json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		m_pCurrSelectUI->Change_Animation(strFile); // Animation Change
	}
}

_bool CWindow_UITool::Check_ImGui_Rect()
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

void CWindow_UITool::Load_Paths()
{
	json json_in;

	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_UI/Texture_Info/Texture_Info";

	CJson_Utility::Load_Json(filePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		PATHINFO* tPath = new PATHINFO;

		tPath->strFileName = object["FileName"];
		tPath->strFilePath = object["FilePath"];
		tPath->iPathNum = object["PathNum"];

		m_vecInitialPaths.push_back(tPath);

		tPath = nullptr;
		delete[] tPath;
	}
}

// ImGui를 사용하여 이미지를 표시하는 함수
void CWindow_UITool::ShowImagePreview(const std::vector<unsigned char>& imageData, int width, int height)
{
	ImVec2 imageSize((_float)width, (_float)height);
	ImGui::Image(reinterpret_cast<void*>(const_cast<unsigned char*>(imageData.data())), imageSize);
}

void CWindow_UITool::UI_Preset()
{
	/* Test 프리셋 */
	if (ImGui::Button("Interface"))
	{
		m_pUIManager->Ready_Interface(LEVEL_STATIC);
	}
	if (ImGui::Button("Crosshair"))
	{
		m_pUIManager->Ready_Crosshair(LEVEL_STATIC);
	}
	if (ImGui::Button("Loading_Intro"))
	{
		m_pUIManager->Ready_Loading_Intro(LEVEL_STATIC);
	}
	if (ImGui::Button("BossHUD_Bar"))
	{
		m_pUIManager->Ready_BossHUD_Bar(LEVEL_STATIC);
	}
	if (ImGui::Button("EnemyHUD_Shard"))
	{
		m_pUIManager->Ready_EnemyHUD_Shard(LEVEL_STATIC);
	}
	if (ImGui::Button("DiedScreen"))
	{
		m_pUIManager->Ready_DiedScreen(LEVEL_STATIC);
	}
	ImGui::SameLine();
	if (ImGui::Button("TestPlayDied"))
	{
		m_pUIManager->Active_DiedScreen();
	}
	ImGui::SameLine();
	if (ImGui::Button("TestNotPlayDied"))
	{
		m_pUIManager->NonActive_DiedScreen();
	}
	if (ImGui::Button("Option"))
	{
		m_pUIManager->Ready_Option(LEVEL_STATIC);
	}
	if (ImGui::Button("TestPlayOption"))
	{
		m_pUIManager->Active_Option();
	}
	ImGui::SameLine();
	if (ImGui::Button("TestNotPlayOption"))
	{
		m_pUIManager->NonActive_Option();
	}

	if (ImGui::Button("MainMenu"))
	{
		m_pUIManager->Ready_MainMenu(LEVEL_STATIC);
	}
	if (ImGui::Button("MainListPlay"))
	{
		m_pUIManager->Active_MainMenu();
	}
	ImGui::SameLine();
	if (ImGui::Button("MainListNonPlay"))
	{
		m_pUIManager->NonActive_MainList();
	}
	ImGui::SameLine();
	if (ImGui::Button("MainLogoPlay"))
	{
		m_pUIManager->Active_MainLogo();
	}
	ImGui::SameLine();
	if (ImGui::Button("LevelListPlay"))
	{
		m_pUIManager->Active_LevelList();
	}
	if (ImGui::Button("SkillWindowButton"))
	{
		m_pUIManager->Ready_SkillWindowButton(LEVEL_STATIC);
	}
	ImGui::SameLine();
	if (ImGui::Button("SkillWindowButtonActive"))
	{
		m_pUIManager->Active_SkillWindowButton(true);
	}
	if (ImGui::Button("SkillWindowAll"))
	{
		m_pUIManager->Ready_SkillWindow(true);
	}
	if (ImGui::Button("SkillWindowAllActive"))
	{
		m_pUIManager->Active_SkillWindowBackground();
	}
}

CWindow_UITool* CWindow_UITool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_UITool* pInstance = new CWindow_UITool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_UITool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_UITool::Free()
{
	__super::Free();

	//if (!m_vecChildObjectName.empty())
	//	Safe_Delete(m_vecChildObjectName);
	//if (!m_vecImagePaths.empty())
	//	Safe_Delete(m_vecImagePaths);
	//if (!m_vecInitialPaths.empty())
	//	Safe_Delete(m_vecInitialPaths);
	//if (!m_vecTexture.empty())
	//	Safe_Delete(m_vecTexture);
	//if (!m_vecChildObject.empty())
	//	Safe_Delete(m_vecChildObject);
	
	// Safe_Delete(m_pCurrChild);
	// Safe_Delete(m_pCurrParent);

	m_pCurrChild = nullptr;
	m_pCurrParent = nullptr;

	//if (!m_vecChildObjectName.empty())
	//{
	//	for (auto& path : m_vecChildObjectName)
	//	{
	//		delete path;
	//	}
	//	m_vecChildObjectName.clear();
	//}
	if (m_vecParentGroup != nullptr)
	{
		//Safe_Delete(m_vecParentGroup);
		//m_vecParentGroup = nullptr;
	}
	if (!m_vecImagePaths.empty())
	{
		for (auto& path : m_vecImagePaths)
		{
			delete path;
		}
		m_vecImagePaths.clear();
	}
	if (!m_vecInitialPaths.empty())
	{
		for (auto& path : m_vecInitialPaths)
		{
			delete path;
		}
		m_vecInitialPaths.clear();
	}
	if (!m_vecTexture.empty())
	{
		for (auto& path : m_vecTexture)
		{
			delete path;
		}
		m_vecTexture.clear();
	}

	if (m_pUIManager)
	{
		Safe_Release(m_pUIManager);
	}
	//if (!m_vecTimeline->empty())
	//{
	//	Safe_Release(m_vecTimeline);
	//}
	
	//if (!m_vecChildObject.empty())
	//{
	//	for (auto& path : m_vecChildObject) {
	//		delete path;
	//	}
	//	m_vecChildObject.clear();
	//}


}