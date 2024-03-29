#include "VIBuffer_Particle.h"

#include "SMath.h"
#include "Easing_Utillity.h"

#include "GameInstance.h"
#include "PipeLine.h"

CVIBuffer_Particle::CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle& rhs)
	: CVIBuffer(rhs)
	, m_tBufferDesc(rhs.m_tBufferDesc)
{
}

HRESULT CVIBuffer_Particle::Initialize_Prototype()
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOINT);

	m_iNumIndices = m_tBufferDesc.iCurNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOINT* pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(1.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEXBUFFER

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

HRESULT CVIBuffer_Particle::Initialize(void* pArg)
{
	m_tBufferDesc = *(PARTICLE_BUFFER_DESC*)pArg;

	// 벡터 공간 예약
	m_vecParticleInfoDesc.reserve(m_tBufferDesc.iCurNumInstance);
	m_vecParticleShaderInfoDesc.reserve(m_tBufferDesc.iCurNumInstance);

	if (m_tBufferDesc.bUseRigidBody)
		m_vecParticleRigidbodyDesc.reserve(m_tBufferDesc.iCurNumInstance);


	// 시간 초기화
	m_tBufferDesc.Reset_Times();

	// ==============================================================================================

	FAILED_CHECK(Init_Instance(m_tBufferDesc.iCurNumInstance));

	return S_OK;
}


HRESULT CVIBuffer_Particle::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};


	/* 어떤 버텍스 버퍼들을 이용할거다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Particle::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Particle::Init_Instance(_int iNumInstance)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOINT);
	m_iInstanceStride = sizeof(VTXINSTANCE);
	m_iIndexCountPerInstance = 1;
	m_iNumInstance = iNumInstance;

	m_iNumIndices = iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; ++i)	// 반복문 시작
	{
		// 초기화용 파티클 입자 하나하나의 정보 Push_back
		PARTICLE_INFO_DESC tParticleInfo = {};
		m_vecParticleInfoDesc.push_back(tParticleInfo);

		PARTICLE_SHADER_INFO_DESC tParticleShaderInfo = {};
		m_vecParticleShaderInfoDesc.push_back(tParticleShaderInfo);


		// 리지드바디 사용이면 입자 하나하나를 위한 초기화용 리지드바디정보 Push_back		
		if (m_tBufferDesc.bUseRigidBody)
		{
			PARTICLE_RIGIDBODY_DESC tParticleRigidbody = {};
			m_vecParticleRigidbodyDesc.push_back(tParticleRigidbody);
		}


		ReSet_Info(i);

		Rotation_Instance(i);	// 점 자체 회전

		// 쉐이더에 던질 라업룩 값으로 초기화
		//pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
		//pVertices[i].vUp	= _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
		//pVertices[i].vLook	= _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };

		pVertices[i].vRight = _float4{ 0.f, 0.f, 0.f, 0.f };
		pVertices[i].vUp =	_float4{ 0.f, 0.f, 0.f, 0.f };
		pVertices[i].vLook = _float4{ 0.f, 0.f, 0.f, 0.f };


		if (m_tBufferDesc.bUseRigidBody)	// 리지드바디 사용이면
		{
			// 센터 위치로 세팅
			XMStoreFloat4(&pVertices[i].vPosition, m_vecParticleInfoDesc[i].vCenterPositions);
		}
		else
		{
			// 리지드바디 사용이 아닐경우
			// , 이미터 사용이 아닐 경우
			//if (!m_tBufferDesc.bUseEmitter)
			{
				//_vector		vDir = Make_Dir(i);					// 방향 만들기
				//m_vecParticleInfoDesc[i].vDir = vDir;			// 쉐이더에 전달할 방향 저장
				//if (SPARK == m_tBufferDesc.eType_Action)
				//{
				//	Update_Spark_Rotation(i);
				//}

				//if (m_tBufferDesc.bReverse)
				//{
				//	// 리버스일 경우
				//	vDir = vDir * m_vecParticleInfoDesc[i].fMaxRange;
				//}

				//// 센터 + 방향 위치로 세팅
				//XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions) + vDir);

				XMStoreFloat4(&pVertices[i].vPosition, m_vecParticleInfoDesc[i].vCenterPositions);
			}

		}


		// 색 초기화
		pVertices[i].vColor = m_vecParticleShaderInfoDesc[i].vCurrentColors;


	} // 반복문 끝


	m_SubResourceData.pSysMem = pVertices;

	FAILED_CHECK(__super::Create_Buffer(&m_pVBInstance));

	Safe_Delete_Array(pVertices);

	return S_OK;

}


void CVIBuffer_Particle::ReSet()
{
	// 시간 초기화
	m_tBufferDesc.Reset_Times();

	// ==============================================================================================

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; ++i)	// 반복문 시작
	{
		// 방출 초기화
		m_tBufferDesc.bEmitFinished = FALSE;
		m_tBufferDesc.fEmissionTimeAcc = 0.f;
		m_tBufferDesc.iEmitCount = 0;
		

		ReSet_Info(i);
		Rotation_Instance(i);	// 점 자체 회전


		// 안보이게
		pVertices[i].vRight = _float4{ 0.f, 0.f, 0.f, 0.f };
		pVertices[i].vUp	= _float4{ 0.f, 0.f, 0.f, 0.f };
		pVertices[i].vLook	= _float4{ 0.f, 0.f, 0.f, 0.f };


		if (m_tBufferDesc.bUseRigidBody)	// 리지드바디 사용이면
		{
			// 센터 위치로 세팅
			XMStoreFloat4(&pVertices[i].vPosition, m_vecParticleInfoDesc[i].vCenterPositions);
		}
		else  
		{
			//// 리지드바디 사용이 아닐 경우
			//_vector		vDir = Make_Dir(i);					// 방향 만들기
			//m_vecParticleInfoDesc[i].vDir = vDir;			// 방향 저장

			//if (SPARK == m_tBufferDesc.eType_Action)
			//{
			//	Update_Spark_Rotation(i);
			//}

			//if (m_tBufferDesc.bReverse)
			//{
			//	// 리버스일 경우
			//	vDir = vDir * m_vecParticleInfoDesc[i].fMaxRange;
			//}

			//// 센터 + 방향 위치로 세팅
			//XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions) + vDir);


			XMStoreFloat4(&pVertices[i].vPosition, m_vecParticleInfoDesc[i].vCenterPositions);
		}



	} // 반복문 끝


	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Particle::ReSet_Info(_uint iNum)
{
	m_vecParticleInfoDesc[iNum].bEmit = FALSE;
	m_vecParticleInfoDesc[iNum].bDie = FALSE;


	// 라이프타임
	m_vecParticleInfoDesc[iNum].Reset_ParticleTimes();
	m_vecParticleInfoDesc[iNum].fLifeTime = SMath::fRandom(m_tBufferDesc.vMinMaxLifeTime.x, m_tBufferDesc.vMinMaxLifeTime.y);


	// 최대 거리
	m_vecParticleInfoDesc[iNum].fAddRange = SMath::fRandom(m_tBufferDesc.vMinMaxAddRange.x, m_tBufferDesc.vMinMaxAddRange.y);	// 추가 범위
	m_vecParticleInfoDesc[iNum].fMaxRange = SMath::fRandom(m_tBufferDesc.vMinMaxRange.x, m_tBufferDesc.vMinMaxRange.y) /*+ m_vecParticleInfoDesc[iNum].fAddRange*/;	// 최대 범위
	m_vecParticleInfoDesc[iNum].fMaxPosY = SMath::fRandom(min(m_tBufferDesc.vMinMaxPosY.x, m_tBufferDesc.vMinMaxPosY.y), max(m_tBufferDesc.vMinMaxPosY.x, m_tBufferDesc.vMinMaxPosY.y)); //최대 높이


	// 스피드
	m_vecParticleInfoDesc[iNum].fCurSpeed = SMath::fRandom(m_tBufferDesc.vMinMaxSpeed.x, m_tBufferDesc.vMinMaxSpeed.y);
	m_vecParticleInfoDesc[iNum].fCurTornadoSpeed = SMath::fRandom(m_tBufferDesc.vMinMaxTornadoSpeed.x, m_tBufferDesc.vMinMaxTornadoSpeed.y);

	// 원 회전(이동) 각도
	m_vecParticleInfoDesc[iNum].fCurTheta = XMConvertToRadians(SMath::fRandom(m_tBufferDesc.vMinMaxTheta.x, m_tBufferDesc.vMinMaxTheta.y));


	m_vecParticleInfoDesc[iNum].vOffsetTheta = { XMConvertToRadians(SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetX.x, m_tBufferDesc.vMinMaxRotationOffsetX.y))
												, XMConvertToRadians(SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetY.x, m_tBufferDesc.vMinMaxRotationOffsetY.y))
												, XMConvertToRadians(SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetZ.x, m_tBufferDesc.vMinMaxRotationOffsetZ.y)) };


	m_vecParticleInfoDesc[iNum].vCenterPositions.x = SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.x, m_tBufferDesc.vMaxCenterOffsetPos.x);
	m_vecParticleInfoDesc[iNum].vCenterPositions.y = SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.y, m_tBufferDesc.vMaxCenterOffsetPos.y);
	m_vecParticleInfoDesc[iNum].vCenterPositions.z = SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.z, m_tBufferDesc.vMaxCenterOffsetPos.z);




	// 크기
	if (m_tBufferDesc.bUseScaleLerp)
	{
		// 현재 크기는 시작 크기로
		m_vecParticleInfoDesc[iNum].vCurScales.x = m_tBufferDesc.vMinMaxWidth.x;
		m_vecParticleInfoDesc[iNum].vCurScales.y = m_tBufferDesc.vMinMaxHeight.x;

		// 스케일 러프 사용이면 최대 크기 고정
		m_vecParticleInfoDesc[iNum].vMaxScales.x = m_tBufferDesc.vMinMaxWidth.y;
		m_vecParticleInfoDesc[iNum].vMaxScales.y = m_tBufferDesc.vMinMaxHeight.y;
	}
	else
	{
		// 스케일 러프 사용이 아니면 현재 스케일은 범위 내 랜덤
		m_vecParticleInfoDesc[iNum].vCurScales.x = SMath::fRandom(m_tBufferDesc.vMinMaxWidth.x, m_tBufferDesc.vMinMaxWidth.y);
		m_vecParticleInfoDesc[iNum].vCurScales.y = m_vecParticleInfoDesc[iNum].vCurScales.x;	// 비율 고정
	}



