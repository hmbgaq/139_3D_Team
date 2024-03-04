#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

#include "SMath.h"

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
	}
	Set_State(STATE_POSITION, vPosition);
	
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	vLook = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	Move_On_Navigation(vLook, pNavigation);
}

void CTransform::Go_Straight_L45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook - vRight) * m_fSpeedPerSec * fTimeDelta;
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Straight_R45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook + vRight) * m_fSpeedPerSec * fTimeDelta;
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vResult = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * -1;
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Backward_L45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook + vRight) * m_fSpeedPerSec * fTimeDelta * -1;
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Backward_R45(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE_LOOK);
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vLook - vRight) * m_fSpeedPerSec * fTimeDelta * -1;
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta * -1;
	Move_On_Navigation(vResult, pNavigation);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vResult = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	Move_On_Navigation(vResult, pNavigation);
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

_bool CTransform::Rotation_Lerp(_float fRadian, _float fTimeDelta)
{
	_fvector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_fRadian = SMath::Extract_PitchYawRollFromRotationMatrix(m_WorldMatrix).y;

	_float fTargetAngle = XMConvertToDegrees(fRadian);
	_float fAngle = XMConvertToDegrees(m_fRadian);

	if (1.f > abs(fTargetAngle - fAngle))
	{
		Rotation(vAxis, fRadian);
		return true;
	}

	_int iDir = fTargetAngle > fAngle ? 1 : -1;

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_float fAdditionalRadian = m_fRotationPerSec * fTimeDelta * iDir;
	m_fRadian += fAdditionalRadian;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fAdditionalRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

	return false;
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

void CTransform::Look_At_Lerp(_fvector vTargetPos, _float fTimeDelta)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	_matrix matrixLook = SMath::Bake_MatrixNormalizeUseLookVector(vLook);

	_float fRadian = SMath::Extract_PitchYawRollFromRotationMatrix(matrixLook).y;

	Rotation_Lerp(fRadian, fTimeDelta);
}


_vector CTransform::Calc_Look_Dir(_fvector vTargetPos)
{
	return XMVector3Normalize(Get_State(STATE::STATE_POSITION) - vTargetPos);
}

_float3 CTransform::Calc_Look_Dir(_float3 vTargetPos)
{
	return Get_Position() - vTargetPos;
}

void CTransform::Add_RootBone_Position(const _float3& vPos, CNavigation* pNavigation)
{
	_vector vRootMove = XMVector3TransformNormal(XMLoadFloat3(&vPos), m_WorldMatrix);
	_vector vResult = vRootMove;
	Move_On_Navigation(vResult, pNavigation);
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
