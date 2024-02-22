#include "stdafx.h"
#include "Window_Style.h"


CWindow_Style::CWindow_Style(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CWindow_Style::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	Load_CustomStyle_ForMember();

	return S_OK;
}

void CWindow_Style::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	ImGui::ColorEdit4("BG Color", m_vBgColor, ImGuiColorEditFlags_None);
	
	m_tStyleDesc.vBgColor.x = m_vBgColor[0];
	m_tStyleDesc.vBgColor.y = m_vBgColor[1];
	m_tStyleDesc.vBgColor.z = m_vBgColor[2];
	m_tStyleDesc.vBgColor.w = m_vBgColor[3];

	colors[ImGuiCol_WindowBg]	 = m_tStyleDesc.vBgColor;
	colors[ImGuiCol_TitleBg]	 = m_tStyleDesc.vBgColor;
	colors[ImGuiCol_MenuBarBg]	 = m_tStyleDesc.vBgColor;
	colors[ImGuiCol_Tab]		 = m_tStyleDesc.vBgColor;
	colors[ImGuiCol_ChildBg]	 = m_tStyleDesc.vBgColor;
	colors[ImGuiCol_ScrollbarBg] = m_tStyleDesc.vBgColor;

	if (ImGui::DragFloat(" WindowRounding ", &m_tStyleDesc.fWindowRounding, 0.5f, 0.f, 20.f))
	{
		style.WindowRounding = m_tStyleDesc.fWindowRounding;
	}

	if (ImGui::DragFloat(" FrameRounding ", &m_tStyleDesc.fFrameRounding, 0.5f, 0.f, 20.f))
	{
		style.FrameRounding = m_tStyleDesc.fFrameRounding;
	}

	if (ImGui::DragFloat(" PopupRounding ", &m_tStyleDesc.fPopupRounding, 0.5f, 0.f, 20.f))
	{
		style.PopupRounding = m_tStyleDesc.fPopupRounding;
	}

	if (ImGui::DragFloat(" GrabRounding ", &m_tStyleDesc.fGrabRounding, 0.5f, 0.f, 20.f))
	{
		style.GrabRounding = m_tStyleDesc.fGrabRounding;
	}

	if (ImGui::DragFloat(" TabRounding ", &m_tStyleDesc.fTabRounding, 0.5f, 0.f, 20.f))
	{
		style.TabRounding = m_tStyleDesc.fTabRounding;
	}


	if (ImGui::Button(" SAVE ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
	{
		char filePath[MAX_PATH] = "../Bin/DataFiles/ImGui_Style_Custom.json";

		json Out_Json;

		Out_Json["TabRounding"] = m_tStyleDesc.fTabRounding;
		Out_Json["GrabRounding"] = m_tStyleDesc.fGrabRounding;
		Out_Json["PopupRounding"] = m_tStyleDesc.fPopupRounding;
		Out_Json["FrameRounding"] = m_tStyleDesc.fFrameRounding;
		Out_Json["WindowRounding"] = m_tStyleDesc.fWindowRounding;
		Out_Json["vBgColor.x"] = m_vBgColor[0];
		Out_Json["vBgColor.y"] = m_vBgColor[1];
		Out_Json["vBgColor.z"] = m_vBgColor[2];
		Out_Json["vBgColor.w"] = m_vBgColor[3];

		CJson_Utility::Save_Json(filePath, Out_Json);
	}


	if (ImGui::Button(" LOAD ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
	{
		json json_in;
		char filePath[MAX_PATH] = "../Bin/DataFiles/ImGui_Style_Custom.json";

		CJson_Utility::Load_Json(filePath, json_in);

		m_tStyleDesc.fTabRounding		= json_in["TabRounding"];
		m_tStyleDesc.fGrabRounding		= json_in["GrabRounding"];
		m_tStyleDesc.fPopupRounding		= json_in["PopupRounding"];
		m_tStyleDesc.fFrameRounding		= json_in["FrameRounding"];
		m_tStyleDesc.fWindowRounding	= json_in["WindowRounding"];

		m_vBgColor[0] = json_in["vBgColor.x"];
		m_vBgColor[1] = json_in["vBgColor.y"];
		m_vBgColor[2] = json_in["vBgColor.z"];
		m_vBgColor[3] = json_in["vBgColor.w"];

	}

	__super::End();

}

void CWindow_Style::Render()
{

}

void CWindow_Style::Load_CustomStyle_ForMember()
{
	json json_in = { nullptr };
	char filePath[MAX_PATH] = "../Bin/DataFiles/ImGui_Style_Custom.json";

	if (FAILED(CJson_Utility::Load_Json(filePath, json_in)))
		return;

	m_tStyleDesc.fTabRounding = json_in["TabRounding"];
	m_tStyleDesc.fGrabRounding = json_in["GrabRounding"];
	m_tStyleDesc.fPopupRounding = json_in["PopupRounding"];
	m_tStyleDesc.fFrameRounding = json_in["FrameRounding"];
	m_tStyleDesc.fWindowRounding = json_in["WindowRounding"];

	m_vBgColor[0] = json_in["vBgColor.x"];
	m_vBgColor[1] = json_in["vBgColor.y"];
	m_vBgColor[2] = json_in["vBgColor.z"];
	m_vBgColor[3] = json_in["vBgColor.w"];

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	ImVec4 bgColor = ImVec4(m_vBgColor[0], m_vBgColor[1], m_vBgColor[2], m_vBgColor[3]);

	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_MenuBarBg] = bgColor;
	colors[ImGuiCol_Tab] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_ScrollbarBg] = bgColor;

	style.WindowRounding = m_tStyleDesc.fWindowRounding;
	style.FrameRounding = m_tStyleDesc.fFrameRounding;
	style.PopupRounding = m_tStyleDesc.fPopupRounding;
	style.GrabRounding = m_tStyleDesc.fGrabRounding;
	style.TabRounding = m_tStyleDesc.fTabRounding;
}


CWindow_Style* CWindow_Style::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_Style* pInstance = new CWindow_Style(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_Style");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Style::Free()
{
	__super::Free();



	Safe_Release(m_pGameInstance);
}