#pragma region 리지드바디 시작
	// 리지드 바디 사용이면
	if (m_tBufferDesc.bUseRigidBody)
	{
		//_vector		vAddRangePos = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		//vAddRangePos = XMVector3Normalize(vAddRangePos) * m_vecParticleInfoDesc[iNum].fAddRange;

		//_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_vecParticleInfoDesc[iNum].vOffsetTheta.x, m_vecParticleInfoDesc[iNum].vOffsetTheta.y, m_vecParticleInfoDesc[iNum].vOffsetTheta.z);
		//_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		//vAddRangePos = XMVector3Normalize(XMVector3TransformNormal(vAddRangePos, RotationMatrix));	// 벡터 회전 적용

		//m_vecParticleInfoDesc[iNum].vCenterPositions.x = XMVectorGetX(vAddRangePos) + SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.x, m_tBufferDesc.vMaxCenterOffsetPos.x);
		//m_vecParticleInfoDesc[iNum].vCenterPositions.y = XMVectorGetY(vAddRangePos) + SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.y, m_tBufferDesc.vMaxCenterOffsetPos.y);
		//m_vecParticleInfoDesc[iNum].vCenterPositions.z = XMVectorGetZ(vAddRangePos) + SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.z, m_tBufferDesc.vMaxCenterOffsetPos.z);
		//m_vecParticleInfoDesc[iNum].vCenterPositions.w = 1.f;


		m_vecParticleRigidbodyDesc[iNum].bForced = FALSE; // 힘 안준걸로 초기화

		Clear_Power(iNum);	// 파워 리셋
		m_vecParticleRigidbodyDesc[iNum].fMass = SMath::fRandom(m_tBufferDesc.vMinMaxMass.x, m_tBufferDesc.vMinMaxMass.y);			// 질량 리셋

		// 방향 만들기
		//_vector		vDir = Make_Dir(iNum);
		//m_vecParticleInfoDesc[iNum].vDir = vDir;			// 방향 저장

	}
#pragma endregion 리지드바디 끝


	// 센터 포지션 & Offset
	if (TORNADO != m_tBufferDesc.eType_Action)
	{
		// 토네이도가 아닐때만 범위 오프셋 주기
		_vector		vAddRangePos = XMVectorSet(1.f, 1.f, 1.f, 0.f);

		//if (0.f < m_vecParticleInfoDesc[iNum].fAddRange) // 추가 범위가 0보다 크면 센터 크게
		{
			vAddRangePos = XMVector3Normalize(vAddRangePos) * m_vecParticleInfoDesc[iNum].fAddRange;

			_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_vecParticleInfoDesc[iNum].vOffsetTheta.x, m_vecParticleInfoDesc[iNum].vOffsetTheta.y, m_vecParticleInfoDesc[iNum].vOffsetTheta.z);
			_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

			vAddRangePos = XMVector3TransformNormal(vAddRangePos, RotationMatrix);	// 벡터 회전 적용

			m_vecParticleInfoDesc[iNum].vCenterPositions.x += XMVectorGetX(vAddRangePos);
			m_vecParticleInfoDesc[iNum].vCenterPositions.y += XMVectorGetY(vAddRangePos);
			m_vecParticleInfoDesc[iNum].vCenterPositions.z += XMVectorGetZ(vAddRangePos);
		}
	}



	// 방향 만들기
	_vector		vDir = { 1.f, 0.f, 0.f, 0.f };
	vDir = Make_Dir(iNum);
	m_vecParticleInfoDesc[iNum].vDir = vDir;	// 방향 저장



	// 색 초기화
	m_vecParticleShaderInfoDesc[iNum].vCurrentColors = { m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxAlpha.x };
	
	// 알파 초기화
	_float		fAlpha;
	if (FADE_NONE == m_tBufferDesc.eType_Fade)
	{
		fAlpha = 1.f;
	}
	else if (FADE_OUT == m_tBufferDesc.eType_Fade)
	{
		fAlpha = 1.f;
	}
	else if (FADE_IN == m_tBufferDesc.eType_Fade)
	{
		fAlpha = 0.f;
	}
	m_vecParticleShaderInfoDesc[iNum].vCurrentColors.w = fAlpha;

}

