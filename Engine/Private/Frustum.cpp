#include "..\Public\Frustum.h"
#include "GameInstance.h"


CFrustum::CFrustum()
	: m_pGameInstance(CGameInstance::GetInstance())	
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	/* 투영공간상의 여덟개 점을 정의하자. */
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	_matrix		ProjMatrixInv = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	_matrix		ViewMatrixInv = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	_float3		vPoints[8];	

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrixInv));
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&vPoints[i]), ViewMatrixInv));
	}

	Make_Planes(vPoints, m_WorldPlanes);	
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	_matrix		WorldMatrixTranspose = XMMatrixTranspose(WorldMatrix);	

	for (size_t i = 0; i < 6; i++)
	{
		XMStoreFloat4(&m_LocalPlanes[i], XMPlaneTransform(XMLoadFloat4(&m_WorldPlanes[i]), WorldMatrixTranspose));
	}

}

_bool CFrustum::isIn_WorldPlanes(_fvector vPoint, _float fRadius)
{	
	/*137  로아, 제2의나라 */
	/*138  원신, 오딘 */
	/*139   */

	/*a b c d 
	  x y z 1*/
	// ax + by + cz + d > 0

	// ax + by + cz + d == 5

	for (size_t i = 0; i < 6; i++)
	{
		if(fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlanes[i]), vPoint)))
			return false;
	}

	return true;
}

_bool CFrustum::isIn_LocalPlanes(_fvector vPoint, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlanes[i]), vPoint)))
			return false;
	}

	return true;
}

void CFrustum::Make_Planes(const _float3 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&m_WorldPlanes[0], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[1]), 
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[6])));

	XMStoreFloat4(&m_WorldPlanes[1], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[3])));

	XMStoreFloat4(&m_WorldPlanes[2], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[1])));

	XMStoreFloat4(&m_WorldPlanes[3], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[3]),
		XMLoadFloat3(&pPoints[2]),
		XMLoadFloat3(&pPoints[6])));

	XMStoreFloat4(&m_WorldPlanes[4], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[7])));

	XMStoreFloat4(&m_WorldPlanes[5], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[1]),
		XMLoadFloat3(&pPoints[2])));
}

CFrustum * CFrustum::Create()
{
	CFrustum*		pInstance = new CFrustum;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
