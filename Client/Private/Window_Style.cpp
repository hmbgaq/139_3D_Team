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


	__super::End();

}

void CWindow_Style::Render()
{

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