_float4 CVIBuffer_Particle::Make_Dir(_uint iNum)
{
	_vector		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);


	vDir = XMVector3Normalize(vDir) * m_vecParticleInfoDesc[iNum].fMaxRange;

	_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_vecParticleInfoDesc[iNum].vOffsetTheta.x, m_vecParticleInfoDesc[iNum].vOffsetTheta.y, m_vecParticleInfoDesc[iNum].vOffsetTheta.z);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, RotationMatrix));	// 가야할 방향벡터 회전 적용

	return vDir;
}


void CVIBuffer_Particle::Rotation_Instance(_uint iNum)
{

	_vector		vRight		= XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	_vector		vUp			= XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_vecParticleInfoDesc[iNum].vCurScales.y;
	_vector		vLook		= XMVectorSet(0.f, 0.f, 1.f, 0.f);


	_vector		vRotation = XMQuaternionRotationRollPitchYaw( XMConvertToRadians(m_tBufferDesc.vRadian.x)
															, XMConvertToRadians(m_tBufferDesc.vRadian.y)
															, XMConvertToRadians(m_tBufferDesc.vRadian.z));

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);


	// 쉐이더에 던질 값(라업룩) 저장 
	m_vecParticleShaderInfoDesc[iNum].vRight	= XMVector3TransformNormal(vRight, RotationMatrix);
	m_vecParticleShaderInfoDesc[iNum].vUp		= XMVector3TransformNormal(vUp, RotationMatrix);
	m_vecParticleShaderInfoDesc[iNum].vLook		= XMVector3TransformNormal(vLook, RotationMatrix);

}

void CVIBuffer_Particle::Update_Spark_Rotation(_uint iNum)
{	
	_vector		vRight, vUp, vLook;

	// 이동 진행 방향벡터를 Up으로 한 새로운 Right, Look 정해주기 ===================================
	vUp		= XMVector4Normalize(m_vecParticleInfoDesc[iNum].vDir) * m_vecParticleInfoDesc[iNum].vCurScales.y;
	vLook	= XMVector4Normalize(XMVector3Cross(float4(0.f, 1.f, 0.f, 0.f), vUp));
	vRight	= XMVector4Normalize(XMVector3Cross(vUp, vLook)) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	vLook	= XMVector4Normalize(XMVector3Cross(vRight, vUp));


	// 추가 회전
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_tBufferDesc.vRadian.x)
															, XMConvertToRadians(m_tBufferDesc.vRadian.y)
															, XMConvertToRadians(m_tBufferDesc.vRadian.z));

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);


	m_vecParticleShaderInfoDesc[iNum].vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	m_vecParticleShaderInfoDesc[iNum].vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	m_vecParticleShaderInfoDesc[iNum].vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	//m_vecParticleShaderInfoDesc[iNum].vRight = vRight;
	//m_vecParticleShaderInfoDesc[iNum].vUp = vUp;
	//m_vecParticleShaderInfoDesc[iNum].vLook = vLook;
}

void CVIBuffer_Particle::Update_Dir_Rotation(_uint iNum)
{
	_vector		vRight, vUp, vLook;

	// 이동 진행 방향벡터를 Look으로 한 새로운 Right, Look 정해주기 ===================================
	vLook	= XMVector4Normalize(m_vecParticleInfoDesc[iNum].vDir);
	vRight	= XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	vUp		= XMVector3Normalize(XMVector3Cross(vLook, vRight)) * m_vecParticleInfoDesc[iNum].vCurScales.y;
	//vRight	= XMVector4Normalize(XMVector3Cross(vUp, vLook)) * m_vecParticleInfoDesc[iNum].vCurScales.x;


	m_vecParticleShaderInfoDesc[iNum].vRight = vRight;
	m_vecParticleShaderInfoDesc[iNum].vUp = vUp;
	m_vecParticleShaderInfoDesc[iNum].vLook = vLook;

}


