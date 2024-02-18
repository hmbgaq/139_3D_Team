#include "stdafx.h"

//#define IMGUI_DEFINE_MATH_OPERATORS
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Imgui_Window.h"
#include "Include_Imgui_Windows.h"


ImGuiIO g_io;
IMPLEMENT_SINGLETON(CImgui_Manager);

CImgui_Manager::CImgui_Manager()
{
}

#pragma region 이니셜라이즈

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	g_io = ImGui::GetIO(); (void)g_io;
	g_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls


	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if (false == ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if (false == ImGui_ImplDX11_Init(m_pDevice, m_pContext))
		return E_FAIL;

	//ImGui::StyleColorsDark();
	g_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, g_io.Fonts->GetGlyphRangesKorean());

	if(FAILED(Ready_Windows()))
		return E_FAIL;


	_int iSize = _int(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_WINDOW_END);

	for (_int i = 0; i < iSize; ++i)
	{
		m_bEnableTool[i] = false;
	}


	return S_OK;

}

void CImgui_Manager::ImGuizmo_Initialize()
{
}

void CImgui_Manager::ImGuiFileDialog_Initialize()
{
}

HRESULT CImgui_Manager::Ready_Windows()
{
	//TODO ImGuiWindowFlags WindowFlag
		// ImGuiWindowFlags_MenuBar |
		// ImGuiWindowFlags_NoDocking |
		// ImGuiWindowFlags_NoTitleBar |
		// ImGuiWindowFlags_NoCollapse |
		// ImGuiWindowFlags_NoResize |
		// ImGuiWindowFlags_NoMove |
		// ImGuiWindowFlags_NoBringToFrontOnFocus |
		// ImGuiWindowFlags_NoNavFocus;


	//! 여기다가 자기가 필요한 툴 윈도우 객체 추가해서 쓰셈. 아래는 예시

	
#pragma region 맵툴
	CImgui_Window* pWindow = CWindow_MapTool::Create(m_pDevice, m_pContext);

	if (pWindow == nullptr)
		return E_FAIL;

	pWindow->SetUp_ImGuiDESC(u8"맵툴", ImVec2{ 200.f, 200.f }, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus, ImVec4(0.f, 0.f, 0.f, 1.f));

	m_mapWindows.emplace(IMGUI_WINDOW_TYPE::IMGUI_MAPTOOL_WINDOW, pWindow);
#pragma endregion 맵툴

	
#pragma region 애니메이션툴

	pWindow = CWindow_AnimTool::Create(m_pDevice, m_pContext);

	if (pWindow == nullptr)
		return E_FAIL;

	pWindow->SetUp_ImGuiDESC(u8"애니메이션툴", ImVec2{ 300.f,300.f }, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus, ImVec4(0.f, 0.f, 0.f, 1.f));

	m_mapWindows.emplace(IMGUI_WINDOW_TYPE::IMGUI_ANIMATIONTOOL_WINDOW, pWindow);
#pragma endregion 애니메이션툴

#pragma region 이펙트툴
	pWindow = CWindow_EffectTool::Create(m_pDevice, m_pContext);
	if (pWindow == nullptr)
		return E_FAIL;

	pWindow->SetUp_ImGuiDESC(u8"이펙트 툴", ImVec2{ 300.f, 680.f }, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | /*ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |*/ ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 1.f));
	m_mapWindows.emplace(IMGUI_WINDOW_TYPE::IMGUI_EFFECTTOOL_WINDOW, pWindow);
#pragma endregion 이펙트툴

#pragma region UI_START
	/* 툴상에 새 윈도우를 만들어준다. */
	CImgui_Window* pWindowUI = CWindow_UITool::Create(m_pDevice, m_pContext);

	if (pWindowUI == nullptr)
		return E_FAIL;

	/* UI윈도우에대한 옵션 설정 */
	pWindowUI->SetUp_ImGuiDESC(u8"UI툴", ImVec2{ 400.f, 600.f }, ImGuiWindowFlags_MenuBar, ImVec4(0.f, 0.f, 0.f, 1.f) /*| ImGuiWindowFlags_NoCollapse*/ /*| ImGuiWindowFlags_NoBringToFrontOnFocus*/ /*| ImGuiWindowFlags_NoNavFocus*/);

	/* 세팅된 윈도우를 추가시켜준다. */
	m_mapWindows.emplace(IMGUI_WINDOW_TYPE::IMGUI_UITOOL_WINDOW, pWindowUI);
#pragma endregion UI_END

	return S_OK;
}



#pragma endregion 이니셜라이즈

