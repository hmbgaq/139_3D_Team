#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

#include "SMath.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
{
}

_bool CTransform::Write_Json(json& Out_Json)
{
	for (_int i = 0; i < (_int)STATE_END; ++i)
		CJson_Utility::Write_Float4(Out_Json["Transform"][i], XMLoadFloat4x4(&m_WorldMatrix).r[i]);

	return true;
}

void CTransform::Load_FromJson(const json& In_Json)
{
	_float4x4 WorldMatrix;
	ZeroMemory(&WorldMatrix, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["Transform"], WorldMatrix);

	Set_WorldMatrix(WorldMatrix);
}

void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);
}

_float3 CTransform::Calc_Front_Pos(_float3 vDiff)
{
	_vector vFront = XMVector3TransformCoord(vDiff, m_WorldMatrix);
	_float3 vResult;
	XMStoreFloat3(&vResult, vFront);

	return vResult;
}

HRESULT CTransform::Initialize_Prototype(_float fSpeedPerSec, _float fRotationPerSec)
{
	m_fSpeedPerSec = fSpeedPerSec;
	m_fRotationPerSec = fRotationPerSec;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

void CTransform::Move_On_Navigation(_vector vMove, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(STATE_POSITION);

	vPosition += vMove;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return; /* 슬라이딩들어갈자리 */

		_float4 vResult;
		_float3 vResult_Float3;

		XMStoreFloat4(&vResult, vPosition);
		//vResult.y -= 9.81f * m_pGameInstance->Get_TimeDelta();

		vResult_Float3 = { vResult.x, vResult.y, vResult.z };

		_float fHeight = pNavigation->Compute_Height(vResult_Float3, &m_bIsGround);

		if (true == m_bIsGround)
		{
			vResult.y = fHeight;
		}

		Set_State(STATE_POSITION, XMLoadFloat4(&vResult));
	}
	else 
	{
		Set_State(STATE_POSITION, vPosition);
	}

}

void CTransform::Move_On_Navigation_ForSliding(_vector vMove, const _float fTimeDelta, CNavigation* pNavigation)
{
	_float4 vResult = {};
	_float4 vSlidingDir = { 0.f, 0.f, 0.f, 0.f };

	_vector	vPosition = Get_State(STATE_POSITION);
	vPosition += vMove;

	if (nullptr == pNavigation)
	{
		Set_State(STATE_POSITION, vPosition);
		return;
	}


	_vector vTargetPos = vPosition;

	_vector vDir = XMVector3Normalize(vPosition - Get_State(CTransform::STATE_POSITION));
	_bool bIsMove_ForSliding = pNavigation->isMove_ForSliding(vPosition, vDir, &vSlidingDir);
	_bool bIsMove_ForSliding_NewPosition = false;

	if (true == bIsMove_ForSliding) 
	{
		//vTargetPos = vPosition;
		//XMStoreFloat4(&vResult, vPosition);
	}
	else if (XMVectorGetX(XMVector3Length(vSlidingDir)) > 0.f)
	{
		vSlidingDir = XMVector3Normalize(vSlidingDir);
		_vector vNewPosition = Get_State(CTransform::STATE_POSITION) + vSlidingDir * m_fSpeedPerSec * fTimeDelta;
		_vector vDir_NewPosition = XMVector3Normalize(vNewPosition - Get_State(CTransform::STATE_POSITION));

		bIsMove_ForSliding_NewPosition = pNavigation->isMove_ForSliding(vNewPosition, vDir_NewPosition, nullptr);
		if (true == bIsMove_ForSliding_NewPosition) 
		{
			vTargetPos = vNewPosition;
		}

	}

	if (true == bIsMove_ForSliding || true == bIsMove_ForSliding_NewPosition)
	{
		XMStoreFloat4(&vResult, vTargetPos);
		//vResult.y -= 9.81f * fTimeDelta;

		_float3 vResult_Float3 = { vResult.x, vResult.y, vResult.z };
		
		_float fHeight = pNavigation->Compute_Height(vResult_Float3, &m_bIsGround);

		if (true == m_bIsGround)
		{
			vResult.y = fHeight;
		}

		Set_State(STATE_POSITION, XMLoadFloat4(&vResult));
	}
	
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	vLook = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Move_On_Navigation_ForSliding(vLook, fTimeDelta, pNavigation);

	//Move_On_Navigation(vLook, pNavigation);
}

