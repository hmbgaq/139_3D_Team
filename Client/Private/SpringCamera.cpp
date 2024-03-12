#include "stdafx.h"
#include "SpringCamera.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Player.h"
#include "MasterCamera.h"
#include "Bone.h"

CSpringCamera::CSpringCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CCamera(pDevice, pContext, strPrototypeTag)
{
}

CSpringCamera::CSpringCamera(const CSpringCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CSpringCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpringCamera::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	SPRING_CAMERA_DESC* pDesc = (SPRING_CAMERA_DESC*)pArg;

	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	{
		DampConstant = 2.f * sqrt(SpringConstant);

		hDist = 0.7f; //Z 축 카메라와 플레이어 거리
		vDist = 0.7f; //Y 축 카메라와 플레이어 거리
		//인게임에서 이제 최적의 포지션값인거같음 
		m_CameraOffset.x = 1.f;
		m_CameraOffset.y = 0.5f;
		m_CameraOffset.z = -3.0f;
		
// 		_uint iCurrentLevel = m_pGameInstance->Get_NextLevel();
// 		
// 		if (iCurrentLevel != (_uint)LEVEL_TOOL)
// 		{
// 			m_pPlayer = CData_Manager::GetInstance()->Get_Player();
// 			m_ptarget = m_pPlayer->Get_Transform();
// 		}

		ActualPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	}
	_float4x4 BoneMatrix = {};
	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();

	BoneMatrix = pPlayer->Get_Body()->Get_BonePtr("Spine2")->Get_CombinedTransformationMatrix();
	_float4x4 pPlayerPos = pPlayer->Get_Transform()->Get_WorldMatrix();
	_float4x4 temp = {};
	XMStoreFloat4x4(&temp, BoneMatrix * pPlayerPos);
	m_TargetPosition.x = temp._41;
	m_TargetPosition.y = temp._42;
	m_TargetPosition.z = temp._43;
	
	//m_pCharacter = m_pGameInstance->Get


	if (m_pGameInstance->Get_NextLevel() == (_uint)LEVEL_TOOL)
		ShowCursor(true);
	else 
		ShowCursor(false);

	return S_OK;
}

void CSpringCamera::Priority_Tick(_float fTimeDelta)
{
	
}

void CSpringCamera::Tick(_float fTimeDelta)
{


	//CCharacter* m_pTargetCharacter = m_pPlayer->Get_Target();
	//if (nullptr != m_pTargetCharacter && nullptr != m_pTargetCharacter->Get_TransformComp())
	//{
	//	hDist = 0.5f;
	//	vDist = 0.5f;

	//	_vector vTargetPos = XMLoadFloat4(&m_pTargetCharacter->Get_Pos_Float4());
	//	m_pTransformCom->Look_At(vTargetPos);

	//	//m_fAngle = m_ptarget->Calc_Radian(vTargetPos);

	//	Lock_On(fTimeDelta);

	//}
	//else 
	//{

		/*if (0.7 - fTimeDelta > hDist)
		{
			hDist += fTimeDelta;
		}
		else
		{
			hDist = 0.7f;
		}

		if (0.7 - fTimeDelta > vDist)
		{
			vDist += fTimeDelta;
		}
		else
		{
			vDist = 0.7f;
		}*/

		//m_pTransformCom->Look_At(m_ptarget->Get_State(CTransform::STATE::STATE_POSITION));
		//CameraRotation(fTimeDelta);
		//
		////Player가 앞키를 누르면 카메라 회전했던 방향쪽에서 회전값을 받아서 카메라가 바라보고 있는 방향으로 플레이어도 쳐다 보게 만듬 
		//if (true == m_pPlayer->Is_Rotate_In_CameraDir())
		//{
		//	RotatePlayer();
		//}
	//}
		if(true == m_pPlayer->m_bPlayerCheck)
		{// 뼈에 붙인 카메라 
			_float4x4 BoneMatrix = {};
			CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();

			BoneMatrix = pPlayer->Get_Body()->Get_BonePtr("Spine2")->Get_CombinedTransformationMatrix();
			_float4x4 pPlayerPos = pPlayer->Get_Transform()->Get_WorldMatrix();
			_float4x4 temp = {};
			XMStoreFloat4x4(&temp, BoneMatrix * pPlayerPos);
			m_TargetPosition.x = temp._41;
			m_TargetPosition.y = temp._42;
			m_TargetPosition.z = temp._43;
		}
		else
		{
			// 뼈에 붙인 카메라 TEST
			_float4x4 BoneMatrix = {};
			
			BoneMatrix = m_pCharacter->Get_Body()->Get_BonePtr("Spine2")->Get_CombinedTransformationMatrix();
			_float4x4 pCharacterPos = m_pCharacter->Get_Transform()->Get_WorldMatrix();
			_float4x4 temp = {};
			XMStoreFloat4x4(&temp, BoneMatrix * pCharacterPos);
			m_TargetPosition.x = temp._41;
			m_TargetPosition.y = temp._42;
			m_TargetPosition.z = temp._43;
			
		}
		

		m_pTransformCom->Look_At(m_ptarget->Get_State(CTransform::STATE::STATE_POSITION));
		CameraRotation(fTimeDelta);

		//Player가 앞키를 누르면 카메라 회전했던 방향쪽에서 회전값을 받아서 카메라가 바라보고 있는 방향으로 플레이어도 쳐다 보게 만듬 
		if (true == m_pPlayer->Is_Rotate_In_CameraDir() && true == m_pPlayer->m_bPlayerCheck)
		{
			RotatePlayer();
		}
		else
		{
			int i = 0;
		}

		if (m_pGameInstance->Key_Down(DIK_TAB))
		{
			if (m_bFix)
			{	
				m_bFix = false;
				m_bCheck = false;
			}
			else
			{
				m_bFix = true;
				m_bCheck = true;
			}
		}

		
		if(m_pGameInstance->Key_Down(DIK_F2))
			CData_Manager::GetInstance()->Get_MasterCamera()->Set_CameraType(CMasterCamera::DynamicCamera);

		_uint iCurrentLevel = m_pGameInstance->Get_NextLevel();
		if (iCurrentLevel != (_uint)LEVEL_TOOL)
		{
			if (m_bCheck == false)
				ShowCursor(FALSE);
			else
				ShowCursor(TRUE);
		}

		if (false == m_bFix)
			return;
		if (true == m_bFix)
		{
			Mouse_Fix();
		}

		__super::Tick(fTimeDelta);
	


	
}

