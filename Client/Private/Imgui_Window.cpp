#include "stdafx.h"
#include "Imgui_Window.h"
#include "GameInstance.h"

CImgui_Window::CImgui_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

}

HRESULT CImgui_Window::SetUp_ImGuiDESC(string _strName, const ImVec2& _vWindowSize, ImGuiWindowFlags _eWindowFlags)
{
	m_tImGuiDESC.strName = _strName;
	m_tImGuiDESC.vWindowSize = _vWindowSize;
	m_tImGuiDESC.eWindowFlags = _eWindowFlags;

	return S_OK;
}

HRESULT CImgui_Window::Initialize()
{

	return S_OK;
}

void CImgui_Window::Tick(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
}



HRESULT CImgui_Window::Begin()
{
	ImGui::PushStyleColor(ImGuiCol_PopupBg, m_BackgroundColor);

	ImGui::SetNextWindowSize(m_tImGuiDESC.vWindowSize, 0);

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

void CImgui_Window::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}