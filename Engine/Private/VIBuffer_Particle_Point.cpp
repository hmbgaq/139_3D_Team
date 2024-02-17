#include "..\Public\VIBuffer_Particle_Point.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{

}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(_uint iNumInstance)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOINT);
	m_iInstanceStride = sizeof(VTXINSTANCE);
	m_iIndexCountPerInstance = 1;
	m_iNumInstance = iNumInstance;

	m_pSpeeds = new _float[m_iNumInstance];
	m_pLifeTimes = new _float[m_iNumInstance];

	m_iNumIndices = iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXPOINT* pVertices = new VTXPOINT;

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices->vPSize = _float2(1.0f, 1.f);

	m_SubResourceData.pSysMem = pVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	///* 인스턴스용 버퍼를 생성한다. */
	//if (FAILED(__super::Initialize(pArg)))
	//	return E_FAIL;

	//m_ParticleDesc = *(PARTICLE_POINT_DESC*)pArg;

	//return S_OK;

	m_fTimeAcc = 0.f;

	m_InstancingDesc = *(INSTANCING_DESC*)pArg;
	m_ParticleDesc = *(PARTICLE_POINT_DESC*)pArg;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	_vector				vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	uniform_real_distribution<float>	RandomRange(m_ParticleDesc.vMinMaxRange.x, m_ParticleDesc.vMinMaxRange.y);

	uniform_real_distribution<float>	RandomRotationX(XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetX.x), XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetX.y));
	uniform_real_distribution<float>	RandomRotationY(XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetY.x), XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetY.y));
	uniform_real_distribution<float>	RandomRotationZ(XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetZ.x), XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetZ.y));

	uniform_real_distribution<float>	RandomScale(m_ParticleDesc.vMinMaxScale.x, m_ParticleDesc.vMinMaxScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_ParticleDesc.vMinMaxSpeed.x, m_ParticleDesc.vMinMaxSpeed.y);
	uniform_real_distribution<float>	RandomLifeTime(m_ParticleDesc.vMinMaxLifeTime.x, m_ParticleDesc.vMinMaxLifeTime.y);

	m_iNumInstance = m_ParticleDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		if(FADE_IN == m_ParticleDesc.eType_Fade)
			pVertices[i].vColor.w = 0.f;
		else
			pVertices[i].vColor.w = 1.f;
		

		m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
		m_pLifeTimes[i] = RandomLifeTime(m_RandomNumber);

		_float	fScale = RandomScale(m_RandomNumber);
		m_ParticleDesc.vCurrentScale.x = fScale + m_ParticleDesc.vAddScale.x;
		m_ParticleDesc.vCurrentScale.y = fScale + m_ParticleDesc.vAddScale.y;

		m_ParticleDesc.vRotationOffset = { RandomRotationX(m_RandomNumber), RandomRotationY(m_RandomNumber), RandomRotationZ(m_RandomNumber) };
		_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_ParticleDesc.vRotationOffset.x, m_ParticleDesc.vRotationOffset.y, m_ParticleDesc.vRotationOffset.z);

		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f) * m_ParticleDesc.vCurrentScale.x;
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f) * m_ParticleDesc.vCurrentScale.x;
		pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);

		vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);

		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_ParticleDesc.vCenterPosition) + XMVector3TransformNormal(vDir, RotationMatrix));
		pVertices[i].vPosition.w = 1.f;
	}


	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;

}

void CVIBuffer_Particle_Point::Update(_float fTimeDelta)
{
	if ( 0 >= m_iNumInstance)
		return;

	if (m_ParticleDesc.bIsPlay)
	{
		m_fTimeAcc += fTimeDelta;	// 시간누적
		_float fTime = m_fTimeAcc / m_ParticleDesc.vMinMaxLifeTime.y;

		if (m_ParticleDesc.vMinMaxLifeTime.y <= m_fTimeAcc)
		{
			if (m_ParticleDesc.bLoop)
			{
				if(SPHERE == m_ParticleDesc.eType_Action)
					ReSet();
				//else
				//{
				//	m_fTimeAcc = 0.f;
				//}
			}
			else
			{
				m_fTimeAcc = m_ParticleDesc.vMinMaxLifeTime.y;
				return;
			}
		}

		D3D11_MAPPED_SUBRESOURCE			SubResource = {};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

		for (_uint i = 0; i < m_iNumInstance; i++)
		{
			_float		fAlpha;
			if (FADE_OUT == m_ParticleDesc.eType_Fade)
			{
				fAlpha = max(m_pLifeTimes[i] - m_fTimeAcc, 0.f);
			}	
			if (FADE_IN == m_ParticleDesc.eType_Fade)
			{
				fAlpha = MIN(m_pLifeTimes[i] - m_fTimeAcc, 1.f);
			}
			if (FADE_NONE == m_ParticleDesc.eType_Fade)
			{
				fAlpha = 1.f;
			}
				
			pVertices[i].vColor = m_ParticleDesc.vCurrentColor;
			pVertices[i].vColor.w = fAlpha;

			if (SPHERE == m_ParticleDesc.eType_Action)
			{
				_vector		vDir;

				if (m_ParticleDesc.bReverse)
					vDir = XMLoadFloat3(&m_ParticleDesc.vCenterPosition) - XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition));
				else
					vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat3(&m_ParticleDesc.vCenterPosition));

				// 가속도
				_float fSpeed;
				if (fTime < m_ParticleDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_ParticleDesc.fAcceleration;
				else
					fSpeed = m_pSpeeds[i];

				// 센터에서 현재 위치까지의 거리
				_vector vCurPos = XMLoadFloat4(&pVertices[i].vPosition);	
				_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat3(&m_ParticleDesc.vCenterPosition)));

				if (fLength < m_ParticleDesc.fMaxLengthPosition)
				{
					vDir = XMVectorSetW(vDir, 0.f);
					XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * fSpeed * fTimeDelta);
				}
			}

			if (FALL == m_ParticleDesc.eType_Action)
			{
				// 가속도
				_float fSpeed;
				if (fTime < m_ParticleDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_ParticleDesc.fAcceleration;
				else
					fSpeed = m_pSpeeds[i];

				pVertices[i].vPosition.y -= fSpeed * fTimeDelta;

				if (m_ParticleDesc.vMinMaxRange.x >= pVertices[i].vPosition.y)
					pVertices[i].vPosition.y = m_ParticleDesc.vMinMaxRange.y;
			}

			if (RISE == m_ParticleDesc.eType_Action)
			{
				// 가속도
				_float fSpeed;
				if (fTime < m_ParticleDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_ParticleDesc.fAcceleration;
				else
					fSpeed = m_pSpeeds[i];

				pVertices[i].vPosition.y += fSpeed * fTimeDelta;

				if (m_ParticleDesc.vMinMaxRange.x >= pVertices[i].vPosition.y)
					pVertices[i].vPosition.y = m_ParticleDesc.vMinMaxRange.y;
			}

			if (TORNADO == m_ParticleDesc.eType_Action)
			{

			}

		}

		m_pContext->Unmap(m_pVBInstance, 0);
	}

}

