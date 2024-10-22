#include "stdafx.h"

#include "Imgui_Window.h"
#include "Imgui_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "../Imgui/ImGuizmo/ImGuizmo.h"
#include "../Imgui/ImGuizmo/ImCurveEdit.h"
#include "../Imgui/ImGuizmo/GraphEditor.h"
#include "../Imgui/ImGuizmo/ImSequencer.h"
#include "../Imgui/ImGuizmo/ImZoomSlider.h"
#include "CustomDialogFont.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "UI.h"

static ImGuizmo::OPERATION mCurrentGizmoOperation;
static ImGuizmo::MODE	   mCurrentGizmoMode;
static bool useSnap(false);
static bool useSnapUI(false);

ImGuiFileDialog* g_pFileDialog;

CImgui_Window::CImgui_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

}

HRESULT CImgui_Window::SetUp_ImGuiDESC(string _strName, const ImVec2& _vWindowSize, ImGuiWindowFlags _eWindowFlags, const ImVec4& _vBackGroundColor)
{
	m_tImGuiDESC.strName = _strName;
	m_tImGuiDESC.vWindowSize = _vWindowSize;
	m_tImGuiDESC.eWindowFlags = _eWindowFlags;
	m_tImGuiDESC.vBackgroundColor = _vBackGroundColor;

	return S_OK;
}

HRESULT CImgui_Window::Initialize()
{
	
	//TODO For. ImguiFileDialog
	g_pFileDialog = ImGuiFileDialog::Instance();
	//FileDialog 파일별 색
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "((Custom.+[.]h))", ImVec4(0.1f, 0.9f, 0.1f, 0.9f));  // use a regex
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);  // add an icon for the filter type
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");             // add an text for a filter type
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);     // for all dirs
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, nullptr, ImVec4(0.2f, 0.9f, 0.2f, 0.9f), ICON_IGFD_FILE);                              // for all link files
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByTypeLink, nullptr, ImVec4(0.8f, 0.8f, 0.8f, 0.8f), ICON_IGFD_FOLDER);  // for all link dirs
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByTypeLink, nullptr, ImVec4(0.8f, 0.8f, 0.8f, 0.8f), ICON_IGFD_FILE);   // for all link files
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

	//TODO For.Guizmo
	m_arrView = new _float[16];
	m_arrProj = new _float[16];
	m_arrOrthoProj = new _float[16];

	Set_GuizmoCamView();
	Set_GuizmoCamProj();
	Set_GuizmoOrthographicLH();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_FirstUseEver);

	return S_OK;
}

void CImgui_Window::Tick(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	ImGuizmo::BeginFrame();
	/*
	기즈모 세팅 예시 : 틱 마다 돌 수 있게 세팅해주세요.
	Set_GuizmoCamView();
	Set_GuizmoCamProj();
	Set_Guizmo(기즈모를 달고싶은 대상 오브젝트를 넣어주세요.);
	*/
}

