#include "Camera.h"
#include "Transform.h"
#include "Light_Manager.h"
#include "GameInstance.h"

float g_fCameraFOV = XM_PI / 4.0f;
float g_fAspectRatio = 1.0f;

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
	, m_fCascadeTotalRange(50.f)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC*	pCameraDesc = (CAMERA_DESC*)pArg;	

	/* 항등상태의 트랜스폼을 내가 셋팅하고 싶은 카메라의 상태로 동기화하낟. */
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->Look_At(XMLoadFloat4(&pCameraDesc->vAt));

	m_fFovy = pCameraDesc->fFovy;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	/* Cascade */
	//// Set the range values
	//m_arrCascadeRanges[0] = m_fNear; /*  { Cam_near, Cam_far, Cam_fovY, Cam_aspectRatio };*/
	//m_arrCascadeRanges[1] = 10.0f;
	//m_arrCascadeRanges[2] = 25.0f;
	//m_arrCascadeRanges[3] = m_fCascadeTotalRange;
	//
	//for (int i = 0; i < m_iTotalCascades; i++)
	//{
	//	m_arrCascadeBoundCenter[i] = _float3(0.0f, 0.0f, 0.0f);
	//	m_arrCascadeBoundRadius[i] = 0.0f;
	//}

	return S_OK;
}

void CCamera::Priority_Tick(_float fTimeDelta)
{
}

void CCamera::Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
	m_arrCameraWorld = m_pTransformCom->Get_WorldFloat4x4();

	//Update_Cascade();
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}


void CCamera::Set_Pos(float4x4 vPos)
{
	m_arrCameraWorld = vPos;
}

void CCamera::Update_Cascade(const float3& vDirectionalDir)
{
	_float3 vWorldCenter = *GetEyePt() + *GetWorldAhead() * m_fCascadeTotalRange* 0.5f;
	_float3 vPos = vWorldCenter; // worldCenter 기준으로 뷰매트릭스 생성 : Directional Light의 위치를 특정하기 어려워서 
	_float3 vLookAt = vWorldCenter + vDirectionalDir * m_fFar;
	_float3 vUp;
	_float3 vRight = _float3(1.0f, 0.0f, 0.0f);
	XMStoreFloat3(&vUp, XMVector3Cross(vDirectionalDir, vRight));
	XMVector3Normalize(vUp);
	_float4x4 mShadowView;
	mShadowView = XMMatrixLookAtLH(XMLoadFloat3(&vPos), XMLoadFloat3(&vLookAt), XMLoadFloat3(&vUp));


	// Get the bounds for the shadow space
}

const _float3* CCamera::GetEyePt() const
{
	return (_float3*)&m_arrCameraWorld._41;
}

const _float3* CCamera::GetWorldAhead() const
{
	return (_float3*)&m_arrCameraWorld._31;
}

const _float3* CCamera::GetWorldUp() const
{
	return (_float3*)&m_arrCameraWorld._21;
}

const _float3* CCamera::GetWorldRight() const
{
	return (_float3*)&m_arrCameraWorld._11;
}

void CCamera::ExtractFrustumBoundSphere(_float fNear, _float fFar, _float3& vBoundCenter, _float& fBoundRadius)
{
	// Get the camera bases
	const _float3& camPos = *GetEyePt();
	const _float3& camRight = *GetWorldRight();
	const _float3& camUp = *GetWorldUp();
	const _float3& camForward = *GetWorldAhead();

	// Calculate the tangent values (this can be cached as long as the FOV doesn't change)
	const _float fTanFOVX = tanf(g_fAspectRatio * g_fCameraFOV);
	const _float fTanFOVY = tanf(g_fAspectRatio);

	// The center of the sphere is in the center of the frustum
	vBoundCenter = camPos + camForward * (fNear + 0.5f * (fNear + fFar));

	// Radius is the distance to one of the frustum far corners
	const _float3 vBoundSpan = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fFar - vBoundCenter;
	//fBoundRadius = D3DXVec3Length(&vBoundSpan);
}


void CCamera::Free()
{
	__super::Free();

}
