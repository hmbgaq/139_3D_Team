#include "stdafx.h"
#include "Window_AnimTool.h"
#include "GameInstance.h"
#include "../../Reference/Imgui/ImGuiFileDialog/ImGuiFileDialog.h"


CWindow_AnimTool::CWindow_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_AnimTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if(FAILED(__super::Initialize()))
		return E_FAIL;
 	ImGuiIO& io = ImGui::GetIO(); (void)io;
// 	ImGui::StyleColorsDark();
// 
	ImGui::StyleColorsClassic();
// 
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.5f;
	}
// 	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());
	return S_OK;
}

void CWindow_AnimTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;
	if (ImGui::Checkbox("RenderTarget", &m_bRenderTargetOnOff))
	{
		m_pGameInstance->Set_RenderDebug(m_bRenderTargetOnOff);
	}
	
	static bool canValidateDialog = false;
		
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png,.fbx,.dds",
			".", 1, nullptr, ImGuiFileDialogFlags_Modal);

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::string filter = ImGuiFileDialog::Instance()->GetCurrentFilter();
			// here convert from string because a string was passed as a userDatas, but it can be what you want
			std::string userDatas;
			if (ImGuiFileDialog::Instance()->GetUserDatas())
				userDatas = std::string((const char*)ImGuiFileDialog::Instance()->GetUserDatas());
			auto selection = ImGuiFileDialog::Instance()->GetSelection(); // multiselection

			// action
		}
		// close
		ImGuiFileDialog::Instance()->Close();
		m_bdialogCheck = false;
	}
	
	
	

	
	

	//ImGui::PushItemWidth(ImGui::GetFontSize() * -12);


	


	__super::End();
}

void CWindow_AnimTool::Render()
{
	

}


CWindow_AnimTool* CWindow_AnimTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_AnimTool* pInstance = new CWindow_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_AnimTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_AnimTool::Free()
{
	__super::Free();
}