void CImgui_Window::OpenDialog(WINDOW_TYPE eWindowType)
{
	string strTitle, strPath, strAdd;

	strPath = m_strDialogPath;

	if (m_eDialogType == DIALOG_TYPE::SAVE_DIALOG)
		strAdd = u8" 저장";
	else if (m_eDialogType == DIALOG_TYPE::LOAD_DIALOG)
		strAdd = u8" 불러오기";

	const _char* szFilters = "Json (*.json){.json},Binary (*.dat, *.vfx,){.dat,.vfx},Instance (*.dat){.dat},All files{.*}";

	switch (eWindowType)
	{
		case WINDOW_TYPE::IMGUI_MAPTOOL_WINDOW:
		{
			m_strDialogKey = "MapToolDialog";
			strTitle = u8"맵 툴" + strAdd;
			break;
		}

		case WINDOW_TYPE::IMGUI_ANIMATIONTOOL_WINDOW:
		{
			m_strDialogKey = "AnimationToolDialog";
			strTitle = u8"애니메이션 툴" + strAdd;
			break;
		}

		case WINDOW_TYPE::IMGUI_EFFECTTOOL_WINDOW:
		{
			m_strDialogKey = "EffectToolDialog";
			strTitle = u8"이펙트 툴" + strAdd;
			break;
		}

		case WINDOW_TYPE::IMGUI_UITOOL_WINDOW:
		{
			m_strDialogKey = "UIToolDialog";
			strTitle = u8"UI 툴" + strAdd;
			break;
		}

		case WINDOW_TYPE::IMGUI_SHADER_WINDOW:
		{
			m_strDialogKey = "ShaderToolDialog";
			strTitle = u8"셰이더 툴" + strAdd;
			break;
		}
	}
	g_pFileDialog->OpenDialog(m_strDialogKey, strTitle, szFilters, strPath, 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
}

void CImgui_Window::ShowDialog()
{
	if (g_pFileDialog->Display(m_strDialogKey))
	{
		if (g_pFileDialog->IsOk())
		{
			string filePathName = g_pFileDialog->GetFilePathName(); //! 예시 = C://Desktop//3D_TeamPortpolio//Client//Bin//Resources//DataFiles//예시.json
			string filePath =	  g_pFileDialog->GetCurrentPath();	//! 예시 = C://Desktop//3D_TeamPortpolio//Client//Bin//Resources//DataFiles//
			string fileName =	  g_pFileDialog->GetCurrentFileName(); //!예시 = 예시.json

			string userDatas;

			if(g_pFileDialog->GetUserDatas())
				userDatas = string((const char*)g_pFileDialog->GetUserDatas());

			auto selection = g_pFileDialog->GetSelection();

			if (m_eDialogType == CImgui_Window::SAVE_DIALOG)
				Save_Function(filePath, fileName);
			else if(m_eDialogType == CImgui_Window::LOAD_DIALOG)
				Load_Function(filePath, fileName);
			else
				MSG_BOX("m_eDialogType 셋팅 하자");
		}
		// SH_Add : Save Load시 키 입력 방지용 bool변수 추가...
		m_bShortOff = false;
		g_pFileDialog->Close();
	}
}

HRESULT CImgui_Window::Save_Function(string strPath, string strFileName)
{

	return S_OK;
}

HRESULT CImgui_Window::Load_Function(string strPath, string strFileName)
{
	return S_OK;
}

void CImgui_Window::Set_Guizmo(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return;

	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	_float* arrView = m_arrView;
	_float* arrProj = m_arrProj;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	XMFLOAT4X4 matWorld = pGameObject->Get_Transform()->Get_WorldFloat4x4();
	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
						  matWorld._41,matWorld._42,matWorld._43,matWorld._44 };

	
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::DragFloat3("Tr", matrixTranslation);
	ImGui::DragFloat3("Rt", matrixRotation);
	ImGui::DragFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);

	//무기 위치 회전 크기 조절하기위한 변수 저장
	m_fmatrixTranslation = { matrixTranslation[0],matrixTranslation[1] ,matrixTranslation[2] };
	m_fmatrixRotation = { matrixRotation[0],matrixRotation[1] ,matrixRotation[2] };
	m_fmatrixScale = { matrixScale[0],matrixScale[1] ,matrixScale[2] };

	ImGui::Checkbox("UseSnap", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation)
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

	ImGuizmo::Manipulate(arrView, arrProj, mCurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnap ? &snap[0] : NULL);

	XMFLOAT4X4 matW = { arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3],
				arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7],
				arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11],
				arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15] };
		

	if (matrixRotation[0] > 0)
	{
		_int i = 0;
	}

	pGameObject->Get_Transform()->Set_WorldMatrix(matW);


	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}
}

void CImgui_Window::Set_GuizmoUI(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return;

	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(true); // true 변경
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);


	//if (ImGui::IsKeyPressed(ImGuiKey_T))
	//	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	//if (ImGui::IsKeyPressed(ImGuiKey_R))
	//	mCurrentGizmoOperation = ImGuizmo::ROTATE;
	//if (ImGui::IsKeyPressed(ImGuiKey_E))
	//	mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	_float* arrView = m_arrView;

	XMFLOAT4X4 matWorld = pGameObject->Get_Transform()->Get_WorldFloat4x4();

	/* 위치 이동 */
	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
						  matWorld._41,matWorld._42,matWorld._43,matWorld._44 };

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::DragFloat3("Tr", matrixTranslation);
	ImGui::DragFloat3("Rt", matrixRotation);
	ImGui::DragFloat3("Sc", matrixScale);
 	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);

	ImGui::Checkbox("UseSnap", &useSnap);
	//ImGui::SameLine();

	switch (mCurrentGizmoOperation)
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
		arrWorld == nullptr)
		return;

	ImGuizmo::Manipulate(arrView, m_arrOrthoProj, mCurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnapUI ? &snap[0] : NULL);


	XMFLOAT4X4 matW = { arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3],
				arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7],
				arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11],
				arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15] };

	CUI* pUI = dynamic_cast<CUI*>(pGameObject);
	if (nullptr == pUI)
		return;


	pUI->Get_Transform()->Set_WorldMatrix(matW);

	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}
}

void CImgui_Window::Set_GuizmoCamView()
{
	_float4x4 matCamView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float	  arrView[] = { matCamView._11,matCamView._12,matCamView._13,matCamView._14,
						  matCamView._21,matCamView._22,matCamView._23,matCamView._24,
						  matCamView._31,matCamView._32,matCamView._33,matCamView._34,
						  matCamView._41,matCamView._42,matCamView._43,matCamView._44 };

	memcpy(m_arrView, &arrView, sizeof(arrView));
}

