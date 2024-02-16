#include "stdafx.h"
#include "Window_MapTool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

#include "Environment_Instance.h"
#include "Environment_Object.h"

CWindow_MapTool::CWindow_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{
}


HRESULT CWindow_MapTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.

	FAILED_CHECK(__super::Initialize());


	//! Loader에서 푸시백 해놓은 Imgui_Manager의 모델태그 벡터를 받아오자.
	FAILED_CHECK(Ready_ModelTags());

	return S_OK;
}

void CWindow_MapTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	//TODO ImGuiTabBarFlags_
	//!	ImGuiTabBarFlags_None = 0,
	
	//!	ImGuiTabBarFlags_Reorderable = 1 << 0,   // 수동으로 탭을 끌어서 순서를 변경할 수 있음 + 새 탭이 목록 끝에 추가됨
	//!	ImGuiTabBarFlags_AutoSelectNewTabs = 1 << 1,   // 새 탭이 나타나면 자동으로 선택
	//!	ImGuiTabBarFlags_TabListPopupButton = 1 << 2,   // 탭 목록 팝업을 열려면 버튼을 비활성화하세요.
	//!	ImGuiTabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,   // 마우스 가운데 버튼을 사용하여 탭 닫기 동작(p_open != NULL로 제출됨)을 비활성화합니다. if (IsItemHovered() && IsMouseClicked(2)) *p_open = false를 사용하여 사용자 측에서 이 동작을 재현할 수 있습니다.
	//!	ImGuiTabBarFlags_NoTabListScrollingButtons = 1 << 4,   //스크롤 버튼 비활성화(피팅 정책이 ImGuiTabBarFlags_FittingPolicyScroll인 경우 적용)
	//!	ImGuiTabBarFlags_NoTooltip = 1 << 5,   //탭을 가리킬 때 도구 설명 비활성화
	//!	ImGuiTabBarFlags_FittingPolicyResizeDown = 1 << 6,   // 탭이 맞지 않는 경우 탭 크기 조정
	//!	ImGuiTabBarFlags_FittingPolicyScroll = 1 << 7,   // 탭이 맞지 않을 때 스크롤 버튼 추가
	//!	ImGuiTabBarFlags_FittingPolicyMask_ = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_FittingPolicyScroll,
	//!	ImGuiTabBarFlags_FittingPolicyDefault_ = ImGuiTabBarFlags_FittingPolicyResizeDown,

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyMask_;
	
	if (ImGui::BeginTabBar(u8"오브젝트 타입", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"그라운드"))
		{
			GroundTab_Function();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"상호작용"))
		{
			InteractTab_Function();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"환경"))
		{
			EnvironmentTab_Function();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	__super::End();
}

void CWindow_MapTool::Render()
{

}

HRESULT CWindow_MapTool::Ready_ModelTags()
{
	//! 애님태그
	vector<wstring> vecAnimTags = *CImgui_Manager::GetInstance()->Get_Anim_E_ModelTag();
	_int iAnimTagVectorSize = (_int)vecAnimTags.size();
	
	m_vecAnimModelTag.reserve(iAnimTagVectorSize);

	for (_int i = 0; i < iAnimTagVectorSize; ++i)
	{
		string strAnimTag = {};
		m_pGameInstance->WString_To_String(vecAnimTags[i], strAnimTag);
		
		m_vecAnimModelTag.push_back(strAnimTag);

	}

	//!논애님 태그
	vector<wstring> vecNonAnimTags = *CImgui_Manager::GetInstance()->Get_NonAnim_E_ModelTag();
	_int iNonAnimTagVectorSize = (_int)vecNonAnimTags.size();

	m_vecNonAnimModelTag.reserve(iNonAnimTagVectorSize);

	for (_int i = 0; i < iNonAnimTagVectorSize; ++i)
	{
		string strNonAnimTag = {};
		m_pGameInstance->WString_To_String(vecNonAnimTags[i], strNonAnimTag);

		m_vecNonAnimModelTag.push_back(strNonAnimTag);
	}

	
	return S_OK;
}

void CWindow_MapTool::GroundTab_Function()
{
}

void CWindow_MapTool::InteractTab_Function()
{
}

void CWindow_MapTool::EnvironmentTab_Function()
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