#include "..\Public\VIBuffer_Particle_Point.h"

#include "Easing_Utillity.h"
#include "SMath.h"

// 안씀 -> CVIBuffer_Particle로 사용합니다.
CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{

}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs)
	: CVIBuffer_Instancing(rhs)
	, m_pLengths(rhs.m_pLengths)
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
	m_pLengths = new _float[m_iNumInstance];

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

	//m_tBufferDesc = *(PARTICLE_POINT_DESC*)pArg;

	//return S_OK;

	m_fTimeAcc = 0.f;

	m_InstancingDesc = *(INSTANCING_DESC*)pArg;
	m_tBufferDesc = *(PARTICLE_BUFFER_DESC*)pArg;

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

	uniform_real_distribution<float>	RandomRange(m_tBufferDesc.vMinMaxRange.x, m_tBufferDesc.vMinMaxRange.y);

	uniform_real_distribution<float>	RandomRotationX(XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetX.x), XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetX.y));
	uniform_real_distribution<float>	RandomRotationY(XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetY.x), XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetY.y));
	uniform_real_distribution<float>	RandomRotationZ(XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetZ.x), XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetZ.y));

	uniform_real_distribution<float>	RandomScale(m_tBufferDesc.vMinMaxScale.x, m_tBufferDesc.vMinMaxScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_tBufferDesc.vMinMaxSpeed.x, m_tBufferDesc.vMinMaxSpeed.y);

	//uniform_real_distribution<float>	RandomLifeTime(m_tBufferDesc.vMinMaxLifeTime.x, m_tBufferDesc.vMinMaxLifeTime.y);

	uniform_real_distribution<float>	RandomLengthPosition(m_tBufferDesc.vMinMaxRangeLength.x, m_tBufferDesc.vMinMaxRangeLength.y);

	uniform_real_distribution<float>	RandomRed(min(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y), max(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y));
	uniform_real_distribution<float>	RandomGreen(min(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y), max(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y));
	uniform_real_distribution<float>	RandomBlue(min(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y), max(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y));
	uniform_real_distribution<float>	RandomAlpha(min(m_tBufferDesc.vMinMaxAlpha.x, m_tBufferDesc.vMinMaxAlpha.y), max(m_tBufferDesc.vMinMaxAlpha.x, m_tBufferDesc.vMinMaxAlpha.y));

	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		if (FADE_IN == m_tBufferDesc.eType_Fade)
			pVertices[i].vColor.w = 0.f;
		else
			pVertices[i].vColor.w = 1.f;

		m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
		m_pLifeTimes[i] = SMath::fRandom(m_tBufferDesc.vMinMaxLifeTime.x, m_tBufferDesc.vMinMaxLifeTime.y);
		m_pLengths[i] = RandomLengthPosition(m_RandomNumber);


		_float	fScale = RandomScale(m_RandomNumber);
		m_tBufferDesc.vCurrentScale.x = fScale + m_tBufferDesc.vAddScale.x;
		m_tBufferDesc.vCurrentScale.y = fScale + m_tBufferDesc.vAddScale.y;

		m_tBufferDesc.vRotationOffset = { RandomRotationX(m_RandomNumber), RandomRotationY(m_RandomNumber), RandomRotationZ(m_RandomNumber) };
		_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_tBufferDesc.vRotationOffset.x, m_tBufferDesc.vRotationOffset.y, m_tBufferDesc.vRotationOffset.z);

		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f) * m_tBufferDesc.vCurrentScale.x;
		pVertices[i].vUp	= _float4(0.f, 1.f, 0.f, 0.f) * m_tBufferDesc.vCurrentScale.x;
		pVertices[i].vLook	= _float4(0.f, 0.f, 1.0f, 0.f);


		vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);
		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		_vector vPos = XMLoadFloat4(&pVertices[i].vPosition);
		if (SPHERE == m_tBufferDesc.eType_Action)
		{
			if (m_tBufferDesc.bReverse)
			{
				vDir = XMVector3TransformNormal(vDir, RotationMatrix);
				vDir = vDir * m_tBufferDesc.vMinMaxRangeLength;

				XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_tBufferDesc.vCenterPosition) + vDir);
			}
			else
			{
				XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_tBufferDesc.vCenterPosition) + XMVector3TransformNormal(vDir, RotationMatrix));
			}
		}
		else
		{
			XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_tBufferDesc.vCenterPosition) + XMVector3TransformNormal(vDir, RotationMatrix));
		}
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
	if (0 >= m_iNumInstance)
		return;

	//if (m_tBufferDesc.bPlay)
	{
		m_fTimeAcc += fTimeDelta;	// 시간누적
		_float fTime = m_fTimeAcc / m_tBufferDesc.vMinMaxLifeTime.y;

		if (m_tBufferDesc.vMinMaxLifeTime.y <= m_fTimeAcc)
		{
			if (m_tBufferDesc.bLoop)
			{
				//if (SPHERE == m_tBufferDesc.eType_Action)
				ReSet();
				//else
				//{
				//	m_fTimeAcc = 0.f;
				//}
			}
			else
			{
				m_fTimeAcc = m_tBufferDesc.vMinMaxLifeTime.y;
				return;
			}
		}

		D3D11_MAPPED_SUBRESOURCE			SubResource = {};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);


		for (_uint i = 0; i < m_iNumInstance; i++)
		{
			_float		fAlpha;
			if (FADE_OUT == m_tBufferDesc.eType_Fade)
			{
				fAlpha = max((m_pLifeTimes[i]) - m_fTimeAcc, 0.f);
			}
			if (FADE_IN == m_tBufferDesc.eType_Fade)
			{
				fAlpha = min((m_pLifeTimes[i]) + m_fTimeAcc, 1.f);
			}
			if (FADE_NONE == m_tBufferDesc.eType_Fade)
			{
				fAlpha = 1.f;
			}

			m_tBufferDesc.vCurrentColor.x = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y, m_fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));
			m_tBufferDesc.vCurrentColor.y = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y, m_fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));
			m_tBufferDesc.vCurrentColor.z = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y, m_fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));

			pVertices[i].vColor = m_tBufferDesc.vCurrentColor;
			pVertices[i].vColor.w = fAlpha;

			if (SPHERE == m_tBufferDesc.eType_Action)
			{
				_vector		vDir;

				if (m_tBufferDesc.bReverse)
					vDir = XMLoadFloat4(&m_tBufferDesc.vCenterPosition) - XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition));
				else
					vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat4(&m_tBufferDesc.vCenterPosition));

				// 가속도
				_float fSpeed = 0.f;
				if (fTime < m_tBufferDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_tBufferDesc.fSpeedAcc;
				else
					fSpeed = (m_pSpeeds[i] - fSpeed) * m_fTimeAcc / m_tBufferDesc.vMinMaxLifeTime.y + m_pSpeeds[i];



				if (m_tBufferDesc.bReverse)
				{
					// 현재 위치에서 센터까지의 거리
					_vector vCurPos = XMLoadFloat4(&pVertices[i].vPosition);
					_float	fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_tBufferDesc.vCenterPosition) - vCurPos));

					if (fLength < m_tBufferDesc.vMinMaxRangeLength.y)
					{
						vDir = XMVectorSetW(vDir, 0.f);
						XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * fSpeed * fTimeDelta);
					}
				}
				else
				{
					// 센터에서 현재 위치까지의 거리
					_vector vCurPos = XMLoadFloat4(&pVertices[i].vPosition);
					_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_tBufferDesc.vCenterPosition)));

					if (fLength < m_pLengths[i])
					{
						vDir = XMVectorSetW(vDir, 0.f);
						XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * fSpeed * fTimeDelta);
					}
				}


			}

			if (SPARK == m_tBufferDesc.eType_Action)
			{
				_vector		vDir;

				if (m_tBufferDesc.bReverse)
					vDir = XMLoadFloat4(&m_tBufferDesc.vCenterPosition) - XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition));
				else
					vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat4(&m_tBufferDesc.vCenterPosition));

				// 가속도
				_float fSpeed = 0.f;
				if (fTime < m_tBufferDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_tBufferDesc.fSpeedAcc;
				else
					fSpeed = (m_pSpeeds[i] - fSpeed) * m_fTimeAcc / m_tBufferDesc.vMinMaxLifeTime.y + m_pSpeeds[i];


				vDir = XMVectorSetW(vDir, 0.f);

				if (m_tBufferDesc.bUseGravity)
				{
					if (fTime > m_tBufferDesc.fUseGravityPosition)
					{
						_vector vWithGravityDir = vDir * fSpeed * fTimeDelta;

						// 중력에 의한 변화 계산(안됨. 전체가 아래로만 떨어짐..포물선모양이고 싶은데)
						_vector vGravity = XMVectorSet(0.f, m_tBufferDesc.fGravityAcc * fTimeDelta, 0.f, 0.f);

						// 새로운 위치 계산
						XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vWithGravityDir + vGravity);
						//XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * fSpeed * fTimeDelta);
					}
					else
					{
						XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * fSpeed * fTimeDelta);
					}

				}
				else
				{
					XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * fSpeed * fTimeDelta);
				}

			}

			if (FALL == m_tBufferDesc.eType_Action)
			{
				// 가속도
				_float fSpeed = 0.f;
				if (fTime < m_tBufferDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_tBufferDesc.fSpeedAcc;
				else
					fSpeed = (m_pSpeeds[i] - fSpeed) * m_fTimeAcc / m_tBufferDesc.vMinMaxLifeTime.y + m_pSpeeds[i];

				pVertices[i].vPosition.y -= fSpeed * fTimeDelta;

				if (m_tBufferDesc.vMinMaxRange.x >= pVertices[i].vPosition.y)
					pVertices[i].vPosition.y = m_tBufferDesc.vMinMaxRange.y;
			}

			if (RISE == m_tBufferDesc.eType_Action)
			{
				// 가속도
				_float fSpeed = 0.f;
				if (fTime < m_tBufferDesc.fAccPosition)
					fSpeed = m_pSpeeds[i] + m_tBufferDesc.fSpeedAcc;
				else
					fSpeed = (m_pSpeeds[i] - fSpeed) * m_fTimeAcc / m_tBufferDesc.vMinMaxLifeTime.y + m_pSpeeds[i];

				pVertices[i].vPosition.y += fSpeed * fTimeDelta;

				if (m_tBufferDesc.vMinMaxRange.y <= pVertices[i].vPosition.y)
					pVertices[i].vPosition.y = m_tBufferDesc.vMinMaxRange.x;
			}

			if (BLINK == m_tBufferDesc.eType_Action)
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

	uniform_real_distribution<float>	RandomRange(m_tBufferDesc.vMinMaxRange.x, m_tBufferDesc.vMinMaxRange.y);

	uniform_real_distribution<float>	RandomRotationX(XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetX.x), XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetX.y));
	uniform_real_distribution<float>	RandomRotationY(XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetY.x), XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetY.y));
	uniform_real_distribution<float>	RandomRotationZ(XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetZ.x), XMConvertToRadians(m_tBufferDesc.vMinMaxRotationOffsetZ.y));

	uniform_real_distribution<float>	RandomScale(m_tBufferDesc.vMinMaxScale.x, m_tBufferDesc.vMinMaxScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_tBufferDesc.vMinMaxSpeed.x, m_tBufferDesc.vMinMaxSpeed.y);
	//uniform_real_distribution<float>	RandomLifeTime(m_tBufferDesc.vMinMaxLifeTime.x, m_tBufferDesc.vMinMaxLifeTime.y);
	uniform_real_distribution<float>	RandomLengthPosition(m_tBufferDesc.vMinMaxRangeLength.x, m_tBufferDesc.vMinMaxRangeLength.y);

	uniform_real_distribution<float>	RandomRed(min(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y), max(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y));
	uniform_real_distribution<float>	RandomGreen(min(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y), max(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y));
	uniform_real_distribution<float>	RandomBlue(min(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y), max(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y));
	uniform_real_distribution<float>	RandomAlpha(min(m_tBufferDesc.vMinMaxAlpha.x, m_tBufferDesc.vMinMaxAlpha.y), max(m_tBufferDesc.vMinMaxAlpha.x, m_tBufferDesc.vMinMaxAlpha.y));

	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		if (FADE_IN == m_tBufferDesc.eType_Fade)
			pVertices[i].vColor.w = 0.f;
		else
			pVertices[i].vColor.w = 1.f;

		m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
		m_pLifeTimes[i] = SMath::fRandom(m_tBufferDesc.vMinMaxLifeTime.x, m_tBufferDesc.vMinMaxLifeTime.y);
		m_pLengths[i] = RandomLengthPosition(m_RandomNumber);


		_float	fScale = RandomScale(m_RandomNumber);
		m_tBufferDesc.vCurrentScale.x = fScale + m_tBufferDesc.vAddScale.x;
		m_tBufferDesc.vCurrentScale.y = fScale + m_tBufferDesc.vAddScale.y;

		m_tBufferDesc.vRotationOffset = { RandomRotationX(m_RandomNumber), RandomRotationY(m_RandomNumber), RandomRotationZ(m_RandomNumber) };
		_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_tBufferDesc.vRotationOffset.x, m_tBufferDesc.vRotationOffset.y, m_tBufferDesc.vRotationOffset.z);


		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f) * m_tBufferDesc.vCurrentScale.x;
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f) * m_tBufferDesc.vCurrentScale.x;
		pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);


		vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);
		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		_vector vPos = XMLoadFloat4(&pVertices[i].vPosition);
		if (SPHERE == m_tBufferDesc.eType_Action)
		{
			//if (m_tBufferDesc.bReverse)
			//{
			//	vDir = XMVector3TransformNormal(vDir, RotationMatrix);
			//	vDir = vDir * m_tBufferDesc.fMaxLengthPosition;

			//	XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_tBufferDesc.vCenterPosition) + vDir);

			//}
			//else
			{
				XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_tBufferDesc.vCenterPosition) + XMVector3TransformNormal(vDir, RotationMatrix));
			}
		}
		else
		{


			XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_tBufferDesc.vCenterPosition) + XMVector3TransformNormal(vDir, RotationMatrix));
		}
		pVertices[i].vPosition.w = 1.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Point::Sort_Z(_uint iCount)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	vector<VTXINSTANCE> instanceData;
	instanceData.resize(iCount);
	memcpy(instanceData.data(), SubResource.pData, iCount * sizeof(VTXINSTANCE));


	_float4 fCamPos = m_pGameInstance->Get_CamPosition();
	_vector vCampos = XMVectorSet(fCamPos.x, fCamPos.y, fCamPos.z, fCamPos.w);

	// 현재 순서 그대로 해당 위치의 값 뷰Z를 구함.
	vector<_float> vecViewZ;
	for (size_t i = 0; i < iCount; i++) {
		_vector vPosition = XMVectorSet(instanceData[i].vPosition.x, instanceData[i].vPosition.y, instanceData[i].vPosition.z, instanceData[i].vPosition.w);
		vecViewZ.push_back(XMVectorGetX(XMVector3Length(vCampos - vPosition)));
	}

	// m_vecViewZ를 기준으로 정렬된 인덱스 구함.
	vector<size_t> sortedIndices(vecViewZ.size());
	iota(sortedIndices.begin(), sortedIndices.end(), 0);
	sort(sortedIndices.begin(), sortedIndices.end(), [&](size_t a, size_t b) {
		return vecViewZ[a] > vecViewZ[b]; }
	);

	// 정렬된 인덱스를 기반으로 다른 컨테이너들도 정렬
	vector<VTXINSTANCE>          sortedInstanceData(instanceData.size());

	for (size_t i = 0; i < sortedIndices.size(); ++i) {
		size_t index = sortedIndices[i];
		sortedInstanceData[i] = instanceData[index];
	}

	// 정렬된 결과 다시 할당
	memcpy(SubResource.pData, sortedInstanceData.data(), iCount * sizeof(VTXINSTANCE));


	m_pContext->Unmap(m_pVBInstance, 0);
}


