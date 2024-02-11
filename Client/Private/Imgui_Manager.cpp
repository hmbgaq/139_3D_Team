#include "stdafx.h"
#include "Imgui_Manager.h"
#include "Imgui_Manager.h"

CImgui_Manager::CImgui_Manager()
{
}

#pragma region 이니셜라이즈

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	//m_pDevice = pDevice;
	//m_pContext = pContext;

	return S_OK;
}

void CImgui_Manager::ImGuizmo_Initialize()
{
}

void CImgui_Manager::ImGuiFileDialog_Initialize()
{
}

#pragma endregion 이니셜라이즈

void CImgui_Manager::Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Render()
{
}

void CImgui_Manager::Free()
{
}
