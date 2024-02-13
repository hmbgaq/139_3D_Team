#include "stdafx.h"
#include "Window_AnimTool.h"
#include "GameInstance.h"
#include "../../Reference/Imgui/ImGuiFileDialog/ImGuiFileDialog.h"
#include "CustomDialogFont.h"
#include "Model.h"

CWindow_AnimTool::CWindow_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_AnimTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if(FAILED(__super::Initialize()))
		return E_FAIL;

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

	return S_OK;
}

void CWindow_AnimTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();
	
	if (ImGui::Checkbox("RenderTarget", &m_bRenderTargetOnOff))
	{
		m_pGameInstance->Set_RenderDebug(m_bRenderTargetOnOff);
	}
	//dialog========================================================================
	static bool canValidateDialog = false;
		
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png,.fbx,.dds",
			".", 1, nullptr);

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
	//===============================dialog============================================

	__super::End();
}

void CWindow_AnimTool::Render()
{
	__super::Begin();

	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Animation View", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Player"))
		{
			Draw_Player();
			Draw_AnimationList();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Monster")))
		{
			Draw_Monster();
			Draw_AnimationList();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Event Editer"))
		{
			Draw_KeyEventEditer();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	__super::End();
	
}

void CWindow_AnimTool::Call_UpdatePreViewModel()
{
}

void CWindow_AnimTool::Call_NextAnimationKey(const _uint& In_Key)
{
}

void CWindow_AnimTool::Add_EffectKeyEvent()
{
}

void CWindow_AnimTool::Add_EnableWeaponEvent(const _bool In_bEnable)
{
}

void CWindow_AnimTool::Add_SoundKeyEvent()
{
}

void CWindow_AnimTool::Add_RandomSoundKeyEvent()
{
}

void CWindow_AnimTool::Save_KeyEvent()
{
}

HRESULT CWindow_AnimTool::Load_KeyEvent()
{
	return E_NOTIMPL;
}

void CWindow_AnimTool::Clear_KeyEvent()
{
}

void CWindow_AnimTool::ClearBack_KeyEvent()
{
}

void CWindow_AnimTool::ClearAll_KeyEvent()
{
}

void CWindow_AnimTool::Clear_WeaponEvent()
{
}

void CWindow_AnimTool::Draw_Player()
{
	if (!m_pCurrentAnimation)
		return; 

}

void CWindow_AnimTool::Draw_Monster()
{
}

void CWindow_AnimTool::Draw_KeyEventEditer()
{
}

void CWindow_AnimTool::Draw_AnimationList()
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