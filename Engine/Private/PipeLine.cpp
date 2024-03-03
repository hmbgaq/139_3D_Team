#include "PipeLine.h"
#include "GameInstance.h"
#
CPipeLine::CPipeLine()
	: m_pGameInstace(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstace);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (D3DTS_VIEW == eState)
	{
		m_PreViewMatrix = m_Transform[D3DTS_VIEW];
	}
	else if (D3DTS_PROJ == eState)
	{
		m_PreProjMatrix = m_Transform[D3DTS_PROJ];
	}

	XMStoreFloat4x4(&m_Transform[eState], TransformMatrix);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	if (D3DTS_VIEW == eState)
	{
		m_PreViewMatrix = m_Transform[D3DTS_VIEW];
		m_PreWorldMatrix = m_Transform[D3DTS_WORLD];

	}
	else if (D3DTS_PROJ == eState)
	{
		m_PreProjMatrix = m_Transform[D3DTS_PROJ];
	}

	m_Transform[eState] = TransformMatrix;
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eState)
{
	return m_Transform[eState];
}

_matrix CPipeLine::Get_TransformMatrixInverse(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform_Inverse[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4Inverse(D3DTRANSFORMSTATE eState)
{
	return m_Transform_Inverse[eState];
}

_float4 CPipeLine::Get_CamPosition()
{
	return m_vCamPosition;
}

_float4 CPipeLine::Get_CamSetting()
{
	_float4 Result = {};

	// 투영의 역행렬 계산
	_matrix projectionMatrix = XMLoadFloat4x4(&m_Transform[D3DTS_PROJ]);
	_matrix projectionInverse = XMMatrixInverse(nullptr, projectionMatrix);

	// 역행렬에서 near, far, fov 추출
	_float Cam_near = projectionInverse.r[3].m128_f32[2] / (projectionInverse.r[2].m128_f32[2] - 1.0f);
	_float Cam_far = projectionInverse.r[3].m128_f32[2] / (projectionInverse.r[2].m128_f32[2] + 1.0f);
	_float Cam_fovY = atan(1.0f / projectionInverse.r[1].m128_f32[1]) * 2.0f;
	_float Cam_aspectRatio = projectionInverse.r[0].m128_f32[0] / projectionInverse.r[1].m128_f32[1];

	Result = { Cam_near, Cam_far, Cam_fovY, Cam_aspectRatio };

	return Result;
}


//RAY CPipeLine::Get_MouseRayWorld(HWND g_hWnd, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY)
//{
//	POINT pt;
//	GetCursorPos(&pt);
//	ScreenToClient(g_hWnd, &pt);
//
//	_vector vProjPos(XMVectorSet(pt.x / (g_iWinSizeX * 0.5f) - 1.f, pt.y / -(g_iWinSizeY * 0.5f) + 1.f, 0.f, 0.f));
//
//	_matrix ProjMatrixInv = Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);
//
//	_vector vViewPos(XMVector3TransformCoord(vProjPos, ProjMatrixInv));
//
//	_vector vRayDir(vViewPos);
//	_vector vRayPos(XMVectorSet(0.f, 0.f, 0.f, 1.f));
//
//	_matrix ViewMatrixInv = Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);
//
//	RAY MouseRay;
//	ZeroMemory(&MouseRay, sizeof(RAY));
//
//	XMStoreFloat3(&MouseRay.vDirection, XMVector3Normalize(XMVector3TransformNormal(vRayDir, ViewMatrixInv)));
//	XMStoreFloat4(&MouseRay.vPosition, XMVector3TransformCoord(vRayPos, ViewMatrixInv));
//
//	MouseRay.fLength = 1000000.0f;
//
//	return MouseRay;
//}

//RAY CPipeLine::Get_MouseRayLocal(HWND g_hWnd, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY, _matrix matWorld)
//{
//	RAY vMouseRayLocal;
//	ZeroMemory(&vMouseRayLocal, sizeof(vMouseRayLocal));
//
//	RAY vMouseRayWorld = Get_MouseRayWorld(g_hWnd, g_iWinSizeX, g_iWinSizeY);
//
//	_vector vMousePos = XMLoadFloat4(&vMouseRayWorld.vPosition);
//	_vector vMouseDir = XMLoadFloat3(&vMouseRayWorld.vDirection);
//
//	_matrix matWorldInv = XMMatrixInverse(nullptr, matWorld);
//	vMousePos = XMVector3TransformCoord(vMousePos, matWorldInv);
//	vMouseDir = XMVector3Normalize(XMVector3TransformNormal(vMouseDir, matWorldInv));
//
//	XMStoreFloat4(&vMouseRayLocal.vPosition, vMousePos);
//	XMStoreFloat3(&vMouseRayLocal.vDirection, vMouseDir);
//	vMouseRayLocal.fLength = 1000000.0f;
//
//	return vMouseRayLocal;
//}

_float4x4 CPipeLine::Get_PreViewMatrix()
{
	_float4x4 PreViewMat = m_PreViewMatrix;
	
	return PreViewMat;
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < ECast(D3DTS_END); i++)
	{
		XMStoreFloat4x4(&m_Transform[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < ECast(D3DTS_END); i++)
	{
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_Transform[i])));
	}

	memcpy(&m_vCamPosition, &m_Transform_Inverse[D3DTS_VIEW].m[3], sizeof(_float4));
}

CPipeLine * CPipeLine::Create()
{
	CPipeLine*		pInstance = new CPipeLine;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CPipeLine::Free()
{
	Safe_Release(m_pGameInstace);
}