void CVIBuffer_Particle::Update(_float fTimeDelta)
{
#pragma region Map UnMap 전 조건 체크 시작
	if (0 >= m_iNumInstance)	// 인스턴스 개수가 0개 이하면 탈출
		return;


	// 누적 시간이 최대 라이프타임보다 커지면 시간 누적 안함 & 탈출
	if (m_tBufferDesc.fTimeAcc > m_tBufferDesc.vMinMaxLifeTime.y)
	{
		if (!m_tBufferDesc.bRecycle) // 리사이클이 아니면 값 고정 & 탈출
		{
			m_tBufferDesc.fTimeAcc = m_tBufferDesc.vMinMaxLifeTime.y;
			m_tBufferDesc.fLifeTimeRatio = 1.f;
			return;
		}		
		else
		{
			m_tBufferDesc.Reset_Times();
		}
	}

	// 시간 누적(전체)
	m_tBufferDesc.fTimeAcc += fTimeDelta;
	m_tBufferDesc.fLifeTimeRatio = min(1.0f, m_tBufferDesc.fTimeAcc / m_tBufferDesc.vMinMaxLifeTime.y);


	if (!m_tBufferDesc.bEmitFinished) // 방출이 끝이 아니면 
	{
		m_tBufferDesc.fEmissionTimeAcc += fTimeDelta; // 시간누적

		if (m_tBufferDesc.fEmissionTimeAcc >= m_tBufferDesc.fEmissionTime) // 하나씩 방출
		{
			if (m_iNumInstance <= m_tBufferDesc.iEmitCount) // 방출 카운트가 인스턴스개수와 크거나 같아지면 방출 끝
			{
				m_tBufferDesc.iEmitCount = m_iNumInstance;
				m_tBufferDesc.bEmitFinished = TRUE;
			}
			else
			{
				// 아니면 방출
				for (_uint i = 0; i <= m_tBufferDesc.iAddEmitCount; i++)
				{
					if (m_iNumInstance <= m_tBufferDesc.iEmitCount)	// 벡터 인덱스 맞추기용
						m_tBufferDesc.iEmitCount = m_iNumInstance - 1;

					m_vecParticleInfoDesc[m_tBufferDesc.iEmitCount].bEmit = TRUE;
					m_tBufferDesc.iEmitCount++;				
				}
				m_tBufferDesc.fEmissionTimeAcc = 0.f; // 초기화
			}

		}
	}

#pragma region Map UnMap 전 조건 체크 끝


	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);


	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; i++)	// 반복문 시작
	{

		if (m_vecParticleInfoDesc[i].bDie)
		{
			// 죽었으면 안보이게 & 다음 반복으로
			m_vecParticleShaderInfoDesc[i].vRight	= { 0.f, 0.f, 0.f, 0.f };
			m_vecParticleShaderInfoDesc[i].vUp		= { 0.f, 0.f, 0.f, 0.f };
			m_vecParticleShaderInfoDesc[i].vLook	= { 0.f, 0.f, 0.f, 0.f };

			pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
			pVertices[i].vUp	= _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
			pVertices[i].vLook	= _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
			m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };
			pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;

			m_vecParticleInfoDesc[i].fTimeAccs = m_vecParticleInfoDesc[i].fLifeTime;
			m_vecParticleInfoDesc[i].fLifeTimeRatios = 1.f;
			continue;
		}

		
		if (m_vecParticleInfoDesc[i].bEmit)	// 방출이 true이면 업데이트
		{
#pragma region 입자들 시간 시작
			// 입자들의 시간 누적이 (입자들의)라이프타임보다 커지면 시간 누적 안함 
			if (m_vecParticleInfoDesc[i].fTimeAccs > m_vecParticleInfoDesc[i].fLifeTime)
			{
				m_vecParticleInfoDesc[i].fTimeAccs = m_vecParticleInfoDesc[i].fLifeTime;
				m_vecParticleInfoDesc[i].fLifeTimeRatios = 1.f;

			}
			else
			{
				// 시간 누적(개별)
				m_vecParticleInfoDesc[i].fTimeAccs += fTimeDelta;
				m_vecParticleInfoDesc[i].fLifeTimeRatios = min(1.0f, m_vecParticleInfoDesc[i].fTimeAccs / m_vecParticleInfoDesc[i].fLifeTime);
			}

#pragma region 입자들 시간 끝


#pragma region 회전 : 자체 회전 시작
			Rotation_Instance(i);

			// 쉐이더에 던질 라업룩 값으로 초기화
			pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
			pVertices[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
			pVertices[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
#pragma region 회전 : 자체 회전 끝


#pragma region 이동 : 리지드바디 시작
			// 리지드바디 사용이면
			if (m_tBufferDesc.bUseRigidBody)
			{
				if (!m_vecParticleRigidbodyDesc.empty())
				{
					// 방출 됐으면 한 번만 힘줘야됨
					if (!m_vecParticleRigidbodyDesc[i].bForced)		// bForced가 false면 힘주기
					{				
						_vector vForce = m_vecParticleInfoDesc[i].vDir * SMath::fRandom(m_tBufferDesc.vMinMaxPower.x, m_tBufferDesc.vMinMaxPower.y);
						Add_Force(i, vForce, m_tBufferDesc.eForce_Mode);

						m_vecParticleRigidbodyDesc[i].bForced = TRUE; // 힘줬으면 true
					}
					else
					{
						// 마찰계수 러프
						// 0~1 사이로 보간한 라이프타임이 마찰계수 변화를 시작할 타임 포지션을 넘기면
						if (m_tBufferDesc.fLifeTimeRatio >= m_tBufferDesc.vFrictionLerp_Pos.x)
						{
							_float fTotalTime = m_tBufferDesc.vMinMaxLifeTime.y * (m_tBufferDesc.vFrictionLerp_Pos.y - m_tBufferDesc.vFrictionLerp_Pos.x);	// 라이프 타임 중, 변화에만 필요한 토탈시간 계산
							if (m_vecParticleRigidbodyDesc[i].fFrictionTimeAccs >= fTotalTime)
							{
								m_vecParticleRigidbodyDesc[i].fFrictionTimeAccs = fTotalTime;
								m_vecParticleRigidbodyDesc[i].fFriction = m_tBufferDesc.vStartEnd_Friction.y;
							}
							else
							{
								m_vecParticleRigidbodyDesc[i].fFrictionTimeAccs += fTimeDelta;	// 시간 누적		
								m_vecParticleRigidbodyDesc[i].fFriction = Easing::LerpToType(m_tBufferDesc.vStartEnd_Friction.x, m_tBufferDesc.vStartEnd_Friction.y, m_vecParticleRigidbodyDesc[i].fFrictionTimeAccs, fTotalTime, m_tBufferDesc.eType_FrictionLerp);
							}
						}


						if (!Check_Sleep(i))	// 슬립이 아니면 리지드바디 업데이트
						{
							if (m_tBufferDesc.bKinetic)
							{
								// 스피드 러프
								m_vecParticleInfoDesc[i].fCurSpeed = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxSpeed.x, m_tBufferDesc.vMinMaxSpeed.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_SpeedLerp));

								Update_Kinetic(i, fTimeDelta);	// 이동 속력 계산 업데이트

								// 계산된 속력으로 이동할 위치 계산 / Translate : vMovePos = vPos + Get_State(CTransform::STATE_POSITION);
								_vector vMovePos = (XMLoadFloat3(&m_vecParticleRigidbodyDesc[i].vVelocity) * (m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta)) + XMLoadFloat4(&pVertices[i].vPosition); XMVectorSetW(vMovePos, 1.f);
								//_vector vMovePos = XMLoadFloat3(&m_vecParticleRigidbodyDesc[i].vVelocity) * fTimeDelta + XMLoadFloat4(&pVertices[i].vPosition); XMVectorSetW(vMovePos, 1.f);
								XMStoreFloat4(&pVertices[i].vPosition, vMovePos);	// 최종 위치 이동 적용


								m_vecParticleInfoDesc[i].vDir = m_vecParticleRigidbodyDesc[i].vVelocity;			// 방향 저장
							}
							else
							{
								Update_Kinematic(i);
							}
						}

						if (SPARK == m_tBufferDesc.eType_Action)
						{
							// SPARK일때만 방사형으로 알아서 회전시킴
							Update_Spark_Rotation(i);
						}

						// 초기화 조건		
						_vector vCurPos = XMLoadFloat4(&pVertices[i].vPosition);

						_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions)));	// 센터에서 현재 위치까지의 거리
						//_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - vZeroCenter));	// 센터에서 현재 위치까지의 거리

						if (m_vecParticleInfoDesc[i].fMaxRange <= fLength)	// 현재 이동 거리가 맥스 레인지보다 크거나 같으면 초기화 or 죽음
						{
							if (m_tBufferDesc.bRecycle)	// 재사용이 true이면 초기화
							{
								// 랜덤 값 다시 뽑기
								ReSet_Info(i);
								m_vecParticleInfoDesc[i].bEmit = TRUE;


								// 안보이게
								m_vecParticleShaderInfoDesc[i].vRight = _float3{ 0.f, 0.f, 0.f };
								m_vecParticleShaderInfoDesc[i].vUp = _float3{ 0.f, 0.f, 0.f };
								m_vecParticleShaderInfoDesc[i].vLook = _float3{ 0.f, 0.f, 0.f };

								pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
								pVertices[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
								pVertices[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
								pVertices[i].vPosition = m_vecParticleInfoDesc[i].vCenterPositions;


								m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };
								pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;


								m_vecParticleInfoDesc[i].Reset_ParticleTimes();		// 시간 초기화
								continue;
							}
							else
							{
								m_vecParticleInfoDesc[i].bDie = TRUE;
							}
						}


					}

				}

				// 알파_리지드바디(거리)
				_vector vCurPos = XMLoadFloat4(&pVertices[i].vPosition);
				_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions)));	// 센터에서 현재 위치까지의 거리
				_float		fAlpha;
				if (FADE_NONE == m_tBufferDesc.eType_Fade)
				{
					fAlpha = 1.f;
				}
				else if (FADE_OUT == m_tBufferDesc.eType_Fade)
				{
					fAlpha = max(m_vecParticleInfoDesc[i].fMaxRange - fLength, 0.f);
				}
				else if (FADE_IN == m_tBufferDesc.eType_Fade)
				{
					fAlpha = min(fLength, 1.f);
				}
				m_vecParticleShaderInfoDesc[i].vCurrentColors.w = fAlpha;
				pVertices[i].vColor.w = fAlpha;

			}
