#include "stdafx.h"
#include "Window_MapTool.h"

CWindow_MapTool::CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_MapTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if(FAILED(__super::Initialize()))
		return E_FAIL;


	return S_OK;
}

void CWindow_MapTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	ImGui::Text("Test");

	static float col1[3] = { 1.0f, 0.0f, 0.2f };
	ImGui::ColorEdit3("color 1", col1, ImGuiColorEditFlags_None);
	ImGui::SameLine();

	__super::End();
}

void CWindow_MapTool::Render()
{

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
}