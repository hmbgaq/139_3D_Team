#include "stdafx.h"
#include "Level_Tool.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"


CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{


	if (FAILED(Ready_Imgui()))
	{
		Safe_Release(m_pDevice);
		Safe_Release(m_pContext);
		return E_FAIL;
	}


}

void CLevel_Tool::Tick(_float fTimeDelta)
{

	m_pImguiManager->Tick(fTimeDelta);

}

HRESULT CLevel_Tool::Render()
{
	SetWindowText(g_hWnd, TEXT("툴 레벨입니다."));

	m_pImguiManager->Render();

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Imgui()
{
	m_pImguiManager = CImgui_Manager::GetInstance();


	if(nullptr == m_pImguiManager)
		return E_FAIL;



	if(FAILED(m_pImguiManager->Initialize(m_pDevice,m_pContext)))
		return E_FAIL;



	return S_OK;
}


CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pImguiManager);
	CImgui_Manager::GetInstance()->DestroyInstance();
}