void CImgui_Manager::Tick(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGuizmo::BeginFrame();

	/////* ---------------------- IMGUI ---------------------- */
	//ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	//
	//auto& style = ImGui::GetStyle();
	//ImVec4* colors = style.Colors;
	//
	///* 투명하게 */
	//const ImVec4 bgColor = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
	//colors[ImGuiCol_WindowBg] = bgColor;
	//colors[ImGuiCol_ChildBg] = bgColor;
	//colors[ImGuiCol_TitleBg] = bgColor;
	//
	//ImGui::SetNextWindowSize(ImVec2(g_iWinSizeX, g_iWinSizeY), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);


	//! 최상단 메뉴바와 관련된 함수
	MenuTick(fTimeDelta);

 	for (auto& pWindowPair : m_mapWindows)
 	{
 		if(true == pWindowPair.second->Is_Enable()) //!현재 윈도우가 활성화상태인지 체크.
 			pWindowPair.second->Tick(fTimeDelta);
 	}

}

void CImgui_Manager::MenuTick(_float fTimeDelta)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Tools"))
		{
 			if (ImGui::MenuItem("MapTool", nullptr, m_bEnableTool[(_int)IMGUI_WINDOW_TYPE::IMGUI_MAPTOOL_WINDOW]))
 			{
 				CImgui_Window* pWindow = Find_Window(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_MAPTOOL_WINDOW);

 				if (nullptr == pWindow)
 				{
 					MSG_BOX("맵 윈도우가 없음. Ready_Window 함수 확인 바람");
 					return;
 				}

 				pWindow->Set_Enable(!pWindow->Is_Enable()); //! 기존에 활성화 상태를 부정으로
 			}

 			if (ImGui::MenuItem("AnimationTool", nullptr, m_bEnableTool[(_int)IMGUI_WINDOW_TYPE::IMGUI_ANIMATIONTOOL_WINDOW]))
 			{
 				CImgui_Window* pWindow = Find_Window(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_ANIMATIONTOOL_WINDOW);
				
 				if (nullptr == pWindow)
 				{
 					MSG_BOX("애니메이션 윈도우가 없음. Ready_Window 함수 확인 바람");
 					return;
 				}

 				pWindow->Set_Enable(!pWindow->Is_Enable()); //! 기존에 활성화 상태를 부정으로o
 			}

 			if (ImGui::MenuItem("EffectTool", nullptr, m_bEnableTool[(_int)IMGUI_WINDOW_TYPE::IMGUI_EFFECTTOOL_WINDOW]))
 			{
 				CImgui_Window* pWindow = Find_Window(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_EFFECTTOOL_WINDOW);

 				if (nullptr == pWindow)
 				{
 					MSG_BOX("이펙트 윈도우가 없음. Ready_Window 함수 확인 바람");
 					return;
 				}

 				pWindow->Set_Enable(!pWindow->Is_Enable()); //! 기존에 활성화 상태를 부정으로
 			}

 			if (ImGui::MenuItem("UITool", nullptr, m_bEnableTool[(_int)IMGUI_WINDOW_TYPE::IMGUI_UITOOL_WINDOW]))
 			{
 				CImgui_Window* pWindow = Find_Window(CImgui_Manager::IMGUI_WINDOW_TYPE::IMGUI_UITOOL_WINDOW);

 				if (nullptr == pWindow)
 				{
 					MSG_BOX("UI 윈도우가 없음. Ready_Window 함수 확인 바람");
 					return;
 				}

 				pWindow->Set_Enable(!pWindow->Is_Enable()); //! 기존에 활성화 상태를 부정으로
 			}

 			ImGui::EndMenu();
		}
		
		
		ImGui::EndMainMenuBar();
		
	}

	
}

void CImgui_Manager::Render()
{
	//!최초 한번 Render 전용. 이유는 Header파일 멤버변수 m_bFirstTick 주석 확인해볼것.
	if (false == m_bFirstTick)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		m_bFirstTick = true;
	}


	for (auto& pWindowPair : m_mapWindows)
	{
		if (true == pWindowPair.second->Is_Enable()) //!현재 윈도우가 활성화상태인지 체크.
			pWindowPair.second->Render();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
}


CImgui_Window* CImgui_Manager::Find_Window(IMGUI_WINDOW_TYPE eWindowType)
{
	auto iter = m_mapWindows.find(eWindowType);

	if (iter == m_mapWindows.end())
		return nullptr;

	return iter->second;
}

_bool CImgui_Manager::Check_ImGui_Rect()
{
	POINT tMouse = {};
	GetCursorPos(&tMouse);
	ScreenToClient(m_pGameInstance->Get_GraphicDesc()->hWnd, &tMouse);

	ImVec2 windowPos = ImGui::GetWindowPos(); //왼쪽상단모서리점
	ImVec2 windowSize = ImGui::GetWindowSize();

	if (tMouse.x >= windowPos.x && tMouse.x <= windowPos.x + windowSize.x &&
		tMouse.y >= windowPos.y && tMouse.y <= windowPos.y + windowSize.y)
	{
		return false; //ImGui 영역 내
	}
	return true; //ImGui 영역이랑 안 겹침!
}

void CImgui_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for(auto& pWindowPair : m_mapWindows)
		Safe_Release(pWindowPair.second);

	m_mapWindows.clear();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