void CSpringCamera::Late_Tick(_float fTimeDelta)
{
	// 카메라가 플레이어랑 같이 Tick에서 위치 계산하고 움직임까지 넣어버리면 화면이 덜덜거림
	//그래서 움직임 코드는 Late_Tick에다가 넣어줬음! 

	//CCharacter* m_pTargetCharacter = m_pPlayer->Get_Target();
	//if (m_pTargetCharacter) return;

	_long	MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
	_long	MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

	if (MouseMoveX != 0)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensor * MouseMoveX * fTimeDelta);
	}

	if (MouseMoveY != 0)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensor * MouseMoveY * fTimeDelta);
	}

}

_bool CSpringCamera::Write_Json(json& Out_Json)
{
	//Out_Json["Name"] = m_sName;
	//__super::Write_Json(Out_Json);

	return false;
}

void CSpringCamera::CameraRotation(_float fTimeDelta)
{
	//카메라 움직임은 Late_Tick에 있다!
	_float3 currentCameraPosition = ActualPosition;
	_float3 idealPosition = m_ptarget->Get_State(CTransform::STATE_POSITION);
	idealPosition = m_TargetPosition;
	_float3 displacement = ActualPosition - idealPosition;
	_float3 SpringAccel = (-SpringConstant * displacement) - (DampConstant * Velocity);
	Velocity += SpringAccel * fTimeDelta;
	ActualPosition += Velocity * fTimeDelta;
	_long	MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
	_long	MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);
	// 캐릭터 주위를 중심으로 하는 회전을 계산
	m_fAngle += m_fMouseSensor * MouseMoveX * fTimeDelta;
	m_fPitch += m_fMouseSensor * MouseMoveY * fTimeDelta;

	//pitch 각도 제한
	m_fPitch = max(-XM_PIDIV2, min(XM_PIDIV2, m_fPitch));

	// 회전 행렬 계산
	_matrix rotationMatrix = XMMatrixRotationRollPitchYaw(m_fPitch, m_fAngle, 0.0f);

	// 카메라 위치 보간
	currentCameraPosition.x = XMVectorGetX(XMVectorLerp(XMLoadFloat3(&currentCameraPosition), XMLoadFloat3(&idealPosition), 1.0f - expf(-CameraMoveSpeed * fTimeDelta)));
	currentCameraPosition.y = XMVectorGetY(XMVectorLerp(XMLoadFloat3(&currentCameraPosition), XMLoadFloat3(&idealPosition), 1.0f - expf(-CameraMoveSpeed * fTimeDelta)));
	currentCameraPosition.z = XMVectorGetZ(XMVectorLerp(XMLoadFloat3(&currentCameraPosition), XMLoadFloat3(&idealPosition), 1.0f - expf(-CameraMoveSpeed * fTimeDelta)));

	// 캐릭터 주위를 중심으로 하는 카메라 위치 계산
	XMVECTOR cameraOffset = XMVectorSet(m_CameraOffset.x, m_CameraOffset.y, m_CameraOffset.z, 0.0f);  // 카메라의 초기 위치
	cameraOffset = XMVector3TransformCoord(cameraOffset, rotationMatrix);

	// 캐릭터의 위치 및 회전 적용
	m_pTransformCom->Set_WorldMatrix(rotationMatrix * XMMatrixTranslationFromVector(ActualPosition));
	m_pTransformCom->Set_Position(currentCameraPosition + cameraOffset * hDist);
}

