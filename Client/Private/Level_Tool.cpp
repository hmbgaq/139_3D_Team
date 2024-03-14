#include "stdafx.h"
#include "Level_Tool.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Data_Manager.h"

#include "Camera_Dynamic.h"
#include "MasterCamera.h"
#include "Sky.h"
#include "Navigation.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	//Level_Tool 레벨 조정 
	m_pGameInstance->Set_CurrentLevel(m_pGameInstance->Get_NextLevel());
	FAILED_CHECK(Ready_LightDesc());
	FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));
	FAILED_CHECK(Ready_Layer_Camera(TEXT("Layer_Camera")));
	{	//!내비게이션 
		CNavigation* pNavi = { nullptr };

		pNavi = dynamic_cast<CNavigation*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Navigation")));

		if (nullptr == pNavi)
			return E_FAIL;

		CData_Manager::GetInstance()->Set_Navigation(pNavi);
	}

	if (FAILED(Ready_Imgui()))
	{
		Safe_Release(m_pDevice);
		Safe_Release(m_pContext);
		return E_FAIL;
	}

	m_pGameInstance->Get_Renderer()->Render_UI_MRT(false);


	
	
	return S_OK;

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

	NULL_CHECK_RETURN(m_pImguiManager, E_FAIL);

	m_pImguiManager->AddRef();

	FAILED_CHECK(m_pImguiManager->Initialize(m_pDevice,m_pContext));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const wstring& strLayerTag)
{

	CGameObject* pSkybox = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Sky"));

	if(nullptr == pSkybox)
		return E_FAIL;

	CData_Manager::GetInstance()->Set_pSkybox(dynamic_cast<CSky*>(pSkybox));

	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Sky")));



	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& strLayerTag)
{
// 	CCamera_Dynamic::DYNAMIC_CAMERA_DESC		tDesc = {};
// 	tDesc.fMouseSensor = 0.05f;
// 	tDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
// 	tDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
// 	tDesc.fFovy = XMConvertToRadians(60.0f);
// 	tDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
// 	tDesc.fNear = 0.1f;
// 	tDesc.fFar = m_pGameInstance->Get_CamFar();
// 	tDesc.fSpeedPerSec = 60.f;
// 	tDesc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_NextLevel(), strLayerTag, TEXT("Prototype_GameObject_MasterCamera"))))
		return E_FAIL;

	CData_Manager::GetInstance()->Get_MasterCamera()->Set_CameraType(CMasterCamera::DynamicCamera);
	//if(pDynamicCam == nullptr)
	//	return E_FAIL;
	//
	//pDynamicCam->Set_Enable(true);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_LightDesc()
{
	LIGHT_DESC			LightDesc{};
	LightDesc.eType			= LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection	= _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse		= _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient		= _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular		= _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, TempLightNumber)))
		return E_FAIL;

	cout << "Level Tool - Light Number = " << TempLightNumber << endl;

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