#pragma endregion 이동 : 리지드바디 끝

#pragma region 이동 : 직접 이동 시작
			else
			{
				// 리지드바디 사용이 아니면 직접 이동
				if (SPARK == m_tBufferDesc.eType_Action)
				{
					// 이동 :: 현재 포지션 + (입자들의 방향 * (현재 스피드 * 타임델타))
					_vector vMovePos = XMLoadFloat4(&pVertices[i].vPosition) + m_vecParticleInfoDesc[i].vDir * (m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta);
					XMStoreFloat4(&pVertices[i].vPosition, vMovePos);


					// 초기화 조건		
					_vector vCurPos = XMLoadFloat4(&pVertices[i].vPosition);
					_vector vZeroCenter = { 0.f, 0.f, 0.f, 1.f };

					//_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions)));	// 센터에서 현재 위치까지의 거리
					_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - vZeroCenter));	// 센터에서 현재 위치까지의 거리

					if (m_vecParticleInfoDesc[i].fMaxRange <= fLength)	// 현재 이동 거리가 맥스 레인지보다 크거나 같으면 초기화 or 죽음
					{
						if (m_tBufferDesc.bRecycle)	// 재사용이 true이면 초기화
						{
							// 랜덤 값 다시 뽑기
							ReSet_Info(i);
							m_vecParticleInfoDesc[i].bEmit = TRUE;


							// 안보이게
							m_vecParticleShaderInfoDesc[i].vRight = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vUp = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vLook = _float3{ 0.f, 0.f, 0.f };

							pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
							pVertices[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
							pVertices[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
							pVertices[i].vPosition = m_vecParticleInfoDesc[i].vCenterPositions;
							// 센터 + 방향 위치로 세팅
							XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions) + m_vecParticleInfoDesc[i].vDir);

							m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };
							pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;


							m_vecParticleInfoDesc[i].Reset_ParticleTimes();		// 시간 초기화
						}
						else
						{
							m_vecParticleInfoDesc[i].bDie = TRUE;
						}
					}

				}
				else if (BLINK == m_tBufferDesc.eType_Action)
				{
					// 이동 없음

					// 초기화 조건
					if (m_vecParticleInfoDesc[i].fLifeTime <= m_vecParticleInfoDesc[i].fTimeAccs)	// 라이프 타임이 끝나면 초기화 or 죽음
					{
						if (m_tBufferDesc.bRecycle)	// 재사용이 true이면 초기화
						{
							// 랜덤 값 다시 뽑기
							ReSet_Info(i);
							m_vecParticleInfoDesc[i].bEmit = TRUE;


							// 안보이게
							m_vecParticleShaderInfoDesc[i].vRight = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vUp = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vLook = _float3{ 0.f, 0.f, 0.f };

							pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
							pVertices[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
							pVertices[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
							//pVertices[i].vPosition = m_vecParticleInfoDesc[i].vCenterPositions;
							// 센터 + 방향 위치로 세팅
							XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions) + m_vecParticleInfoDesc[i].vDir);

							m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };
							pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;


							// 시간 초기화
							m_vecParticleInfoDesc[i].Reset_ParticleTimes();
						}
						else
						{
							m_vecParticleInfoDesc[i].bDie = TRUE;
						}
					}

				}
				else if (FALL == m_tBufferDesc.eType_Action)
				{


				}
				else if (RISE == m_tBufferDesc.eType_Action)
				{
					pVertices[i].vPosition.y += m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta;	// 이동


					// 초기화 조건
					if (m_vecParticleInfoDesc[i].fMaxPosY <= pVertices[i].vPosition.y)	// 현재 y위치가 최대 범위보다 크면 초기화 or 죽음
					{
						if (m_tBufferDesc.bRecycle)	// 재사용이 true이면 초기화
						{

							// 랜덤 값 다시 뽑기
							ReSet_Info(i);
							m_vecParticleInfoDesc[i].bEmit = TRUE;


							// 안보이게
							m_vecParticleShaderInfoDesc[i].vRight = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vUp = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vLook = _float3{ 0.f, 0.f, 0.f };

							pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
							pVertices[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
							pVertices[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
							// 초기위치로 세팅 : 센터 + 방향 위치로 세팅
							m_vecParticleInfoDesc[i].vCenterPositions.y = m_tBufferDesc.vMinMaxPosY.x;
							XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions) + m_vecParticleInfoDesc[i].vDir);

							m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };
							pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;



							// 시간 초기화
							m_vecParticleInfoDesc[i].Reset_ParticleTimes();
						}
						else
						{
							m_vecParticleInfoDesc[i].bDie = TRUE;
						}
					}

				}
				else if (TORNADO == m_tBufferDesc.eType_Action)
				{
					_float fTheta = m_vecParticleInfoDesc[i].fCurTheta + fTimeDelta * m_vecParticleInfoDesc[i].fCurTornadoSpeed; // 초기 각도에 시간과 회전 속도를 곱하여 갱신된 각도를 얻는다.

					_float fNewPosX = m_vecParticleInfoDesc[i].vCenterPositions.x + ((m_vecParticleInfoDesc[i].fMaxRange + (m_vecParticleInfoDesc[i].fAddRange * m_vecParticleInfoDesc[i].fLifeTimeRatios))) * cos(fTheta); // 중심 x좌표에 반지름 * cos(theta)를 더한다.
					_float fNewPosZ = m_vecParticleInfoDesc[i].vCenterPositions.z + ((m_vecParticleInfoDesc[i].fMaxRange + (m_vecParticleInfoDesc[i].fAddRange * m_vecParticleInfoDesc[i].fLifeTimeRatios))) * sin(fTheta); // 중심 z좌표에 반지름 * sin(theta)를 더한다.


					pVertices[i].vPosition.x = fNewPosX;
					pVertices[i].vPosition.z = fNewPosZ;
					pVertices[i].vPosition.y += m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta;


					// 각 입자의 현재 각도를 업데이트
					m_vecParticleInfoDesc[i].fCurTheta = fTheta;
					m_vecParticleInfoDesc[i].vDir = { pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z };
					//Update_Dir_Rotation(i);	// 쉐이더에 던져줄 라업룩 계산
					Update_Spark_Rotation(i);


					// 알파_토네이도 (높이)
					_float		fAlpha = 0.f;
					if (FADE_NONE == m_tBufferDesc.eType_Fade)
					{
						fAlpha = 1.f;
					}
					else if (FADE_OUT == m_tBufferDesc.eType_Fade)
					{
						fAlpha = max(m_vecParticleInfoDesc[i].fMaxPosY - pVertices[i].vPosition.y, 0.f);
					}
					else if (FADE_IN == m_tBufferDesc.eType_Fade)
					{
						fAlpha = min(pVertices[i].vPosition.y, 1.f);
					}
					m_vecParticleShaderInfoDesc[i].vCurrentColors.w = fAlpha;
					pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;


					if (m_vecParticleInfoDesc[i].fMaxPosY <= pVertices[i].vPosition.y)	// 현재 y위치가 최대 범위보다 크면 초기화 or 죽음
					{
						if (m_tBufferDesc.bRecycle)	// 재사용이 true이면 초기화
						{
							// 랜덤 값 다시 뽑기
							ReSet_Info(i);
							m_vecParticleInfoDesc[i].bEmit = TRUE;


							// 안보이게
	
							m_vecParticleShaderInfoDesc[i].vRight = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vUp = _float3{ 0.f, 0.f, 0.f };
							m_vecParticleShaderInfoDesc[i].vLook = _float3{ 0.f, 0.f, 0.f };

							pVertices[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
							pVertices[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
							pVertices[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };
							// 초기위치로 세팅 : 센터 + 방향 위치로 세팅
							m_vecParticleInfoDesc[i].vCenterPositions.y = m_tBufferDesc.vMinMaxPosY.x;
							XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions) + m_vecParticleInfoDesc[i].vDir);

							m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };
							pVertices[i].vColor.w = m_vecParticleShaderInfoDesc[i].vCurrentColors.w;


							// 시간 초기화
							m_vecParticleInfoDesc[i].Reset_ParticleTimes();
						}
						else
						{
							m_vecParticleInfoDesc[i].bDie = TRUE;
						}

					}


				}


			}
