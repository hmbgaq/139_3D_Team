#include "..\Public\MasterCamera.h"
 
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Character.h"
#include "Player.h"
#include "Data_Manager.h"
#include "Clone_Manager.h"

CMasterCamera::CMasterCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CMasterCamera::CMasterCamera(const CMasterCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMasterCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMasterCamera::Initialize(void* pArg)
{
	m_pPlayer = CData_Manager::GetInstance()->Get_Player();

	CCamera_Dynamic::DYNAMIC_CAMERA_DESC		Desc = {};

	m_iCurrnetLevel = m_pGameInstance->Get_NextLevel();
	

	Desc.fMouseSensor = 0.05f;

	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	if (m_iCurrnetLevel == (_uint)LEVEL_SNOWMOUNTAIN)
		Desc.fSpeedPerSec = 300.f;
	else 
		Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);


	if (m_pPlayer != nullptr)
	{
		_float4 vPlayerPos = m_pPlayer->Get_Transform()->Get_Position_Float4();
		_float pPlayerPosW = vPlayerPos.w;

		_float4 targetPos = { 0.f, 2.f, -5.f, pPlayerPosW };

		XMStoreFloat4(&targetPos, XMVector3TransformCoord(XMLoadFloat4(&targetPos), m_pPlayer->Get_Transform()->Get_WorldMatrix()));

		Desc.vEye = targetPos;
		Desc.vAt = vPlayerPos;
	}
	else
	{
		Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
		Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	}


	
	{
		CCamera* pCamera = CCamera_Dynamic::Create(m_pDevice,m_pContext, L"FakeCameraDynamic");
		pCamera->Initialize(&Desc);
		if (pCamera)	m_Cameras.push_back(pCamera);

		//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"), CCamera_Dynamic::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Dynamic"))));
	//FAILED_CHECK(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Spring"), CSpringCamera::Create(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Camera_Spring"))));

		//CCamera* pCamera = CClone_Manager::GetInstance()->Clone_Object<CSpringCamera>(iNextLevel, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Spring"), &Desc);
		//if (pCamera)	m_Cameras.push_back(pCamera);
		
	}

// 	if (m_pPlayer != nullptr)
// 	{
// 		CCamera* pCamera = CSpringCamera::Create(m_pDevice, m_pContext, L"FakeCameraSpring");
// 		pCamera->Initialize(&Desc);
// 		if (pCamera)	m_Cameras.push_back(pCamera);
// 		//CCamera* pCamera = CClone_Manager::GetInstance()->Clone_Object<CCamera_Dynamic>(iNextLevel, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Dynamic"), &Desc);
// 		//if (pCamera)	m_Cameras.push_back(pCamera);
// 
// 	}

	{//추가 적으로 나중에 컷신 카메라 나오면 여기에 생성 하셈 
		//CCamera* pCamera = CClone_Manager::GetInstance()->Clone_Object<CSpringCamera>(iNextLevel, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Spring"), &Desc);
		//if (pCamera)	m_Cameras.push_back(pCamera);
		////m_eCameraType = CameraType::SpringCamera;
	}
		
	


	if(m_iCurrnetLevel == (_uint)LEVEL_TOOL)
		m_eCameraType = CMasterCamera::DynamicCamera;
	else
		m_eCameraType = CameraType::SpringCamera;



	CData_Manager::GetInstance()->Set_MasterCamera(this);

	return S_OK;
}

void CMasterCamera::Priority_Tick(_float fTimeDelta)
{
	// error : UI(다른객체는 모름)Set_Dead로 삭제시 플레이어가 null값에서 이상한 값으로 들어가있음
	//return;
	// error

	if(m_pPlayer == nullptr)
		m_pPlayer = CData_Manager::GetInstance()->Get_Player();

	if (m_pPlayer != nullptr && m_bfirstCheck == true)
	{
		_uint iNextLevel = m_pGameInstance->Get_NextLevel();

		CCamera_Dynamic::DYNAMIC_CAMERA_DESC		Desc = {};

		Desc.fMouseSensor = 0.05f;

		Desc.fFovy = XMConvertToRadians(60.0f);
		Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		Desc.fNear = 0.1f;
		Desc.fFar = 1000.f;
		Desc.fSpeedPerSec = 20.f;
		Desc.fRotationPerSec = XMConvertToRadians(180.0f);


		{
			_float4 vPlayerPos = m_pPlayer->Get_Transform()->Get_Position_Float4();
			_float pPlayerPosW = vPlayerPos.w;

			//	_float4 targetPos = { 0.f, 2.f, -5.f, pPlayerPosW };

			//	XMStoreFloat4(&targetPos, XMVector3TransformCoord(XMLoadFloat4(&targetPos), m_pPlayer->Get_Transform()->Get_WorldMatrix()));

			//	Desc.vEye = targetPos;
			//	Desc.vAt = vPlayerPos;
			//}

		}
		{
			CCamera* pCamera = CSpringCamera::Create(m_pDevice, m_pContext, L"FakeCameraSpring");
			pCamera->Initialize(&Desc);
			if (pCamera)	m_Cameras.push_back(pCamera);

			CSpringCamera* pSpringCamera = dynamic_cast<CSpringCamera*>(pCamera);
			pSpringCamera->Set_pTarget(m_pPlayer->Get_Transform());
			pSpringCamera->Set_pPlayer(m_pPlayer);
			
			//pCamera->Set_T
			//CCamera* pCamera = CClone_Manager::GetInstance()->Clone_Object<CSpringCamera>(iNextLevel, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Spring"), &Desc);
			//if (pCamera)	m_Cameras.push_back(pCamera);
			m_bfirstCheck = false;
		}
	}

	if (m_Cameras[m_eCameraType])
		m_Cameras[m_eCameraType]->Priority_Tick(fTimeDelta);
}

void CMasterCamera::Tick(_float fTimeDelta)
{
	if (m_eCameraType == CMasterCamera::SpringCamera && m_pPlayer == nullptr)
		return;


	if (m_Cameras[m_eCameraType])
		m_Cameras[m_eCameraType]->Tick(fTimeDelta);
}

void CMasterCamera::Late_Tick(_float fTimeDelta)
{
	if (m_Cameras[m_eCameraType])
		m_Cameras[m_eCameraType]->Late_Tick(fTimeDelta);
}

CCamera_Dynamic* CMasterCamera::Get_DynamicCamera()
{
	return dynamic_cast<CCamera_Dynamic*>(m_Cameras[CMasterCamera::DynamicCamera]);
	
}

CSpringCamera* CMasterCamera::Get_SpringCamera()
{
	return dynamic_cast<CSpringCamera*>(m_Cameras[CMasterCamera::SpringCamera]);
}

CMasterCamera* CMasterCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CMasterCamera* pInstance = new CMasterCamera(pDevice, pContext, strPrototypeTag);
	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMasterCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMasterCamera::Clone(void* pArg)
{
	CMasterCamera* pInstance = new CMasterCamera(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMasterCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMasterCamera::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_Cameras.size(); ++i)
	{
		if (nullptr != m_Cameras[i])
			Safe_Release(m_Cameras[i]);
	}
}

CGameObject* CMasterCamera::Pool()
{
	return new CMasterCamera(*this);
}
