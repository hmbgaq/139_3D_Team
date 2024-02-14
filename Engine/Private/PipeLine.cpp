#include "..\Public\PipeLine.h"

CPipeLine::CPipeLine()
{

}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_Transform[eState], TransformMatrix);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
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

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; i++)
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

}