#pragma endregion 이동 : 직접 이동 끝


#pragma region 크기 러프 시작
			if (m_tBufferDesc.bUseScaleLerp) // 크기 변경 러프 사용이면
			{
				if (m_tBufferDesc.fLifeTimeRatio >= m_tBufferDesc.vScaleLerp_Up_Pos.x)		// 0~1로 보간한 라이프타임이 크기 증가를 시작할 타임 포지션을 넘어가면
				{
					if (m_tBufferDesc.fLifeTimeRatio >= m_tBufferDesc.vScaleLerp_Down_Pos.x) // 0~1로 보간한 라이프타임이 크기 감소를 시작할 타임 포지션도 넘어가면 감소 시작
					{
						// 크기 감소를 시작할 타임 포지션 (크기 0이 목표)
						_float fTotalTime = m_tBufferDesc.vMinMaxLifeTime.y * (1.f - m_tBufferDesc.vScaleLerp_Down_Pos.x);	// 라이프 타임 중, 감소에만 필요한 토탈시간 계산
						if (m_vecParticleInfoDesc[i].fDownScaleTimeAccs >= fTotalTime)
						{
							m_vecParticleInfoDesc[i].fDownScaleTimeAccs = fTotalTime;
							m_vecParticleInfoDesc[i].vCurScales.x = 0.f;
							m_vecParticleInfoDesc[i].vCurScales.y = 0.f;
						}
						else
						{
							m_vecParticleInfoDesc[i].fDownScaleTimeAccs += fTimeDelta;	// 시간 누적	
							m_vecParticleInfoDesc[i].vCurScales.x = abs(Easing::LerpToType(m_vecParticleInfoDesc[i].vMaxScales.x, 0.f, m_vecParticleInfoDesc[i].fDownScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
							m_vecParticleInfoDesc[i].vCurScales.y = abs(Easing::LerpToType(m_vecParticleInfoDesc[i].vMaxScales.y, 0.f, m_vecParticleInfoDesc[i].fDownScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
						}
					}
					else
					{
						// Max크기가 목표
						_float fTotalTime = m_tBufferDesc.vMinMaxLifeTime.y * (m_tBufferDesc.vScaleLerp_Up_Pos.y - m_tBufferDesc.vScaleLerp_Up_Pos.x);	// 라이프 타임 중, 증가에만 필요한 토탈시간 계산
						if (m_vecParticleInfoDesc[i].fUpScaleTimeAccs >= fTotalTime)
						{
							m_vecParticleInfoDesc[i].fUpScaleTimeAccs = fTotalTime;
							m_vecParticleInfoDesc[i].vCurScales.x = m_vecParticleInfoDesc[i].vMaxScales.x;
							m_vecParticleInfoDesc[i].vCurScales.y = m_vecParticleInfoDesc[i].vMaxScales.y;
						}
						else
						{
							m_vecParticleInfoDesc[i].fUpScaleTimeAccs += fTimeDelta;	// 시간 누적		
							m_vecParticleInfoDesc[i].vCurScales.x = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxWidth.x, m_vecParticleInfoDesc[i].vMaxScales.x, m_vecParticleInfoDesc[i].fUpScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
							m_vecParticleInfoDesc[i].vCurScales.y = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxHeight.x, m_vecParticleInfoDesc[i].vMaxScales.y, m_vecParticleInfoDesc[i].fUpScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
						}

					}

				}
			}

			// 크기변경 적용
			pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.x;
			pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.y;

#pragma region 크기 러프 끝





#pragma region 색 변경 시작

			if (m_tBufferDesc.bDynamic_Color)	// 입자마다 다른 주기로 색 변경
			{
				m_vecParticleShaderInfoDesc[i].vCurrentColors.x = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_ColorLerp));
				m_vecParticleShaderInfoDesc[i].vCurrentColors.y = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_ColorLerp));
				m_vecParticleShaderInfoDesc[i].vCurrentColors.z = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_ColorLerp));

				// 쉐이더에 던질 곱하기 색상 값 저장
				//m_vecParticleShaderInfoDesc[i].vCurrentColors = m_vecParticleInfoDesc[i].vCurrentColors;
				pVertices[i].vColor = m_vecParticleShaderInfoDesc[i].vCurrentColors;
			}
			else // 일괄 색 변경
			{
				m_tBufferDesc.vCurrentColor.x = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y, m_tBufferDesc.fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));
				m_tBufferDesc.vCurrentColor.y = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y, m_tBufferDesc.fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));
				m_tBufferDesc.vCurrentColor.z = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y, m_tBufferDesc.fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));

				// 쉐이더에 던질 곱하기 색상 값 저장
				m_vecParticleShaderInfoDesc[i].vCurrentColors = m_tBufferDesc.vCurrentColor;
				pVertices[i].vColor = m_tBufferDesc.vCurrentColor;
			}


#pragma region 색 변경 끝
		}





	} // 반복문 끝

	m_pContext->Unmap(m_pVBInstance, 0);

}