_bool CVIBuffer_Particle_Point::Write_Json(json& Out_Json)
{
	Out_Json["Com_VIBuffer"]["iCurNumInstance"] = m_tBufferDesc.iCurNumInstance;

	Out_Json["Com_VIBuffer"]["eType_Action"] = m_tBufferDesc.eType_Action;
	Out_Json["Com_VIBuffer"]["eType_Fade"] = m_tBufferDesc.eType_Fade;
	Out_Json["Com_VIBuffer"]["eType_ColorLerp"] = m_tBufferDesc.eType_ColorLerp;

	Out_Json["Com_VIBuffer"]["bLoop"] = m_tBufferDesc.bLoop;
	Out_Json["Com_VIBuffer"]["bReverse"] = m_tBufferDesc.bReverse;

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxLifeTime"], m_tBufferDesc.vMinMaxLifeTime);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRangeLength"], m_tBufferDesc.vMinMaxRangeLength);

	CJson_Utility::Write_Float4(Out_Json["Com_VIBuffer"]["vCenterPosition"], m_tBufferDesc.vCenterPosition);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxSpeed"], m_tBufferDesc.vMinMaxSpeed);

	Out_Json["Com_VIBuffer"]["fSpeedAcc"] = m_tBufferDesc.fSpeedAcc;
	Out_Json["Com_VIBuffer"]["fAccPosition"] = m_tBufferDesc.fAccPosition;

	Out_Json["Com_VIBuffer"]["bUseGravity"] = m_tBufferDesc.bUseGravity;
	Out_Json["Com_VIBuffer"]["fGravityAcc"] = m_tBufferDesc.fGravityAcc;
	Out_Json["Com_VIBuffer"]["fUseGravityPosition"] = m_tBufferDesc.fUseGravityPosition;


	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vRotationOffset"], m_tBufferDesc.vRotationOffset);


	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vCurrentRotation"], m_tBufferDesc.vCurrentRotation);
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vMinMaxRotationForce"], m_tBufferDesc.vMinMaxRotationForce);


	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxScale"], m_tBufferDesc.vMinMaxScale);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vAddScale"], m_tBufferDesc.vAddScale);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vCurrentScale"], m_tBufferDesc.vCurrentScale);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRed"], m_tBufferDesc.vMinMaxRed);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxGreen"], m_tBufferDesc.vMinMaxGreen);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxBlue"], m_tBufferDesc.vMinMaxBlue);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxAlpha"], m_tBufferDesc.vMinMaxAlpha);

	CJson_Utility::Write_Float4(Out_Json["Com_VIBuffer"]["vCurrentColor"], m_tBufferDesc.vCurrentColor);


	return true;
}

