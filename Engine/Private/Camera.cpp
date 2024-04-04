#include "Camera.h"
#include "Transform.h"
#include "Light_Manager.h"
#include "GameInstance.h"
#include "Light.h"
#include "Light_Manager.h"

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
	NULL_CHECK_RETURN(pArg, E_FAIL);

	FAILED_CHECK(__super::Initialize(pArg));

	CAMERA_DESC*	pCameraDesc = (CAMERA_DESC*)pArg;	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->Look_At(XMLoadFloat4(&pCameraDesc->vAt));

	m_fFovy = pCameraDesc->fFovy;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	FAILED_CHECK(Ready_Components());

	/* Cascade */
	FAILED_CHECK(Ready_CSM());

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

void CCamera::Update_Cascade()
{
	//_float3 vWorldCenter = *GetEyePt() + *GetWorldAhead() * m_fCascadeTotalRange* 0.5f;
	//_float3 vPos = vWorldCenter; // worldCenter 기준으로 뷰매트릭스 생성 : Directional Light의 위치를 특정하기 어려워서 
	//_float3 vLookAt = vWorldCenter + vDirectionalDir * m_fFar;
	//_float3 vUp;
	//_float3 vRight = _float3(1.0f, 0.0f, 0.0f);
	//XMStoreFloat3(&vUp, XMVector3Cross(vDirectionalDir, vRight));
	//XMVector3Normalize(vUp);
	//_float4x4 mShadowView;
	//mShadowView = XMMatrixLookAtLH(XMLoadFloat3(&vPos), XMLoadFloat3(&vLookAt), XMLoadFloat3(&vUp));

	// Get the bounds for the shadow space

	// 카메라의 역행렬과 시야각,화면비, 가까운 평면 Z,먼 평면 Z
	_float4x4 matWorld = m_pTransformCom->Get_WorldMatrix();
	_float4x4 matView = m_pTransformCom->Get_WorldMatrixInverse();

	//시야각을 이용하여 수직 시야각을 구함
	_float tanHalfVFov = tanf(m_fFovy / 2.0f);
	// 수직 시야각을 이용하여 수평 시야각을 구함
	_float tanHalfHFov = tanHalfVFov * m_fAspect;

	// 절두체를 나누기위한 각 부분 절두체의 끝 지점 선언
	m_fCascadeEnd[0] = m_fNear;
	m_fCascadeEnd[1] = 10.f;
	m_fCascadeEnd[2] = 20.f;
	m_fCascadeEnd[3] = 40.f;

	// 3개의 절두체 나누기 
	for (uint32_t i = 0; i < 3; ++i)
	{
		//+X,+Y 좌표에 수평,수직 시야각을 이용하여 구함. 각 부분 절두체의 가까운,먼
		//평면의 값을 곱하여 4개의 점을 구함
		_float xn = m_fCascadeEnd[i]		* tanHalfHFov;
		_float xf = m_fCascadeEnd[i + 1]	* tanHalfHFov;
		_float yn = m_fCascadeEnd[i]		* tanHalfVFov;
		_float yf = m_fCascadeEnd[i + 1]	* tanHalfVFov;

		//+좌표값을 구하면 -좌표값을 구하여 각각의 절두체 평면을 구할수있음.
		//각 절두체의 Z값을 저장하여 i 가 낮은 순서로 가까운 평면 먼평면을 구성함
		_float4 frustumCorners[8] =
		{
			//near Face
			{	 xn,	 yn,	m_fCascadeEnd[i],	1.0f},
			{	-xn,	 yn,	m_fCascadeEnd[i],	1.0f},
			{	 xn,	-yn,	m_fCascadeEnd[i],	1.0f},
			{	-xn,	-yn,	m_fCascadeEnd[i],	1.0f},
			//far Face
			{	xf,		yf,		m_fCascadeEnd[i + 1],	1.0f},
			{	-xf,	yf,		m_fCascadeEnd[i + 1],	1.0f},
			{	xf,	   -yf,		m_fCascadeEnd[i + 1],	1.0f},
			{	-xf,-	yf,		m_fCascadeEnd[i + 1],	1.0f}
		};

		_float4 centerPos;
		for (uint32_t j = 0; j < 8; ++j)
		{
			frustumCorners[j] = XMVector4Transform(frustumCorners[j], matWorld);
			centerPos += frustumCorners[j];
		}

		centerPos /= 8.0f;
		_float radius = 0.0f;

		for (uint32_t j = 0; j < 8; ++j)
		{
			//_float distance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&(frustumCorners[j] - centerPos))));
			// Compute distance between frustumCorner[j] and centerPos
			_float dx = frustumCorners[j].x - centerPos.x;
			_float dy = frustumCorners[j].y - centerPos.y;
			_float dz = frustumCorners[j].z - centerPos.z;
			_float distanceSquared = dx * dx + dy * dy + dz * dz;
			_float distance = sqrt(distanceSquared);

			radius = max(radius, distance);
		}

		radius = ceil(radius * 16.0f) / 16.0f;

		// using radius ,  we made aabb box
		_float3 maxExtents(radius, radius, radius + 60.0f);
		_float3 minExtents = -maxExtents;

		_float4 vLightDir4 = m_pGameInstance->Get_LightManager()->Get_ShadowLightDir(m_pGameInstance->Get_NextLevel());
		_float3 vLightDir = { vLightDir4.x, vLightDir4.y, vLightDir4.z };
		_float3 shadowCamPos = _float3(centerPos) + (vLightDir * minExtents.z);

		XMVECTOR zaxis = XMVector3Normalize(XMVectorNegate(XMLoadFloat3(&vLightDir)));
		XMFLOAT3 UP = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMVECTOR yaxis = XMLoadFloat3(&UP);
		XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
		yaxis = XMVector3Cross(zaxis, xaxis);

		_float4x4 lightMatrix;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&lightMatrix._11), xaxis);
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&lightMatrix._21), yaxis);
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&lightMatrix._31), zaxis);
		lightMatrix._14 = lightMatrix._24 = lightMatrix._34 = 0.f;
		lightMatrix._41 = shadowCamPos.x; 
		lightMatrix._42 = shadowCamPos.y; 
		lightMatrix._43 = shadowCamPos.z;
		lightMatrix._44 = 1.f;

		XMMATRIX M = XMLoadFloat4x4(&lightMatrix);
		_float4x4 lightInv;
		XMVECTOR det;
		XMStoreFloat4x4(&lightInv, XMMatrixInverse(&det, M));

		_float3 cascadeExtents = maxExtents - minExtents;

		_float4x4 matProj = XMMatrixOrthographicLH(cascadeExtents.x, cascadeExtents.y, 0.0f, cascadeExtents.z);
		m_shadowOrthoProj[i] = lightInv * matProj;

		_float3 vPos, vScale;
		_float4 vQuat = { 0.f, 0.f, 0.f, 1.f };


		XMVECTOR s, r, t;
		if (!XMMatrixDecompose(&s, &r, &t, lightMatrix))
			return;

		XMStoreFloat3(&vScale, s);
		XMStoreFloat4(&vQuat, r);
		XMStoreFloat3(&vPos, t);

		m_tCascadeShadowBox[i].Orientation = vQuat;
		m_tCascadeShadowBox[i].Center = float3(centerPos);
		m_tCascadeShadowBox[i].Extents = maxExtents;
	}

	m_pGameInstance->Set_ShadowProj(m_shadowOrthoProj);
	m_pGameInstance->Set_CascadeBoxes(m_tCascadeShadowBox);
}

HRESULT CCamera::Ready_CSM()
{
	// Set the range values
	m_arrCascadeRanges[0] = m_fNear; /*  { Cam_near, Cam_far, Cam_fovY, Cam_aspectRatio };*/
	m_arrCascadeRanges[1] = 10.0f;
	m_arrCascadeRanges[2] = 25.0f;
	m_arrCascadeRanges[3] = m_fCascadeTotalRange;

	for (int i = 0; i < m_iTotalCascades; i++)
	{
		m_arrCascadeBoundCenter[i] = _float3(0.0f, 0.0f, 0.0f);
		m_arrCascadeBoundRadius[i] = 0.0f;
	}

	return S_OK;
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
