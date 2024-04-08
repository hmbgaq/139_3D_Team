#include "stdafx.h"
#include "Camera_Dynamic.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "Character.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
	: CCamera(pDevice, pContext, strPrototypeTag)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void * pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	DYNAMIC_CAMERA_DESC*	pDesc = (DYNAMIC_CAMERA_DESC*)pArg;

	m_fMouseSensor = pDesc->fMouseSensor;

	FAILED_CHECK(__super::Initialize(pDesc));

	//CData_Manager::GetInstance()->Set_MasterCamera();
	
	return S_OK;
}

void CCamera_Dynamic::Priority_Tick(_float fTimeDelta)
{
	//CData_Manager::GetInstance()->Reset_Camera_Dynamic((LEVEL)m_pGameInstance->Get_NextLevel());
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_LEFTARROW))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_RIGHTARROW))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_UPARROW))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_DOWNARROW))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		_long	MouseMove = 0;

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * MouseMove * fTimeDelta);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * MouseMove * fTimeDelta);
		}
	}

		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if (m_pGameInstance->Key_Down(DIK_F2))
				CData_Manager::GetInstance()->Get_MasterCamera()->Set_CameraType(CMasterCamera::SpringCamera);
		}

		// 병합할 때 주석 풀기
		if (m_pGameInstance->Key_Down(DIK_NUMPAD9))
		{
			m_pGameInstance->Get_Player()->Set_InitPosition(m_pTransformCom->Get_Position());
		}

	__super::Tick(fTimeDelta);
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Dynamic::Ready_Components()
{
	return S_OK;
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Dynamic::Pool()
{
	return new CCamera_Dynamic(*this);
}


void CCamera_Dynamic::Free()
{
	__super::Free();
}

