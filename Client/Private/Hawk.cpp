#include "..\Public\Hawk.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Hawk_Eating_01.h"
#include "Hawk_Trans_StandGround_to_FlyHeavy.h"
#include "Player.h"
#include "MasterCamera.h"
#include "SpringCamera.h"

CHawk::CHawk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter_Client(pDevice, pContext, strPrototypeTag)
{
}

CHawk::CHawk(const CHawk& rhs)
	: CCharacter_Client(rhs)
{
}

HRESULT CHawk::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CHawk::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));


	m_pActor = new CActor<CHawk>(this);
	m_pActor->Set_State(new CHawk_Eating_01());

	m_fHp = 1.f;

	m_pRigidBody->Set_UseGravity(true);
	m_vPreCutSceneMatrix = m_pTransformCom->Get_WorldFloat4x4();

	CMasterCamera* pMasterCamera = CData_Manager::GetInstance()->Get_MasterCamera();
	CSpringCamera* pSpringCamera = pMasterCamera->Get_SpringCamera();

	if (pSpringCamera != nullptr)
	{
		pSpringCamera->Set_Hawk(this);
	}

	return S_OK;
}

void CHawk::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHawk::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

	if (m_bCutSceneHawk)
	{
		Spline_Move_LogicFunction(fTimeDelta);
	}


}

void CHawk::Late_Tick(_float fTimeDelta)
{
	//if (GAME_STATE::GAMEPLAY != CData_Manager::GetInstance()->Get_GameState())
	//	return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CHawk::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CHawk::Check_Frustum()
{
	m_bIsInFrustum = true;
}

void CHawk::FlyAway()
{
	if (nullptr == m_pDataManager)
	{
		m_pDataManager = CData_Manager::GetInstance();
	}

	_vector vPlayerPos = m_pDataManager->Get_Player()->Get_Position_Vector();


	m_pTransformCom->Look_At_OnLand_Back(vPlayerPos);


	m_pActor->Set_State(new CHawk_Trans_StandGround_to_FlyHeavy());

	m_pNavigationCom = nullptr;
}

void CHawk::Set_HawkCamAtPoints(vector<_float4>& vecCamAtPoints)
{
	CMasterCamera* pMasterCam = CData_Manager::GetInstance()->Get_MasterCamera();

	CSpringCamera* pSpringCam = pMasterCam->Get_SpringCamera();

	if (pSpringCam == nullptr)
		return;

	pSpringCam->Set_CamAtPoints(vecCamAtPoints);
}

void CHawk::Spline_Move_LogicFunction(const _float fTimeDelta)
{
	m_fSplineTimeAcc += (_float)fTimeDelta * m_fSplineMoveSpeed;

	if (m_vecMovePoints.size() == 1)
	{
		m_bFixMove = true;
	}

	if (m_vecMovePoints.size() != 1 && !m_bOnceMove)
	{
		m_bFixMove = false;
		m_bOnceMove = true;
	}

	if (m_fSplineTimeAcc >= 1.f)
	{
		m_fSplineTimeAcc = 0.f;


		if (m_iCurrentMovePoint + 1 >= m_vecMovePoints.size())
		{
			m_bFixMove = true;
		}
		else
		{
			++m_iCurrentMovePoint;
		}

		if (m_iCurrentMovePoint + 1 >= m_vecMovePoints.size())
		{
			m_bCutSceneHawk = false;
		}

	}

	Spline_Move_Function(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CHawk::Spline_Move_Function(const _float fTimeDelta)
{
	_vector vMovePosition;

	if (!m_bFixMove)
	{
		if (m_iCurrentMovePoint + 1 >= m_vecMovePoints.size())
			vMovePosition = XMLoadFloat4(&m_vecMovePoints[m_iCurrentMovePoint]);
		else
		{
			_vector vMovePoint1, vMovePoint2;
			vMovePoint1 = XMLoadFloat4(&m_vecMovePoints[m_iCurrentMovePoint]);
			vMovePoint2 = XMLoadFloat4(&m_vecMovePoints[m_iCurrentMovePoint + 1]);
			vMovePosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
		}
	}
	else
	{
		vMovePosition = XMLoadFloat4(&m_vecMovePoints[m_iCurrentMovePoint]);
	}

	_vector vNewLook = vMovePosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vNewLook = XMVector3Normalize(vNewLook);

	_vector vUp = { 0.f , 1.f , 0.f , 0.f };

	_vector vRight = XMVector3Cross(vUp, vNewLook);

	vUp = XMVector3Cross(vNewLook, vRight);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
}



void CHawk::Start_CutSceneHawk()
{
	if(true == m_vecMovePoints.empty())
		return;

	CMasterCamera* pMasterCam = CData_Manager::GetInstance()->Get_MasterCamera();

	CSpringCamera* pSpringCam = pMasterCam->Get_SpringCamera();

	if (pSpringCam == nullptr)
		return;

	
	m_bCutSceneHawk = true;
	pSpringCam->Set_HawkSpring(m_bCutSceneHawk);
	
}

void CHawk::Stop_CutSceneHawk()
{
	CMasterCamera* pMasterCam = CData_Manager::GetInstance()->Get_MasterCamera();

	CSpringCamera* pSpringCam = pMasterCam->Get_SpringCamera();

	if (pSpringCam == nullptr)
		return;

	m_bCutSceneHawk = false;
	pSpringCam->Set_HawkSpring(m_bCutSceneHawk);

	m_vecMovePoints.clear();
	
	
	m_fSplineTimeAcc = 0.f;
	m_iCurrentMovePoint = 0;
	m_bFixMove = false;
	m_bOnceMove = false;
	
}

HRESULT CHawk::Ready_Components()
{
	return S_OK;
}

HRESULT CHawk::Ready_PartObjects()
{

	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Hawk"), BodyDesc));

	return S_OK;
}


CHawk* CHawk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CHawk* pInstance = new CHawk(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHawk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHawk::Clone(void* pArg)
{
	CHawk* pInstance = new CHawk(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHawk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHawk::Pool()
{
	return new CHawk(*this);
}

void CHawk::Free()
{
	m_vecMovePoints.clear();

	__super::Free();
}