_float3 CVIBuffer_Particle::Update_Kinetic(_uint iNum, _float fTimeDelta)
{
	/* 중력 사용 시 아래로 떨어짐 */
	if (m_tBufferDesc.bUseGravity)
		m_vecParticleRigidbodyDesc[iNum].vVelocity.y += m_tBufferDesc.fGravity * fTimeDelta;

	/* V += A * TD */
	/* m_vVelocity += m_vAccel * fTimeDelta; */
	XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity) += XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vAccel) * fTimeDelta);


	/* 마찰력에 의한 반대방향으로의 가속도(감속) */
	if (1.f > m_vecParticleRigidbodyDesc[iNum].fFriction)
	{
		/* (m_vVelocity * (1.f - m_fFriction)) */
		XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity) * (1.f - m_vecParticleRigidbodyDesc[iNum].fFriction));
	}
	else
	{
		m_vecParticleRigidbodyDesc[iNum].vVelocity = { 0.f, 0.f, 0.f };
	}


	/* 잠긴 축에 대한 힘 제거 */
	if (m_tBufferDesc.byFreezeAxis)
	{
		for (_int i = 0; i < 3; ++i)
		{
			if (m_tBufferDesc.byFreezeAxis & 1 << i) // 특정 비트가 1인지 확인(1이면 잠긴 축?)
			{
				*((_float*)&m_vecParticleRigidbodyDesc[iNum].vVelocity + i) = 0.f;
			}
		}
	}

	return m_vecParticleRigidbodyDesc[iNum].vVelocity;
}

void CVIBuffer_Particle::Update_Kinematic(_uint iNum)
{
	/* 질량 무시, 직접 속도 변화 부여 (키네마틱, 즉 속도 계산안함) */
	Clear_Force(iNum, FORCE_MODE::VELOCITY_CHANGE);
}


void CVIBuffer_Particle::Add_Force(_uint iNum, _fvector vForce, FORCE_MODE eMode)
{
	switch (eMode)
	{
	case FORCE_MODE::FORCE:
		XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vAccel, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vAccel) += vForce / m_vecParticleRigidbodyDesc[iNum].fMass);
		break;

	case FORCE_MODE::IMPULSE:
		XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity) += vForce / m_vecParticleRigidbodyDesc[iNum].fMass);
		break;

	case FORCE_MODE::ACCELERATION:
		XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vAccel, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vAccel) += vForce);
		break;

	case FORCE_MODE::VELOCITY_CHANGE:
		XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity) += vForce);
		break;

	default:
		break;
	}

	Wake(iNum);
}

void CVIBuffer_Particle::Clear_Force(_uint iNum, const FORCE_MODE& eMode)
{
	switch (eMode)
	{
	case FORCE_MODE::FORCE:
		::ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vAccel, sizeof(_float3));
		break;

	case FORCE_MODE::IMPULSE:
		::ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vVelocity, sizeof(_float3));
		break;

	case FORCE_MODE::ACCELERATION:
		::ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vAccel, sizeof(_float3));
		break;

	case FORCE_MODE::VELOCITY_CHANGE:
		::ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vVelocity, sizeof(_float3));
		break;

	default:
		break;
	}
}


void CVIBuffer_Particle::Clear_Power(_uint iNum)
{
	ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vAccel, sizeof(_float3));
	ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vVelocity, sizeof(_float3));
}


const _bool CVIBuffer_Particle::Check_Sleep(_uint iNum)
{
	if (m_vecParticleRigidbodyDesc[iNum].bSleep)
		return TRUE;

	//if (m_tBufferDesc.bUseGravity)
	//{
	//	if (m_tBufferDesc.bRecycle)
	//	{
	//		_float2 vVelocityXZ = { m_vecParticleRigidbodyDesc[iNum].vVelocity.x, m_vecParticleRigidbodyDesc[iNum].vVelocity.z };
	//		_float fLengthXZ = XMVectorGetX(XMVector2Length(XMLoadFloat2(&vVelocityXZ)));

	//		if (m_tBufferDesc.fSleepThreshold > fLengthXZ)
	//		{
	//			Sleep(iNum);
	//			return TRUE;
	//		}
	//	}

	//}
	//else
	{
		_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity)));

		if (m_tBufferDesc.fSleepThreshold > fLength)
		{
			Sleep(iNum);
			return TRUE;
		}
	}


	///* 선형 속도의 크기가 m_fSleepThreshold보다 작으면 슬립(연산안함) */
	//_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity)));
	//if (!m_tBufferDesc.bUseGravity && m_tBufferDesc.fSleepThreshold > fLength)
	//{
	//	Sleep(iNum);
	//	return TRUE;
	//}

	return FALSE;
}


_bool CVIBuffer_Particle::Write_Json(json& Out_Json)
{
	Out_Json["Com_VIBuffer"]["iCurNumInstance"] = m_tBufferDesc.iCurNumInstance;


	Out_Json["Com_VIBuffer"]["bRecycle"] = m_tBufferDesc.bRecycle;
	Out_Json["Com_VIBuffer"]["bReverse"] = m_tBufferDesc.bReverse;
	Out_Json["Com_VIBuffer"]["eType_Action"] = m_tBufferDesc.eType_Action;
	Out_Json["Com_VIBuffer"]["eType_Fade"] = m_tBufferDesc.eType_Fade;
	
	/* LifeTime */
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxLifeTime"], m_tBufferDesc.vMinMaxLifeTime);

	/* Emitter */
	Out_Json["Com_VIBuffer"]["fEmissionTime"] = m_tBufferDesc.fEmissionTime;
	Out_Json["Com_VIBuffer"]["iAddEmitCount"] = m_tBufferDesc.iAddEmitCount;


	/* RigidBody */
	Out_Json["Com_VIBuffer"]["bUseRigidBody"] = m_tBufferDesc.bUseRigidBody;
	Out_Json["Com_VIBuffer"]["bKinetic"] = m_tBufferDesc.bKinetic;
	Out_Json["Com_VIBuffer"]["bUseGravity"] = m_tBufferDesc.bUseGravity;
	Out_Json["Com_VIBuffer"]["eForce_Mode"] = m_tBufferDesc.eForce_Mode;

	Out_Json["Com_VIBuffer"]["fGravity"] = m_tBufferDesc.fGravity;
	Out_Json["Com_VIBuffer"]["eType_FrictionLerp"] = m_tBufferDesc.eType_FrictionLerp;
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vFrictionLerp_Pos"], m_tBufferDesc.vFrictionLerp_Pos);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vStartEnd_Friction"], m_tBufferDesc.vStartEnd_Friction);

	Out_Json["Com_VIBuffer"]["fSleepThreshold"] = m_tBufferDesc.fSleepThreshold;
	Out_Json["Com_VIBuffer"]["byFreezeAxis"] = m_tBufferDesc.byFreezeAxis;

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxPower"], m_tBufferDesc.vMinMaxPower);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxSpeed"], m_tBufferDesc.vMinMaxSpeed);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxTornadoSpeed"], m_tBufferDesc.vMinMaxTornadoSpeed); // 토네이도 회전 스피드
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxMass"], m_tBufferDesc.vMinMaxMass);

	/* For.Position */
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vMinCenterOffsetPos"], m_tBufferDesc.vMinCenterOffsetPos);
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vMaxCenterOffsetPos"], m_tBufferDesc.vMaxCenterOffsetPos);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxAddRange"], m_tBufferDesc.vMinMaxAddRange);


	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxPosY"], m_tBufferDesc.vMinMaxPosY);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxTheta"], m_tBufferDesc.vMinMaxTheta);

	/* For.Rotation */
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vRadian"], m_tBufferDesc.vRadian);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);


	/* For.Scale */
	Out_Json["Com_VIBuffer"]["bUseScaleLerp"] = m_tBufferDesc.bUseScaleLerp;
	Out_Json["Com_VIBuffer"]["eType_ScaleLerp"] = m_tBufferDesc.eType_ScaleLerp;
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vScaleLerp_Up_Pos"], m_tBufferDesc.vScaleLerp_Up_Pos);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vScaleLerp_Down_Pos"], m_tBufferDesc.vScaleLerp_Down_Pos);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxWidth"], m_tBufferDesc.vMinMaxWidth);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxHeight"], m_tBufferDesc.vMinMaxHeight);



	/* For.Color */
	Out_Json["Com_VIBuffer"]["eType_ColorLerp"] = m_tBufferDesc.eType_ColorLerp;
	Out_Json["Com_VIBuffer"]["bDynamic_Color"] = m_tBufferDesc.bDynamic_Color;
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRed"], m_tBufferDesc.vMinMaxRed);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxGreen"], m_tBufferDesc.vMinMaxGreen);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxBlue"], m_tBufferDesc.vMinMaxBlue);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxAlpha"], m_tBufferDesc.vMinMaxAlpha);


	return true;
}