void CSpringCamera::StartCameraRotation(_fmatrix StartRotationMatrix)
{
	m_pTransformCom->Set_WorldMatrix(StartRotationMatrix * XMMatrixTranslationFromVector(ActualPosition));
}

void CSpringCamera::Lock_On(_float fTimeDelta)
{
	//카메라 움직임은 Late_Tick에 있다!
	_float3 currentCameraPosition = ActualPosition;
	_float3 idealPosition = m_ptarget->Get_State(CTransform::STATE_POSITION);
	_float3 displacement = ActualPosition - idealPosition;
	_float3 SpringAccel = (-SpringConstant * displacement) - (DampConstant * Velocity);
	Velocity += SpringAccel * fTimeDelta;
	ActualPosition += Velocity * fTimeDelta;

	//pitch 각도 제한
	m_fPitch = max(-XM_PIDIV2, min(XM_PIDIV2, m_fPitch));

	// 회전 행렬 계산
	_matrix rotationMatrix = XMMatrixRotationRollPitchYaw(m_fPitch, m_fAngle, 0.0f);

	// 카메라 위치 보간
	currentCameraPosition.x = XMVectorGetX(XMVectorLerp(XMLoadFloat3(&currentCameraPosition), XMLoadFloat3(&idealPosition), 1.0f - expf(-CameraMoveSpeed * fTimeDelta)));
	currentCameraPosition.y = XMVectorGetY(XMVectorLerp(XMLoadFloat3(&currentCameraPosition), XMLoadFloat3(&idealPosition), 1.0f - expf(-CameraMoveSpeed * fTimeDelta)));
	currentCameraPosition.z = XMVectorGetZ(XMVectorLerp(XMLoadFloat3(&currentCameraPosition), XMLoadFloat3(&idealPosition), 1.0f - expf(-CameraMoveSpeed * fTimeDelta)));

	// 캐릭터 주위를 중심으로 하는 카메라 위치 계산
	XMVECTOR cameraOffset = XMVectorSet(m_CameraOffset.x, m_CameraOffset.y, m_CameraOffset.z, 0.0f);  // 카메라의 초기 위치
	cameraOffset = XMVector3TransformCoord(cameraOffset, rotationMatrix);

	// 캐릭터의 위치 및 회전 적용
	m_pTransformCom->Set_WorldMatrix(rotationMatrix * XMMatrixTranslationFromVector(ActualPosition));
	m_pTransformCom->Set_Position(currentCameraPosition + cameraOffset * hDist);

}

void CSpringCamera::RotatePlayer()
{
	//_matrix rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, m_fAngle, 0.0f);

	m_pPlayer->Set_Rotate_In_CameraDir(false);

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_ptarget->Rotation(vUp, m_fAngle);

	//m_ptarget->Set_WorldMatrix(rotationMatrix* XMMatrixTranslationFromVector(m_ptarget->Get_State(CTransform::STATE_POSITION)));
}

void CSpringCamera::Mouse_Fix()
{
	POINT   pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CSpringCamera::Set_pTargetCharacter(CCharacter* _pCharacter)
{
	m_pCharacter = _pCharacter;
}


CSpringCamera* CSpringCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CSpringCamera* pInstance = new CSpringCamera(pDevice, pContext, strPrototypeTag);
	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpringCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSpringCamera::Clone(void* pArg)
{
	CSpringCamera* pInstance = new CSpringCamera(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpringCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpringCamera::Free()
{
	__super::Free();
}

Engine::CGameObject* CSpringCamera::Pool()
{
	return new CSpringCamera(*this);
}