void CTransform::Go_Straight_L45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook - vRight) * m_fSpeedPerSec * fTimeDelta;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Straight_R45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook + vRight) * m_fSpeedPerSec * fTimeDelta;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vResult = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * -1;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Backward_L45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook + vRight) * m_fSpeedPerSec * fTimeDelta * -1;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Backward_R45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook - vRight) * m_fSpeedPerSec * fTimeDelta * -1;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta * -1;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
	//Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;

	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(CTransform::STATE_UP);


	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;

	}

	Set_State(STATE_POSITION, vPosition);
}





void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_float fAdditionalRadian = m_fRotationPerSec * fTimeDelta;
	m_fRadian += fAdditionalRadian;

	_float fAngle = XMConvertToDegrees(m_fRadian);
	fAngle = _float((_int)fAngle % 360);
	m_fRadian = XMConvertToRadians(fAngle);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fAdditionalRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	m_fRadian = fRadian;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

_bool CTransform::Rotation_Lerp(_float fRadian, _float fTimeDelta, _float fMinRadian)
{
	_fvector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_fRadian = SMath::Extract_PitchYawRollFromRotationMatrix(m_WorldMatrix).y;

	_float vLocalPos;



	_float fTargetAngle = XMConvertToDegrees(fRadian);
	_float fAngle = XMConvertToDegrees(m_fRadian);


	_float fDiff = max(abs(fTargetAngle - fAngle) / 60.f, 1.f);

	if (fMinRadian > abs(fTargetAngle - fAngle))
	{
		Rotation(vAxis, fRadian);
		return true;
	}

	_int iDir = abs(fTargetAngle - fAngle < 180) ? 1 : -1;
	iDir *= (fTargetAngle > fAngle) ? 1 : -1;

	_vector      vRight = Get_State(STATE_RIGHT);
	_vector      vUp = Get_State(STATE_UP);
	_vector      vLook = Get_State(STATE_LOOK);

	_float fAdditionalRadian = m_fRotationPerSec * fTimeDelta * iDir * fDiff;
	m_fRadian += fAdditionalRadian;

	_matrix      RotationMatrix = XMMatrixRotationAxis(vAxis, fAdditionalRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

	return false;

}

void CTransform::Rotation_Quaternion(_float3 vRotation)
{
	_float3 vScale = Get_Scaled();

	_vector      vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector      vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector      vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_vector      vRot = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(vRotation.x)
															, XMConvertToRadians(vRotation.y)
															, XMConvertToRadians(vRotation.z));

	_matrix      RotationMatrix = XMMatrixRotationQuaternion(vRot);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vTargetPos - vPosition;



	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if(fDistance >= fSpare)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

_bool CTransform::Go_TargetArrivalCheck(_fvector vTargetPos, _double fTimeDelta, _float fSpare)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));
	//Look_At(vTargetPos);
	Look_At_Lerp(vTargetPos, fTimeDelta, 0.2f);

	if (fDistance >= fSpare)
	{
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
		
		return true;
	}
	else
		return false;
	
}

void CTransform::Look_At(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = XMVector3Normalize(vTargetPos - vPosition) * vScale.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Look_At_OnLand(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f))) * vScale.z;

	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	
	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Look_At_OnLandBoss(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp)) * vScale.z;




	//vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	//
	//vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f))) * vScale.z;

	//_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRadian + XMConvertToRadians(270)));


}

void CTransform::Look_At_Direction(_fvector _vLook)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook	= XMVector3Normalize(_vLook);
	_vector		vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vRight	= XMVector3Normalize(XMVector3Cross(vUp, vLook));
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	vRight	*= vScale.x;
	vUp		*= vScale.y;
	vLook	*= vScale.z;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Look_At_Lerp(_fvector vTargetPos, _float fTimeDelta, _float fMinRadian)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	_matrix matrixLook = SMath::Bake_MatrixNormalizeUseLookVector(vLook);

	_float fRadian = SMath::Extract_PitchYawRollFromRotationMatrix(matrixLook).y;

	Rotation_Lerp(fRadian, fTimeDelta, fMinRadian);
}