void CVIBuffer_Particle_Point::Load_FromJson(const json& In_Json)
{
	m_tBufferDesc.iCurNumInstance = In_Json["Com_VIBuffer"]["iCurNumInstance"];

	m_tBufferDesc.eType_Action = In_Json["Com_VIBuffer"]["eType_Action"];
	m_tBufferDesc.eType_Fade = In_Json["Com_VIBuffer"]["eType_Fade"];
	m_tBufferDesc.eType_ColorLerp = In_Json["Com_VIBuffer"]["eType_ColorLerp"];

	m_tBufferDesc.bLoop = In_Json["Com_VIBuffer"]["bLoop"];
	m_tBufferDesc.bReverse = In_Json["Com_VIBuffer"]["bReverse"];

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxLifeTime"], m_tBufferDesc.vMinMaxLifeTime);


	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRangeLength"], m_tBufferDesc.vMinMaxRangeLength);

	CJson_Utility::Load_Float4(In_Json["Com_VIBuffer"]["vCenterPosition"], m_tBufferDesc.vCenterPosition);


	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxSpeed"], m_tBufferDesc.vMinMaxSpeed);


	m_tBufferDesc.fSpeedAcc = In_Json["Com_VIBuffer"]["fSpeedAcc"];
	m_tBufferDesc.fAccPosition = In_Json["Com_VIBuffer"]["fAccPosition"];


	m_tBufferDesc.bUseGravity = In_Json["Com_VIBuffer"]["bUseGravity"];
	m_tBufferDesc.fGravityAcc = In_Json["Com_VIBuffer"]["fGravityAcc"];
	m_tBufferDesc.fUseGravityPosition = In_Json["Com_VIBuffer"]["fUseGravityPosition"];


	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vRotationOffset"], m_tBufferDesc.vRotationOffset);

	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vCurrentRotation"], m_tBufferDesc.vCurrentRotation);
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vMinMaxRotationForce"], m_tBufferDesc.vMinMaxRotationForce);


	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxScale"], m_tBufferDesc.vMinMaxScale);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vAddScale"], m_tBufferDesc.vAddScale);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vCurrentScale"], m_tBufferDesc.vCurrentScale);


	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRed"], m_tBufferDesc.vMinMaxRed);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxGreen"], m_tBufferDesc.vMinMaxGreen);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxBlue"], m_tBufferDesc.vMinMaxBlue);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxAlpha"], m_tBufferDesc.vMinMaxAlpha);


	CJson_Utility::Load_Float4(In_Json["Com_VIBuffer"]["vCurrentColor"], m_tBufferDesc.vCurrentColor);

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

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pLengths);
	}

}
