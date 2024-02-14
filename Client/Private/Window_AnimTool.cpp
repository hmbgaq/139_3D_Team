#include "stdafx.h"
#include "Window_AnimTool.h"
#include "GameInstance.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ImGuizmo/ImSequencer.h"
#include "ImGuizmo/ImZoomSlider.h"
#include "ImGuizmo/ImCurveEdit.h"
#include "ImGuizmo/GraphEditor.h"
#include "CustomDialogFont.h"
#include "Model.h"
#include "PreviewAnimationModel.h"


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
			ImGui::Text(u8"플레이어");

			//Draw_Player();
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

void CWindow_AnimTool::Create_Object(const wstring& strLayerTag, const wstring& strPrototypeTag)
{
	m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, strPrototypeTag);

	list<CGameObject*> pGameObjects = *(m_pGameInstance->Get_GameObjects(LEVEL_TOOL, strLayerTag));
	CGameObject* pGameObject = pGameObjects.back();

	const _float3& temp = _float3(0.0f, 0.0f, 5.0f);
	pGameObject->Set_Position(temp);
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
	//if (!m_pCurrentAnimation)
	//	return;
	
	if (ImGui::TreeNode("AnimationModel"))
	{
		string items[] = { "Layer_Player", "Layer_Monster","Layer_Environment","Layer_Object","Layer_Effect","Layer_Something"};

		static int Object_idx = 0; // Here we store our selection data as an index.
		static int Layer_idx = 0; // Here we store our selection data as an index.
		int ObjectTagSize = m_vObjectTag.size();

		if (ImGui::BeginListBox("ObjectList"))
		{
			for (int n = 0; n < ObjectTagSize; n++)
			{
				const bool is_selected = (Object_idx == n);
				if (ImGui::Selectable(m_vObjectTag[n].c_str(), is_selected))
					Object_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndListBox();
		}
		ImGui::Spacing();
		if (ImGui::BeginListBox("LayerList"))
		{
			for (int n = 0; n <6; n++)
			{
				const bool is_selected = (Layer_idx == n);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
					Layer_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					if (m_bCreateCheck)
						if (m_pGameInstance->Mouse_Down(DIM_LB))
						{
							Create_Object(ConvertCtoWC(items[Layer_idx].c_str()), ConvertCtoWC(m_vObjectTag[Object_idx].c_str()));
							//Create_Object_On_Map(ConvertCtoWC(items[Layer_idx].c_str()), ConvertCtoWC(m_vObjectTag[Object_idx].c_str()));
							m_bCloneCount = true;
							m_bListCheck = true;
						}
					
				}

			}
			
			ImGui::EndListBox();
		}
		ImGui::Spacing();
		ImGui::Checkbox("Create",&m_bCreateCheck);
		ImGui::SameLine();
		ImGui::Checkbox("Delete",& m_bDeleteCheck);
		

		
		static int CreateIndex = 0; // Here we store our selection data as an index.
		
		if (m_bListCheck)
		{
			m_iCreateObjectSize =m_CreateList.size();
			if (ImGui::BeginListBox("CreateList"))
			{

				for (int n = 0; n < m_iCreateObjectSize; n++)
				{
					string str ="Object";
					string str2 = to_string(n);

					const bool is_selected = (CreateIndex == n);
					if (ImGui::Selectable((str + "." + str2).c_str(), is_selected))
						CreateIndex = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						m_PickingObject = m_CreateList[CreateIndex];
						ImGui::SetItemDefaultFocus();
						if (m_bDeleteCheck)
						{
							_bool isdead = true;
							m_CreateList[CreateIndex]->Set_Dead(isdead);
							m_CreateList.erase(m_CreateList.begin() + CreateIndex);
							m_bDeleteCheck = false;
						}
							
					}
				}
				ImGui::EndListBox();
			}
		}
		ImGui::TreePop();
		CCharacter* pcharacter = dynamic_cast<CCharacter*>(m_CreateList.back());
		if (ImGui::BeginListBox("AnimationList"))
		{
			for (int n = 0; n < 6; n++)
			{
				const bool is_selected = (Layer_idx == n);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
					Layer_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					if (m_bCreateCheck)
						if (m_pGameInstance->Mouse_Down(DIM_LB))
						{
							Create_Object(ConvertCtoWC(items[Layer_idx].c_str()), ConvertCtoWC(m_vObjectTag[Object_idx].c_str()));
							m_bCloneCount = true;
							m_bListCheck = true;
						}

				}

			}

			ImGui::EndListBox();
		}
	}

	if (ImGui::Button("Play"))
	{
		m_bStop = !m_bStop;
	}

	ImGui::SameLine();

	if (ImGui::Button("Hold"))
	{
		m_bHold = !m_bHold;

// 		_byte byFlag(0);
// 
// 		CModel* pCurrentModel = m_pPreViewModel->Get_CurrentModel();
// 
// 		if (m_bHold)
// 		{
// 			byFlag = (_byte)ROOTNODE_FLAG::X | (_byte)ROOTNODE_FLAG::Z;
// 		}


	}
}

char* CWindow_AnimTool::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	char* result = new char[size_needed];

	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);

	return result;
}

char* CWindow_AnimTool::ConvertWCtoC(const wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;
	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];
	//형 변환
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}

wchar_t* CWindow_AnimTool::ConvertCtoWC(const char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);
	return pStr;
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