_vector CTransform::Calc_Look_Dir(_fvector vTargetPos)
{
	return XMVector3Normalize(Get_State(STATE::STATE_POSITION) - vTargetPos);
}

_vector CTransform::Calc_Look_Dir_XZ(_fvector vTargetPos)
{
	_float4 vLook;
	XMStoreFloat4(&vLook, Calc_Look_Dir(vTargetPos));
	vLook.y = 0.f;

	return XMLoadFloat4(&vLook);
}

void CTransform::Move_Position(_float4 vDir, _float fSpeed, _float fTimeDelta)
{
	/* 총알용입니다. 네비없어도됨 */
	/* 주의사항. vDir채로 더하므로 전에 미리 Normalize해서 넘기세요 */

	_float4 vPos = Get_Position();
	vPos += vDir * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPos);
}

void CTransform::Add_RootBone_Position(const _float3& vPos, CNavigation* pNavigation)
{
	_vector vRootMove = XMVector3TransformNormal(XMLoadFloat3(&vPos), m_WorldMatrix);
	_vector vResult = vRootMove;

	//Move_On_Navigation_ForSliding(vResult, m_pGameInstance->Get_TimeDelta(), pNavigation);
	
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Add_RootBone_ForTarget(const _float3& vPos, CNavigation* pNavigation, CTransform* pTargetTransform)
{
	_vector vRootMove = XMVector3TransformNormal(XMLoadFloat3(&vPos), pTargetTransform->Get_WorldFloat4x4());
	_vector vResult = vRootMove;

	//Move_On_Navigation_ForSliding(vResult, m_pGameInstance->Get_TimeDelta(), pNavigation);

	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Add_RootBone_Position(const _float3& vPos, const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vRootMove = XMVector3TransformNormal(XMLoadFloat3(&vPos), m_WorldMatrix);
	_vector vResult = vRootMove;

	//Move_On_Navigation_ForSliding(vResult, m_pGameInstance->Get_TimeDelta(), pNavigation);

	Move_On_Navigation(vResult, pNavigation);
	//Move_On_Navigation_ForSliding(vResult, fTimeDelta, pNavigation);
}

_bool CTransform::Calc_FrontCheck(const _float3& vTargetPos)
{
	_float3 vMyLook = Get_Look();
	_float3 vMyPos = Get_Pos();
	_float3 vTargetDir = XMVector3Normalize(vTargetPos - vMyPos);
	
	//! 앞인지 뒤인지 구분
	{
		if (XMVectorGetX(XMVector3Dot(vTargetDir, vMyLook)) > 0)
			return true;
		else
			return false;
	}

}

_bool CTransform::Calc_LeftCheck_ForCamLook(const _float3& vTargetPos)
{
	
	_vector vCamDir = XMLoadFloat4(&m_pGameInstance->Get_CamDirection());
	_vector vCamUp = { 0.f, 1.f, 0.f};
	_vector vTargetPos1 = { vTargetPos.x, vTargetPos.y, vTargetPos.z, 1.f};

	_vector vTargetToCamDir = { vTargetPos1 - XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) };

	_vector vRight = XMVector3Normalize(XMVector3Cross(vCamUp, vCamDir));
	
	if (XMVectorGetX(XMVector3Dot(vTargetToCamDir, vRight)) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


_float3 Calculate_SlidingVector(const _fvector& velocity, const _fvector& normal) {
	// velocity: 물체의 현재 속도
	// normal: 충돌 지점의 법선 벡터

	// 법선 벡터와 속도 벡터 사이의 수직 성분을 계산
	_vector normalComponent = XMVector3Dot(velocity, normal) * normal;
	// 수평 성분을 계산
	_vector tangentComponent = velocity - normalComponent;

	// 수평 성분이 없으면(법선 벡터와 평행한 경우), 슬라이딩 벡터는 영벡터
	if (XMVector3Equal(tangentComponent, XMVectorZero())) {
		return _float3(0.0f, 0.0f, 0.0f);
	}

	// 수평 성분을 정규화하여 슬라이딩 벡터를 계산
	_vector slidingVector = XMVector3Normalize(tangentComponent);

	// 슬라이딩 벡터를 반환
	_float3 result;
	XMStoreFloat3(&result, slidingVector);
	return result;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

	
CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _float fSpeedPerSec, _float fRotationPerSec)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(fSpeedPerSec, fRotationPerSec)))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