void CImgui_Window::Set_GuizmoCamProj()
{
	_float4x4 matCamProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	_float	  arrProj[] = { matCamProj._11,matCamProj._12,matCamProj._13,matCamProj._14,
						  matCamProj._21,matCamProj._22,matCamProj._23,matCamProj._24,
						  matCamProj._31,matCamProj._32,matCamProj._33,matCamProj._34,
						  matCamProj._41,matCamProj._42,matCamProj._43,matCamProj._44 };
	memcpy(m_arrProj, &arrProj, sizeof(arrProj));
}

void CImgui_Window::Set_GuizmoOrthographicLH()
{
	_float4x4 matCamProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4x4(&matCamProj, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	_float	  arrProj[] = { matCamProj._11,matCamProj._12,matCamProj._13,matCamProj._14,
						  matCamProj._21,matCamProj._22,matCamProj._23,matCamProj._24,
						  matCamProj._31,matCamProj._32,matCamProj._33,matCamProj._34,
						  matCamProj._41,matCamProj._42,matCamProj._43,matCamProj._44 };
	memcpy(m_arrOrthoProj, &arrProj, sizeof(arrProj));
}

_bool CImgui_Window::ImGui_MouseInCheck()
{
	POINT tMouse;

	GetCursorPos(&tMouse);
	ScreenToClient(m_pGameInstance->Get_GraphicDesc()->hWnd, &tMouse);


	ImVec2 windowPos = ImGui::GetWindowPos(); //왼쪽상단모서리점
	ImVec2 windowSize = ImGui::GetWindowSize();

	if (tMouse.x >= windowPos.x && tMouse.x <= windowPos.x + windowSize.x &&
		tMouse.y >= windowPos.y && tMouse.y <= windowPos.y + windowSize.y)
	{
		return false; //ImGui 영역 내
	}

	return true;
}



HRESULT CImgui_Window::Begin()
{
	ImGui::PushStyleColor(ImGuiCol_PopupBg, m_tImGuiDESC.vBackgroundColor);


	
	//ImGui::SetNextWindowSize(m_tImGuiDESC.vWindowSize, 0);

	if (!(ImGui::Begin(m_tImGuiDESC.strName.c_str(), 0, m_tImGuiDESC.eWindowFlags)))
		return S_OK;
	
	/*if (ImGui::IsWindowFocused())
	{
		m_BackgroundColor.w += m_fTimeDelta;
		m_BackgroundColor.w = min(1.f, m_BackgroundColor.w);
	}

	else
	{
		m_BackgroundColor.w -= m_fTimeDelta;
		m_BackgroundColor.w = max(0.f, m_BackgroundColor.w);
	}*/

	return S_OK;
}

HRESULT CImgui_Window::End()
{
	ImGui::PopStyleColor();

	ImGui::End();

	return S_OK;
}

void CImgui_Window::Load_CustomStyle()
{
	json json_in = { nullptr };
	char filePath[MAX_PATH] = "../Bin/DataFiles/ImGui_Style_Custom.json";

	if (FAILED(CJson_Utility::Load_Json(filePath, json_in)))
		return;

	_float		vBgColor[4] = { 0.f, 0.f, 0.f, 1.f };

	_float fWindowRounding = { 0.f };
	_float fFrameRounding = { 0.f };
	_float fPopupRounding = { 0.f };
	_float fGrabRounding = { 0.f };
	_float fTabRounding = { 0.f };

	fTabRounding = json_in["TabRounding"];
	fGrabRounding = json_in["GrabRounding"];
	fPopupRounding = json_in["PopupRounding"];
	fFrameRounding = json_in["FrameRounding"];
	fWindowRounding = json_in["WindowRounding"];

	vBgColor[0] = json_in["vBgColor.x"];
	vBgColor[1] = json_in["vBgColor.y"];
	vBgColor[2] = json_in["vBgColor.z"];
	vBgColor[3] = json_in["vBgColor.w"];

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	ImVec4 bgColor = ImVec4(vBgColor[0], vBgColor[1], vBgColor[2], vBgColor[3]);

	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_MenuBarBg] = bgColor;
	colors[ImGuiCol_Tab] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_ScrollbarBg] = bgColor;

	style.WindowRounding = fWindowRounding;
	style.FrameRounding = fFrameRounding;
	style.PopupRounding = fPopupRounding;
	style.GrabRounding = fGrabRounding;
	style.TabRounding = fTabRounding;

}

void CImgui_Window::Free()
{

	Safe_Delete_Array(m_arrView);
	Safe_Delete_Array(m_arrProj);
	Safe_Delete_Array(m_arrOrthoProj);

	g_pFileDialog->Close();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}