#include "stdafx.h"
#include "Event_UITrigger.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Window_MapTool.h"
#include "UI_Manager.h"

CEvent_UITrigger::CEvent_UITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEvent_Trigger(pDevice, pContext)
{
}

HRESULT CEvent_UITrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	m_tUITriggerDesc = *(CEvent_UITrigger::UI_TRIGGERDESC*)pArg;


	Set_ColliderSize(m_tUITriggerDesc.vColliderSize);
	Set_ColliderCenter(m_tUITriggerDesc.vColliderCenter);

	return S_OK;
}

void CEvent_UITrigger::Activate()
{
	if (m_tUITriggerDesc.iSpawnGroupIndex == 0)
	{// 게임시작 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::START); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	if (m_tUITriggerDesc.iSpawnGroupIndex == 1)
	{// Crane 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::CRANE); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	else if (m_tUITriggerDesc.iSpawnGroupIndex == 2)
	{// 좀비 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::ZOMBIE); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	else if (m_tUITriggerDesc.iSpawnGroupIndex == 3)
	{// Jenu거인 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::JENU); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	//else if (m_tUITriggerDesc.iSpawnGroupIndex == 4)
	//{// 좀비무리 설명
	//	m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::ZOMBIE_OVER); // 메세지 변경
	//	m_pUIManager->Active_TutorialBox(); // Active 활성화
	//}
	else if (m_tUITriggerDesc.iSpawnGroupIndex == 4)
	{// 스나이퍼 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::SNIPER); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	else if (m_tUITriggerDesc.iSpawnGroupIndex == 5)
	{// 설산 시작 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::SNOW_START); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	else if (m_tUITriggerDesc.iSpawnGroupIndex == 6)
	{// 탱커 설명
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::TANK); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}
	else if (m_tUITriggerDesc.iSpawnGroupIndex == 7)
	{// 기술 습득
		m_pUIManager->Change_TutorialText(TUTORIAL_TEXT::REWARD); // 메세지 변경
		m_pUIManager->Active_TutorialBox(); // Active 활성화
	}

	m_tTriggerDesc.bOnTrigger = true;
}

_bool CEvent_UITrigger::Activate_Condition()
{
	if (m_bForceActivate == true)
		return true;

	_bool bSuperCondition = __super::Activate_Condition();

	if (bSuperCondition == true)
		return true;

	return false;
}

_bool CEvent_UITrigger::End_Condition()
{
	if (m_tTriggerDesc.bOnTrigger == true)
		return true;
	else
		return false;
}

_bool CEvent_UITrigger::Write_Json(json& Out_Json)
{
	json Temp_Json = Out_Json;

	__super::Write_Json(Out_Json);

	return false;
}

void CEvent_UITrigger::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}

HRESULT CEvent_UITrigger::Ready_Component()
{

	return S_OK;
}

CEvent_UITrigger* CEvent_UITrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CEvent_UITrigger* pInstance = new CEvent_UITrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEvent_UITrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_UITrigger::Set_ColliderSize(_float3 vColliderSize)
{
	__super::Set_ColliderSize(vColliderSize);

	m_tUITriggerDesc.vColliderSize = vColliderSize;

}

void CEvent_UITrigger::Set_ColliderCenter(_float3 vColliderCenter)
{
	__super::Set_ColliderCenter(vColliderCenter);

	m_tUITriggerDesc.vColliderCenter = vColliderCenter;
}

void CEvent_UITrigger::Free()
{
	__super::Free();
}