void CVIBuffer_Particle_Point::ReSet()
{
	m_fTimeAcc = 0.f;

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

	_vector				vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_float				fLength = { 0.0f };

	uniform_real_distribution<float>	RandomRange(m_ParticleDesc.vMinMaxRange.x, m_ParticleDesc.vMinMaxRange.y);

	uniform_real_distribution<float>	RandomRotationX(XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetX.x), XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetX.y));
	uniform_real_distribution<float>	RandomRotationY(XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetY.x), XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetY.y));
	uniform_real_distribution<float>	RandomRotationZ(XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetZ.x), XMConvertToRadians(m_ParticleDesc.vMinMaxRotationOffsetZ.y));

	uniform_real_distribution<float>	RandomScale(m_ParticleDesc.vMinMaxScale.x, m_ParticleDesc.vMinMaxScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_ParticleDesc.vMinMaxSpeed.x, m_ParticleDesc.vMinMaxSpeed.y);
	uniform_real_distribution<float>	RandomLifeTime(m_ParticleDesc.vMinMaxLifeTime.x, m_ParticleDesc.vMinMaxLifeTime.y);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		if (FADE_IN == m_ParticleDesc.eType_Fade)
			pVertices[i].vColor.w = 0.f;
		else
			pVertices[i].vColor.w = 1.f;

		m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
		m_pLifeTimes[i] = RandomLifeTime(m_RandomNumber);

		_float	fScale = RandomScale(m_RandomNumber);
		m_ParticleDesc.vCurrentScale.x = fScale + m_ParticleDesc.vAddScale.x;
		m_ParticleDesc.vCurrentScale.y = fScale + m_ParticleDesc.vAddScale.y;

		m_ParticleDesc.vRotationOffset = { RandomRotationX(m_RandomNumber), RandomRotationY(m_RandomNumber), RandomRotationZ(m_RandomNumber) };
		_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_ParticleDesc.vRotationOffset.x, m_ParticleDesc.vRotationOffset.y, m_ParticleDesc.vRotationOffset.z);


		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f) * m_ParticleDesc.vCurrentScale.x;
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f) * m_ParticleDesc.vCurrentScale.x;
		pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);


		vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);

		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_ParticleDesc.vCenterPosition) + XMVector3TransformNormal(vDir, RotationMatrix));
		pVertices[i].vPosition.w = 1.f;

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Particle_Point::Set_RotationOffset(MINMAX eMinMax, AXIS eAxis, _float fRotationOffset)
{
	if (MIN == eMinMax)
	{
		switch (eAxis)
		{
		case Engine::AXIS_X:
			m_ParticleDesc.vMinMaxRotationOffsetX.x = fRotationOffset;
			break;

		case Engine::AXIS_Y:
			m_ParticleDesc.vMinMaxRotationOffsetY.x = fRotationOffset;;
			break;

		case Engine::AXIS_Z:
			m_ParticleDesc.vMinMaxRotationOffsetZ.x = fRotationOffset;;
			break;
		}
	}

	if (MAX == eMinMax)
	{
		switch (eAxis)
		{
		case Engine::AXIS_X:
			m_ParticleDesc.vMinMaxRotationOffsetX.y = fRotationOffset;;
			break;

		case Engine::AXIS_Y:
			m_ParticleDesc.vMinMaxRotationOffsetY.y = fRotationOffset;;
			break;

		case Engine::AXIS_Z:
			m_ParticleDesc.vMinMaxRotationOffsetZ.y = fRotationOffset;;
			break;
		}
	}

}

void CVIBuffer_Particle_Point::Set_Color(_float fRed, _float fGreen, _float fBlue)
{
	m_ParticleDesc.vCurrentColor.x = fRed;
	m_ParticleDesc.vCurrentColor.y = fGreen;
	m_ParticleDesc.vCurrentColor.z = fBlue;
}


CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Particle_Point::Clone(void* pArg)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
	__super::Free();

}