void CVIBuffer_Particle::Load_FromJson(const json& In_Json)
{
	m_tBufferDesc.iCurNumInstance = In_Json["Com_VIBuffer"]["iCurNumInstance"];


	m_tBufferDesc.bRecycle = In_Json["Com_VIBuffer"]["bRecycle"];
	m_tBufferDesc.bReverse = In_Json["Com_VIBuffer"]["bReverse"];
	m_tBufferDesc.eType_Action = In_Json["Com_VIBuffer"]["eType_Action"];
	m_tBufferDesc.eType_Fade = In_Json["Com_VIBuffer"]["eType_Fade"];


	/* LifeTime */
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxLifeTime"], m_tBufferDesc.vMinMaxLifeTime);


	/* Emitter */
	if (In_Json["Com_VIBuffer"].contains("fEmissionTime")) // 다시 저장 후 삭제
	{
		m_tBufferDesc.fEmissionTime = In_Json["Com_VIBuffer"]["fEmissionTime"];
		m_tBufferDesc.iAddEmitCount = In_Json["Com_VIBuffer"]["iAddEmitCount"];
	}
	

	/* RigidBody */
	m_tBufferDesc.bUseRigidBody = In_Json["Com_VIBuffer"]["bUseRigidBody"];
	m_tBufferDesc.bKinetic = In_Json["Com_VIBuffer"]["bKinetic"];
	m_tBufferDesc.bUseGravity = In_Json["Com_VIBuffer"]["bUseGravity"];
	m_tBufferDesc.eForce_Mode = In_Json["Com_VIBuffer"]["eForce_Mode"];

	m_tBufferDesc.fGravity = In_Json["Com_VIBuffer"]["fGravity"];
	//m_tBufferDesc.fFriction = In_Json["Com_VIBuffer"]["fFriction"];

	if (In_Json["Com_VIBuffer"].contains("eType_FrictionLerp")) // 다시 저장 후 삭제
	{
		m_tBufferDesc.eType_FrictionLerp = In_Json["Com_VIBuffer"]["eType_FrictionLerp"];
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vFrictionLerp_Pos"], m_tBufferDesc.vFrictionLerp_Pos);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vStartEnd_Friction"], m_tBufferDesc.vStartEnd_Friction);
	}


	m_tBufferDesc.fSleepThreshold = In_Json["Com_VIBuffer"]["fSleepThreshold"];
	m_tBufferDesc.byFreezeAxis = In_Json["Com_VIBuffer"]["byFreezeAxis"];

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxPower"], m_tBufferDesc.vMinMaxPower);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxSpeed"], m_tBufferDesc.vMinMaxSpeed);

	if (In_Json["Com_VIBuffer"].contains("vMinMaxTornadoSpeed")) // 다시 저장 후 삭제
	{
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxTornadoSpeed"], m_tBufferDesc.vMinMaxTornadoSpeed);
	}

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxMass"], m_tBufferDesc.vMinMaxMass);

	/* For.Position */
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vMinCenterOffsetPos"], m_tBufferDesc.vMinCenterOffsetPos);
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vMaxCenterOffsetPos"], m_tBufferDesc.vMaxCenterOffsetPos);

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);


	if (In_Json["Com_VIBuffer"].contains("vMinMaxAddRange")) // 다시 저장 후 삭제
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxAddRange"], m_tBufferDesc.vMinMaxAddRange);


	if (In_Json["Com_VIBuffer"].contains("vMinMaxTheta")) // 다시 저장 후 삭제
	{
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxTheta"], m_tBufferDesc.vMinMaxTheta);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxPosY"], m_tBufferDesc.vMinMaxPosY);
	}


	/* For.Rotation */
	if (In_Json["Com_VIBuffer"].contains("vRadian")) // 다시 저장 후 삭제
		CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vRadian"], m_tBufferDesc.vRadian);

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);


	/* For.Scale */
	if (In_Json["Com_VIBuffer"].contains("bUseScaleLerp")) // 다시 저장 후 삭제
		m_tBufferDesc.bUseScaleLerp = In_Json["Com_VIBuffer"]["bUseScaleLerp"];

	m_tBufferDesc.eType_ScaleLerp = In_Json["Com_VIBuffer"]["eType_ScaleLerp"];

	if (In_Json["Com_VIBuffer"].contains("vScaleLerp_Up_Pos")) // 다시 저장 후 삭제
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vScaleLerp_Up_Pos"], m_tBufferDesc.vScaleLerp_Up_Pos);

	if (In_Json["Com_VIBuffer"].contains("vScaleLerp_Down_Pos")) // 다시 저장 후 삭제
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vScaleLerp_Down_Pos"], m_tBufferDesc.vScaleLerp_Down_Pos);

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxWidth"], m_tBufferDesc.vMinMaxWidth);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxHeight"], m_tBufferDesc.vMinMaxHeight);


	/* For.Color */
	m_tBufferDesc.eType_ColorLerp = In_Json["Com_VIBuffer"]["eType_ColorLerp"];
	m_tBufferDesc.bDynamic_Color = In_Json["Com_VIBuffer"]["bDynamic_Color"];
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRed"], m_tBufferDesc.vMinMaxRed);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxGreen"], m_tBufferDesc.vMinMaxGreen);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxBlue"], m_tBufferDesc.vMinMaxBlue);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxAlpha"], m_tBufferDesc.vMinMaxAlpha);


	//// vTest가 있으면 로드하기
	//if (In_Json.contains("vTest"))
	//	m_tBufferDesc.vTest = In_Json["Com_VIBuffer"]["vTest"];

}

CVIBuffer_Particle* CVIBuffer_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Particle* pInstance = new CVIBuffer_Particle(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Particle::Clone(void* pArg)
{
	CVIBuffer_Particle* pInstance = new CVIBuffer_Particle(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Particle::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}