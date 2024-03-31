#include "VIBuffer_Effect_Model_Instance.h"
#include "Mesh.h"

#include "SMath.h"
#include "Easing_Utillity.h"

CVIBuffer_Effect_Model_Instance::CVIBuffer_Effect_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Model_Instance(pDevice,pContext)
{
}

CVIBuffer_Effect_Model_Instance::CVIBuffer_Effect_Model_Instance(const CVIBuffer_Effect_Model_Instance& rhs)
	: CVIBuffer_Model_Instance(rhs)
{
}

HRESULT CVIBuffer_Effect_Model_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Effect_Model_Instance::Initialize(void* pArg)
{
	m_tBufferDesc = *(EFFECT_MODEL_INSTANCE_DESC*)pArg;


	CModel* pModel[MORPH_END] = { nullptr };
	for (_uint i = 0; i < ECast<_uint>(MORPH_END); ++i)
	{	
		if (nullptr != m_tBufferDesc.pModel[i])
		{
			Safe_AddRef(m_tBufferDesc.pModel[i]);
			pModel[i] = m_tBufferDesc.pModel[i];
		}
	}
	
	vector<CMesh*> Meshes[MORPH_END];
	for (_uint i = 0; i < ECast<_uint>(MORPH_END); ++i)
	{
		if (nullptr != pModel[i])
		{
			Meshes[i] = pModel[i]->Get_Meshes();
			m_iNumMeshes = (_int)Meshes[i].size();

			for (_int j = 0; j < m_iNumMeshes; ++j)
			{
				m_vecInstanceMesh.push_back(Meshes[i][j]);
				Safe_AddRef(Meshes[i][j]);
			}

			m_iNumMaterials = pModel[i]->Get_NumMaterials();
		}
	}


	// 벡터 공간 예약(파티클모드일때만)
	if (MODE_PARTICLE == m_tBufferDesc.eType_Mode)
	{
		m_vecParticleInfoDesc.reserve(m_tBufferDesc.iCurNumInstance);
		m_vecParticleShaderInfoDesc.reserve(m_tBufferDesc.iCurNumInstance);

		if (m_tBufferDesc.bUseRigidBody)
			m_vecParticleRigidbodyDesc.reserve(m_tBufferDesc.iCurNumInstance);
	}


	// 시간 초기화
	m_tBufferDesc.Reset_Times();


	// 방출 초기화
	m_tBufferDesc.bEmitFinished = FALSE;
	m_tBufferDesc.fEmissionTimeAcc = 0.f;
	m_tBufferDesc.iEmitCount = 0;


	// ==============================================================================================

	Init_Instance(m_tBufferDesc.iCurNumInstance);


	return S_OK;
}


void CVIBuffer_Effect_Model_Instance::Init_Instance(_int iNumInstance)
{
	m_iInstanceStride = sizeof(VTXMODELINSTANCE);
	m_iNumInstance = iNumInstance;
	m_iNumVertexBuffers = 2;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMODELINSTANCE* pModelInstance = new VTXMODELINSTANCE[m_iNumInstance];


	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; ++i) // 반복문 시작
	{

		// 파티클 모드일때만
		if (MODE_PARTICLE == m_tBufferDesc.eType_Mode)
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


			// 방출 초기화
			m_vecParticleInfoDesc[i].bEmit = FALSE;


			// 랜덤 값 뽑기
			ReSet_ParticleInfo(i);


			// 안보이게
			{
				m_vecParticleShaderInfoDesc[i].vRight = { 0.f, 0.f, 0.f, 0.f };
				m_vecParticleShaderInfoDesc[i].vUp = { 0.f, 0.f, 0.f, 0.f };
				m_vecParticleShaderInfoDesc[i].vLook = { 0.f, 0.f, 0.f, 0.f };

				pModelInstance[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x	, m_vecParticleShaderInfoDesc[i].vRight.y	, m_vecParticleShaderInfoDesc[i].vRight.z	, 0.f };
				pModelInstance[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x		, m_vecParticleShaderInfoDesc[i].vUp.y		, m_vecParticleShaderInfoDesc[i].vUp.z		, 0.f };
				pModelInstance[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x	, m_vecParticleShaderInfoDesc[i].vLook.y	, m_vecParticleShaderInfoDesc[i].vLook.z	, 0.f };


				// 초기 위치 세팅
				XMStoreFloat4(&pModelInstance[i].vTranslation, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions));

			}

		}
		else
		{
			// 파티클 모드가 아니면
			pModelInstance[i].vRight	= _float4(1.f, 0.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.x;
			pModelInstance[i].vUp		= _float4(0.f, 1.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.y;
			pModelInstance[i].vLook		= _float4(0.f, 0.f, 1.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.z;

			// 원점 위치로 고정
			//XMStoreFloat4(&pModelInstance[i].vTranslation, m_tBufferDesc.vCenterPosition);
			XMStoreFloat4(&pModelInstance[i].vTranslation, m_vecParticleInfoDesc[i].vCenterPositions);
		}



	} // 반복문 끝

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pModelInstance;

	Create_Buffer(&m_pVBInstance);

	Safe_Delete_Array(pModelInstance);

}

void CVIBuffer_Effect_Model_Instance::ReSet()
{
	// 시간 초기화
	m_tBufferDesc.Reset_Times();

	// 방출 초기화
	m_tBufferDesc.bEmitFinished = FALSE;
	m_tBufferDesc.fEmissionTimeAcc = 0.f;
	m_tBufferDesc.iEmitCount = 0;

	// ==============================================================================================

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pModelInstance = ((VTXMODELINSTANCE*)SubResource.pData);

	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; i++)	// 반복문 시작
	{

		// 파티클 모드일때만
		if (MODE_PARTICLE == m_tBufferDesc.eType_Mode)
		{
			// 방출 초기화
			m_vecParticleInfoDesc[i].bEmit = FALSE;


			// 랜덤 값 뽑기
			ReSet_ParticleInfo(i);


			// 안보이게
			{
				m_vecParticleShaderInfoDesc[i].vRight	= { 0.f, 0.f, 0.f, 0.f };
				m_vecParticleShaderInfoDesc[i].vUp		= { 0.f, 0.f, 0.f, 0.f };
				m_vecParticleShaderInfoDesc[i].vLook	= { 0.f, 0.f, 0.f, 0.f };

				pModelInstance[i].vRight	= _float4{ m_vecParticleShaderInfoDesc[i].vRight.x	, m_vecParticleShaderInfoDesc[i].vRight.y	, m_vecParticleShaderInfoDesc[i].vRight.z	, 0.f };
				pModelInstance[i].vUp		= _float4{ m_vecParticleShaderInfoDesc[i].vUp.x		, m_vecParticleShaderInfoDesc[i].vUp.y		, m_vecParticleShaderInfoDesc[i].vUp.z		, 0.f };
				pModelInstance[i].vLook		= _float4{ m_vecParticleShaderInfoDesc[i].vLook.x	, m_vecParticleShaderInfoDesc[i].vLook.y	, m_vecParticleShaderInfoDesc[i].vLook.z	, 0.f };


				// 초기 위치 세팅
				XMStoreFloat4(&pModelInstance[i].vTranslation, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions));

			}

		}
		else
		{
			// 파티클 모드가 아니면
			pModelInstance[i].vRight	= _float4(1.f, 0.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.x;
			pModelInstance[i].vUp		= _float4(0.f, 1.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.y;
			pModelInstance[i].vLook		= _float4(0.f, 0.f, 1.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.z;

			// 원점 위치로 고정
			//XMStoreFloat4(&pModelInstance[i].vTranslation, m_tBufferDesc.vCenterPosition);
			XMStoreFloat4(&pModelInstance[i].vTranslation, m_vecParticleInfoDesc[i].vCenterPositions);
		}

		


	} // 반복문 끝

	m_pContext->Unmap(m_pVBInstance, 0);
}



void CVIBuffer_Effect_Model_Instance::ReSet_ParticleInfo(_uint iNum)
{
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


	// 센터 위치 
	m_vecParticleInfoDesc[iNum].vCenterPositions.x = SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.x, m_tBufferDesc.vMaxCenterOffsetPos.x);
	m_vecParticleInfoDesc[iNum].vCenterPositions.y = SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.y, m_tBufferDesc.vMaxCenterOffsetPos.y);
	m_vecParticleInfoDesc[iNum].vCenterPositions.z = SMath::fRandom(m_tBufferDesc.vMinCenterOffsetPos.z, m_tBufferDesc.vMaxCenterOffsetPos.z);
	m_vecParticleInfoDesc[iNum].vCenterPositions.w = 1.f;



	// 크기
	if (m_tBufferDesc.bUseScaleLerp)
	{
		// 현재 크기는 시작 크기로
		m_vecParticleInfoDesc[iNum].vCurScales.x = m_tBufferDesc.vStartScale.x;
		m_vecParticleInfoDesc[iNum].vCurScales.y = m_tBufferDesc.vStartScale.y;
		m_vecParticleInfoDesc[iNum].vCurScales.z = m_tBufferDesc.vStartScale.z;

		// 스케일 러프 사용이면 최대 크기 고정
		m_vecParticleInfoDesc[iNum].vMaxScales.x = m_tBufferDesc.vEndScale.x;
		m_vecParticleInfoDesc[iNum].vMaxScales.y = m_tBufferDesc.vEndScale.y;
		m_vecParticleInfoDesc[iNum].vMaxScales.z = m_tBufferDesc.vEndScale.z;
	}
	else
	{
		// 스케일 러프 사용이 아니면 현재 스케일은 범위 내 랜덤
		if (m_tBufferDesc.bScaleRatio)
		{
			// 정비율 (가로 세로 깊이 사이즈 다 같음)
			m_vecParticleInfoDesc[iNum].vCurScales.x = SMath::fRandom(m_tBufferDesc.vStartScale.x, m_tBufferDesc.vEndScale.x);
			m_vecParticleInfoDesc[iNum].vCurScales.y = m_vecParticleInfoDesc[iNum].vCurScales.x; 
			m_vecParticleInfoDesc[iNum].vCurScales.z = m_vecParticleInfoDesc[iNum].vCurScales.x; 
		}
		else
		{
			// 비율 신경 안씀
			m_vecParticleInfoDesc[iNum].vCurScales.x = SMath::fRandom(m_tBufferDesc.vStartScale.x, m_tBufferDesc.vEndScale.x);
			m_vecParticleInfoDesc[iNum].vCurScales.y = SMath::fRandom(m_tBufferDesc.vStartScale.y, m_tBufferDesc.vEndScale.y);
			m_vecParticleInfoDesc[iNum].vCurScales.z = SMath::fRandom(m_tBufferDesc.vStartScale.z, m_tBufferDesc.vEndScale.z);
		}

	}


#pragma region 리지드바디 시작
	// 리지드 바디 사용이면
	if (m_tBufferDesc.bUseRigidBody)
	{
		m_vecParticleRigidbodyDesc[iNum].bForced = FALSE; // 힘 안준걸로 초기화

		Clear_Power(iNum);	// 파워 리셋
	}
#pragma endregion 리지드바디 끝



	// 센터 포지션 & Offset
	if (TORNADO != m_tBufferDesc.eType_Action)
	{
		// 토네이도가 아닐때만 범위 오프셋 주기(토네이도는 다른 의도로 멤버변수가 쓰임)
		_vector		vAddRangePos = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		vAddRangePos = XMVector3Normalize(vAddRangePos) * m_vecParticleInfoDesc[iNum].fAddRange;

		_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_vecParticleInfoDesc[iNum].vOffsetTheta.x, m_vecParticleInfoDesc[iNum].vOffsetTheta.y, m_vecParticleInfoDesc[iNum].vOffsetTheta.z);
		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		vAddRangePos = XMVector3TransformNormal(vAddRangePos, RotationMatrix);	// 벡터 회전 적용

		m_vecParticleInfoDesc[iNum].vCenterPositions.x += XMVectorGetX(vAddRangePos);
		m_vecParticleInfoDesc[iNum].vCenterPositions.y += XMVectorGetY(vAddRangePos);
		m_vecParticleInfoDesc[iNum].vCenterPositions.z += XMVectorGetZ(vAddRangePos);
	}


	// 방향 만들기
	m_vecParticleInfoDesc[iNum].vDir = Make_Dir(iNum);	// 방향 저장

	if (m_tBufferDesc.bReverse) // 리버스면 센터위치는 센터 + 길이를 가진 방향
	{
		m_vecParticleInfoDesc[iNum].vCenterPositions += m_vecParticleInfoDesc[iNum].vDir;
		m_vecParticleInfoDesc[iNum].vCenterPositions.w = 1.f;

		m_vecParticleInfoDesc[iNum].vDir = XMVector3Normalize(m_vecParticleInfoDesc[iNum].vDir) * -1.f; // 방향벡터 노멀라이즈 + 반대방향으로 만들기
	}
	else
	{
		m_vecParticleInfoDesc[iNum].vDir = XMVector3Normalize(m_vecParticleInfoDesc[iNum].vDir);
	}


	// 색 초기화
	m_vecParticleShaderInfoDesc[iNum].vCurrentColors = { m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxAlpha.x };



	// 알파 초기화
	_float		fAlpha = 1.f;
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


	// 알파 가중치
	m_vecParticleInfoDesc[iNum].fAddAlpha = SMath::fRandom(m_tBufferDesc.vMinMaxAlpha.x, m_tBufferDesc.vMinMaxAlpha.y);
	m_vecParticleShaderInfoDesc[iNum].fCurAddAlpha = m_vecParticleInfoDesc[iNum].fAddAlpha;

}

_float4 CVIBuffer_Effect_Model_Instance::Make_Dir(_uint iNum)
{
	_vector		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	vDir = XMVector3Normalize(vDir) * m_vecParticleInfoDesc[iNum].fMaxRange;

	_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_vecParticleInfoDesc[iNum].vOffsetTheta.x, m_vecParticleInfoDesc[iNum].vOffsetTheta.y, m_vecParticleInfoDesc[iNum].vOffsetTheta.z);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	vDir = XMVector3TransformNormal(vDir, RotationMatrix);	// 가야할 방향벡터 회전 적용

	return vDir;
}

void CVIBuffer_Effect_Model_Instance::Rotation_Instance(_uint iNum)
{

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f)	* m_vecParticleInfoDesc[iNum].vCurScales.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * m_vecParticleInfoDesc[iNum].vCurScales.z;


	_vector		vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_tBufferDesc.vRadian.x)
															, XMConvertToRadians(m_tBufferDesc.vRadian.y)
															, XMConvertToRadians(m_tBufferDesc.vRadian.z));

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);


	// 쉐이더에 던질 값(라업룩) 저장 
	m_vecParticleShaderInfoDesc[iNum].vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	m_vecParticleShaderInfoDesc[iNum].vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	m_vecParticleShaderInfoDesc[iNum].vLook = XMVector3TransformNormal(vLook, RotationMatrix);

}

void CVIBuffer_Effect_Model_Instance::Update_DirToAxis(_uint iNum)
{
	if (DIR_RIGHT == m_tBufferDesc.eType_Dir)
	{
		Make_DirToRight(iNum);
	}
	else if (DIR_UP == m_tBufferDesc.eType_Dir)
	{
		Make_DirToUp(iNum);
	}
	else if (DIR_LOOK == m_tBufferDesc.eType_Dir)
	{
		Make_DirToLook(iNum);
	}


	// 추가 회전
	Rotation_Instance(iNum);
}

void CVIBuffer_Effect_Model_Instance::Make_DirToRight(_uint iNum)
{
	_vector		vRight, vUp, vLook;

	// 이동 진행 방향벡터를 Right로 한 새로운 Up, Look 정해주기 ===================================
	vRight = XMVector4Normalize(m_vecParticleInfoDesc[iNum].vDir) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	vUp = XMVector3Normalize(XMVector3Cross(float4(0.f, 0.f, 1.f, 0.f), vRight)) * m_vecParticleInfoDesc[iNum].vCurScales.y;
	vLook = XMVector4Normalize(XMVector3Cross(vRight, vUp)) * m_vecParticleInfoDesc[iNum].vCurScales.z;
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * m_vecParticleInfoDesc[iNum].vCurScales.y;

	// 저장
	m_vecParticleShaderInfoDesc[iNum].vRight = vRight;
	m_vecParticleShaderInfoDesc[iNum].vUp = vUp;
	m_vecParticleShaderInfoDesc[iNum].vLook = vLook;
}


void CVIBuffer_Effect_Model_Instance::Make_DirToUp(_uint iNum)
{
	_vector		vRight, vUp, vLook;

	// 이동 진행 방향벡터를 Right로 한 새로운 Up, Look 정해주기 ===================================
	vRight = XMVector4Normalize(m_vecParticleInfoDesc[iNum].vDir) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	vUp = XMVector3Normalize(XMVector3Cross(float4(0.f, 0.f, 1.f, 0.f), vRight)) * m_vecParticleInfoDesc[iNum].vCurScales.y;
	vLook = XMVector4Normalize(XMVector3Cross(vRight, vUp)) * m_vecParticleInfoDesc[iNum].vCurScales.z;
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * m_vecParticleInfoDesc[iNum].vCurScales.y;

	// 저장
	m_vecParticleShaderInfoDesc[iNum].vRight = vRight;
	m_vecParticleShaderInfoDesc[iNum].vUp = vUp;
	m_vecParticleShaderInfoDesc[iNum].vLook = vLook;

}

void CVIBuffer_Effect_Model_Instance::Make_DirToLook(_uint iNum)
{
	_vector		vRight, vUp, vLook;

	// 이동 진행 방향벡터를 Look으로 한 새로운 Right, Look 정해주기 ===================================
	vLook = XMVector4Normalize(m_vecParticleInfoDesc[iNum].vDir) * m_vecParticleInfoDesc[iNum].vCurScales.z;
	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * m_vecParticleInfoDesc[iNum].vCurScales.x;
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * m_vecParticleInfoDesc[iNum].vCurScales.y;
	vRight = XMVector4Normalize(XMVector3Cross(vUp, vLook)) * m_vecParticleInfoDesc[iNum].vCurScales.x;


	// 저장
	m_vecParticleShaderInfoDesc[iNum].vRight = vRight;
	m_vecParticleShaderInfoDesc[iNum].vUp = vUp;
	m_vecParticleShaderInfoDesc[iNum].vLook = vLook;
}



void CVIBuffer_Effect_Model_Instance::Update(_float fTimeDelta)
{

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pModelInstance = ((VTXMODELINSTANCE*)SubResource.pData);

	for (_uint i = 0; i < m_iNumInstance; i++)	// 반복문 시작
	{


	}	// 반복문 끝

	m_pContext->Unmap(m_pVBInstance, 0);

}


void CVIBuffer_Effect_Model_Instance::Update_Particle(_float fTimeDelta)
{

	// 파티클 모드일때만
	if (MODE_PARTICLE == m_tBufferDesc.eType_Mode)
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



#pragma region 모델 바꿔끼기 시작
		// 모프가 True이면 
		if (m_tBufferDesc.bMorph)
		{
			m_tBufferDesc.fMorphTimeAcc += fTimeDelta;

			_int iNum = ECast(m_tBufferDesc.eCurModelNum);
			if (m_tBufferDesc.fMorphTimeAcc >= m_tBufferDesc.fMorphTimeTerm)
			{
				iNum += 1;
				m_tBufferDesc.eCurModelNum = (MODEL_MORPH)iNum;

				if (m_tBufferDesc.eCurModelNum >= MORPH_END)
				{
					m_tBufferDesc.eCurModelNum = MORPH_01;
				}

				m_tBufferDesc.fMorphTimeAcc = 0.f;
			}
		}
#pragma region 모델 바꿔끼기 끝


#pragma region 방출 시작
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
#pragma region 방출 끝


#pragma region Map UnMap 전 조건 체크 끝

		D3D11_MAPPED_SUBRESOURCE			SubResource = {};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		VTXMODELINSTANCE* pModelInstance = ((VTXMODELINSTANCE*)SubResource.pData);

#ifdef _DEBUG
		m_iNumInstance = m_tBufferDesc.iCurNumInstance;
#endif // _DEBUG
		for (_uint i = 0; i < m_iNumInstance; i++)	// 반복문 시작
		{

			if (m_vecParticleInfoDesc[i].bDie) // 죽었으면
			{
				// 죽었으면 안보이게 & 다음 반복으로
				m_vecParticleShaderInfoDesc[i].vRight	= { 0.f, 0.f, 0.f, 0.f };
				m_vecParticleShaderInfoDesc[i].vUp		= { 0.f, 0.f, 0.f, 0.f };
				m_vecParticleShaderInfoDesc[i].vLook	= { 0.f, 0.f, 0.f, 0.f };


				pModelInstance[i].vRight = _float4{ m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f };
				pModelInstance[i].vUp = _float4{ m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f };
				pModelInstance[i].vLook = _float4{ m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f };

				m_vecParticleShaderInfoDesc[i].vCurrentColors.w = { 0.f };


				// 시간 정지
				m_vecParticleInfoDesc[i].fTimeAccs = m_vecParticleInfoDesc[i].fLifeTime;
				m_vecParticleInfoDesc[i].fLifeTimeRatios = 1.f;


				if (m_tBufferDesc.bRecycle)	// 재사용이면 리셋
				{
					m_vecParticleInfoDesc[i].bDie = FALSE;

					// 랜덤 값 다시 뽑기
					ReSet_ParticleInfo(i);


					// 초기 위치 세팅
					XMStoreFloat4(&pModelInstance[i].vTranslation, XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions));


					m_vecParticleInfoDesc[i].Reset_ParticleTimes();		// 시간 초기화
				}
				else
				{
					continue; // 죽었고, 재사용도 아니면 다음 반복으로
				}

			}
			else
			{
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
									m_vecParticleInfoDesc[i].vCurScales.z = 0.f;


									// 크기에 따른 알파(0으로 초기화)
									if (SCALE == m_tBufferDesc.eType_Fade_Takes)
									{
										m_vecParticleShaderInfoDesc[i].vCurrentColors.w = 0.f;
										m_vecParticleShaderInfoDesc[i].fCurAddAlpha = 0.f;
									}

								}
								else
								{
									m_vecParticleInfoDesc[i].fDownScaleTimeAccs += fTimeDelta;	// 시간 누적	
									m_vecParticleInfoDesc[i].vCurScales.x = abs(Easing::LerpToType(m_vecParticleInfoDesc[i].vMaxScales.x, 0.f, m_vecParticleInfoDesc[i].fDownScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
									m_vecParticleInfoDesc[i].vCurScales.y = abs(Easing::LerpToType(m_vecParticleInfoDesc[i].vMaxScales.y, 0.f, m_vecParticleInfoDesc[i].fDownScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
									m_vecParticleInfoDesc[i].vCurScales.z = abs(Easing::LerpToType(m_vecParticleInfoDesc[i].vMaxScales.z, 0.f, m_vecParticleInfoDesc[i].fDownScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));


									// 크기에 따른 알파 러프
									if (SCALE == m_tBufferDesc.eType_Fade_Takes)
									{
										_float fAlpha = max(fTotalTime - m_vecParticleInfoDesc[i].fDownScaleTimeAccs, 0.f);

										m_vecParticleShaderInfoDesc[i].vCurrentColors.w = fAlpha;
										m_vecParticleShaderInfoDesc[i].fCurAddAlpha = m_vecParticleInfoDesc[i].fAddAlpha * fAlpha;
									}

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
									m_vecParticleInfoDesc[i].vCurScales.z = m_vecParticleInfoDesc[i].vMaxScales.z;
								}
								else
								{
									m_vecParticleInfoDesc[i].fUpScaleTimeAccs += fTimeDelta;	// 시간 누적		
									m_vecParticleInfoDesc[i].vCurScales.x = abs(Easing::LerpToType(m_tBufferDesc.vStartScale.x, m_vecParticleInfoDesc[i].vMaxScales.x, m_vecParticleInfoDesc[i].fUpScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
									m_vecParticleInfoDesc[i].vCurScales.y = abs(Easing::LerpToType(m_tBufferDesc.vStartScale.y, m_vecParticleInfoDesc[i].vMaxScales.y, m_vecParticleInfoDesc[i].fUpScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
									m_vecParticleInfoDesc[i].vCurScales.z = abs(Easing::LerpToType(m_tBufferDesc.vStartScale.z, m_vecParticleInfoDesc[i].vMaxScales.z, m_vecParticleInfoDesc[i].fUpScaleTimeAccs, fTotalTime, m_tBufferDesc.eType_ScaleLerp));
								}

							}

						}
					}

					//// 크기변경 적용
					//pModelInstance[i].vRight = _float4(1.f, 0.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.x;
					//pModelInstance[i].vUp = _float4(0.f, 1.f, 0.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.y;
					//pModelInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f) * m_vecParticleInfoDesc[i].vCurScales.y;

#pragma region 크기 러프 끝


#pragma region 회전 : 자체 회전 시작

					Update_DirToAxis(i);

					// 회전, 크기 적용
					pModelInstance[i].vRight	= _float4(m_vecParticleShaderInfoDesc[i].vRight.x, m_vecParticleShaderInfoDesc[i].vRight.y, m_vecParticleShaderInfoDesc[i].vRight.z, 0.f) * m_vecParticleInfoDesc[i].vCurScales.x;
					pModelInstance[i].vUp		= _float4(m_vecParticleShaderInfoDesc[i].vUp.x, m_vecParticleShaderInfoDesc[i].vUp.y, m_vecParticleShaderInfoDesc[i].vUp.z, 0.f) * m_vecParticleInfoDesc[i].vCurScales.y;
					pModelInstance[i].vLook		= _float4(m_vecParticleShaderInfoDesc[i].vLook.x, m_vecParticleShaderInfoDesc[i].vLook.y, m_vecParticleShaderInfoDesc[i].vLook.z, 0.f) * m_vecParticleInfoDesc[i].vCurScales.z;

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

								if (!Check_Sleep(i))	// 슬립이 아니면 리지드바디 업데이트
								{
									if (m_tBufferDesc.bKinetic)
									{
										// 스피드 러프
										m_vecParticleInfoDesc[i].fCurSpeed = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxSpeed.x, m_tBufferDesc.vMinMaxSpeed.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_SpeedLerp));

										Update_Kinetic(i, fTimeDelta);	// 이동 속력 계산 업데이트

										// 계산된 속력으로 이동할 위치 계산 / Translate : vMovePos = vPos + Get_State(CTransform::STATE_POSITION);
										_vector vMovePos = (XMLoadFloat3(&m_vecParticleRigidbodyDesc[i].vVelocity) * (m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta)) + XMLoadFloat4(&pModelInstance[i].vTranslation); XMVectorSetW(vMovePos, 1.f);
										//_vector vMovePos = XMLoadFloat3(&m_vecParticleRigidbodyDesc[i].vVelocity) * fTimeDelta + XMLoadFloat4(&pVertices[i].vPosition); XMVectorSetW(vMovePos, 1.f);
										XMStoreFloat4(&pModelInstance[i].vTranslation, vMovePos);	// 최종 위치 이동 적용


										m_vecParticleInfoDesc[i].vDir = m_vecParticleRigidbodyDesc[i].vVelocity;			// 방향 저장

									}
									else
									{
										Update_Kinematic(i);
									}
								}

								// 죽음 조건		
								_vector vCurPos = XMLoadFloat4(&pModelInstance[i].vTranslation);
								_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions)));	// 센터에서 현재 위치까지의 거리

								if (m_vecParticleInfoDesc[i].fMaxRange <= fLength)	// 현재 이동 거리가 맥스 레인지보다 크거나 같으면 초기화 or 죽음
								{
									m_vecParticleInfoDesc[i].bDie = TRUE;
								}

							}

						}

					}
#pragma endregion 이동 : 리지드바디 끝


#pragma region 이동 : 직접 이동 시작
					else
					{
						// 리지드바디 사용이 아니면 직접 이동
						if (SPARK == m_tBufferDesc.eType_Action)
						{
							// 이동 :: 현재 포지션 + (입자들의 방향 * (현재 스피드 * 타임델타))
							_vector vMovePos = XMLoadFloat4(&pModelInstance[i].vTranslation) + m_vecParticleInfoDesc[i].vDir * (m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta);
							XMStoreFloat4(&pModelInstance[i].vTranslation, vMovePos);


							// 초기화 조건		
							_vector vCurPos = XMLoadFloat4(&pModelInstance[i].vTranslation);
							_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions)));	// 센터에서 현재 위치까지의 거리

							if (m_vecParticleInfoDesc[i].fMaxRange <= fLength)	// 현재 이동 거리가 맥스 레인지보다 크거나 같으면 초기화 or 죽음
							{
								m_vecParticleInfoDesc[i].bDie = TRUE;
							}

						}
						else if (BLINK == m_tBufferDesc.eType_Action)
						{
							// 이동 없음

							// 죽음 조건
							if (m_vecParticleInfoDesc[i].fLifeTime <= m_vecParticleInfoDesc[i].fTimeAccs)	// 라이프 타임이 끝나면 초기화 or 죽음
							{
								m_vecParticleInfoDesc[i].bDie = TRUE;
							}

						}
						else if (FALL == m_tBufferDesc.eType_Action)
						{
							pModelInstance[i].vTranslation.y -= m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta;	// 위로 이동

							// 죽음 조건
							if (m_vecParticleInfoDesc[i].fMaxPosY >= pModelInstance[i].vTranslation.y)	// 현재 y위치가 최대 범위보다 작으면 초기화 or 죽음
							{
								m_vecParticleInfoDesc[i].bDie = TRUE;
							}

						}
						else if (RISE == m_tBufferDesc.eType_Action)
						{
							pModelInstance[i].vTranslation.y += m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta;	// 위로 이동


							// 죽음 조건
							if (m_vecParticleInfoDesc[i].fMaxPosY <= pModelInstance[i].vTranslation.y)	// 현재 y위치가 최대 범위보다 크면 초기화 or 죽음
							{
								m_vecParticleInfoDesc[i].bDie = TRUE;
							}

						}
						else if (TORNADO == m_tBufferDesc.eType_Action)
						{
							// 초기 각도에 시간과 회전 속도를 곱하여 갱신된 각도를 얻는다.
							_float fTheta = m_vecParticleInfoDesc[i].fCurTheta + fTimeDelta * m_vecParticleInfoDesc[i].fCurTornadoSpeed;

							_float fNewPosX = m_vecParticleInfoDesc[i].vCenterPositions.x + ((m_vecParticleInfoDesc[i].fMaxRange + (m_vecParticleInfoDesc[i].fAddRange * m_vecParticleInfoDesc[i].fLifeTimeRatios))) * cos(fTheta); // 중심 x좌표에 반지름 * cos(theta)를 더한다.
							_float fNewPosZ = m_vecParticleInfoDesc[i].vCenterPositions.z + ((m_vecParticleInfoDesc[i].fMaxRange + (m_vecParticleInfoDesc[i].fAddRange * m_vecParticleInfoDesc[i].fLifeTimeRatios))) * sin(fTheta); // 중심 z좌표에 반지름 * sin(theta)를 더한다.


							pModelInstance[i].vTranslation.x = fNewPosX;
							pModelInstance[i].vTranslation.z = fNewPosZ;
							pModelInstance[i].vTranslation.y += m_vecParticleInfoDesc[i].fCurSpeed * fTimeDelta;


							// 각 입자의 현재 각도를 업데이트
							m_vecParticleInfoDesc[i].fCurTheta = fTheta;
							m_vecParticleInfoDesc[i].vDir = { pModelInstance[i].vTranslation.x, pModelInstance[i].vTranslation.y, pModelInstance[i].vTranslation.z };


							// 죽음 조건
							if (m_vecParticleInfoDesc[i].fMaxPosY <= pModelInstance[i].vTranslation.y)	// 현재 y위치가 최대 범위보다 크면 초기화 or 죽음
							{
								m_vecParticleInfoDesc[i].bDie = TRUE;
							}


						}


					}
#pragma endregion 이동 : 직접 이동 끝


#pragma region 색 변경 시작

					if (m_tBufferDesc.bDynamic_Color)	// 입자마다 다른 주기로 색 변경
					{
						m_vecParticleShaderInfoDesc[i].vCurrentColors.x = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_ColorLerp));
						m_vecParticleShaderInfoDesc[i].vCurrentColors.y = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_ColorLerp));
						m_vecParticleShaderInfoDesc[i].vCurrentColors.z = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y, m_vecParticleInfoDesc[i].fTimeAccs, m_vecParticleInfoDesc[i].fLifeTime, m_tBufferDesc.eType_ColorLerp));

					}
					else // 일괄 색 변경
					{
						m_tBufferDesc.vCurrentColor.x = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxRed.x, m_tBufferDesc.vMinMaxRed.y, m_tBufferDesc.fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));
						m_tBufferDesc.vCurrentColor.y = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxGreen.x, m_tBufferDesc.vMinMaxGreen.y, m_tBufferDesc.fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));
						m_tBufferDesc.vCurrentColor.z = abs(Easing::LerpToType(m_tBufferDesc.vMinMaxBlue.x, m_tBufferDesc.vMinMaxBlue.y, m_tBufferDesc.fTimeAcc, m_tBufferDesc.vMinMaxLifeTime.y, m_tBufferDesc.eType_ColorLerp));

						// 쉐이더에 던질 곱하기 색상 값 저장
						m_vecParticleShaderInfoDesc[i].vCurrentColors = m_tBufferDesc.vCurrentColor;
					}

#pragma region 색 변경 끝


#pragma region 알파 업데이트 시작

					if (LIFE == m_tBufferDesc.eType_Fade_Takes)
					{
						// 라이프타임에 따라 알파 업데이트

						_float	fAlpha = 1.f;

						if (FADE_NONE == m_tBufferDesc.eType_Fade)
						{
							fAlpha = 1.f;
						}
						else if (FADE_OUT == m_tBufferDesc.eType_Fade)
						{
							fAlpha = max(m_vecParticleInfoDesc[i].fLifeTime - m_vecParticleInfoDesc[i].fTimeAccs, 0.f);
						}
						else if (FADE_IN == m_tBufferDesc.eType_Fade)
						{
							fAlpha = min(m_vecParticleInfoDesc[i].fLifeTime + m_vecParticleInfoDesc[i].fTimeAccs, 1.f);
						}

						m_vecParticleShaderInfoDesc[i].vCurrentColors.w = fAlpha;
						m_vecParticleShaderInfoDesc[i].fCurAddAlpha = m_vecParticleInfoDesc[i].fAddAlpha * fAlpha;

					}
					else if (DIST == m_tBufferDesc.eType_Fade_Takes)
					{
						// 거리에 따라 알파 업데이트

						_float	fAlpha = 1.f;

						_vector vCurPos = XMLoadFloat4(&pModelInstance[i].vTranslation);
						_float	fLength = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat4(&m_vecParticleInfoDesc[i].vCenterPositions)));	// 센터에서 현재 위치까지의 거리

						if (FADE_NONE == m_tBufferDesc.eType_Fade)
						{
							fAlpha = 1.f;
						}
						else if (FADE_OUT == m_tBufferDesc.eType_Fade)
						{
							_float fValue = m_vecParticleInfoDesc[i].fMaxRange - fLength;

							if (0.f != m_vecParticleInfoDesc[i].fMaxRange && 0.f != fValue)
								fAlpha = max(min(fValue / m_vecParticleInfoDesc[i].fMaxRange, 1.f), 0.f);
		
						}
						else if (FADE_IN == m_tBufferDesc.eType_Fade)
						{
							//fAlpha = min(fLength, 1.f);

							if (0.f != m_vecParticleInfoDesc[i].fMaxRange && 0.f != fLength)
								fAlpha = max(min(fLength / m_vecParticleInfoDesc[i].fMaxRange, 1.f), 0.f);
	
						}

						m_vecParticleShaderInfoDesc[i].vCurrentColors.w = fAlpha;
						m_vecParticleShaderInfoDesc[i].fCurAddAlpha = m_vecParticleInfoDesc[i].fAddAlpha * fAlpha;

					}
					else if (HEIGHT == m_tBufferDesc.eType_Fade_Takes)
					{
						// 높이에 따라 알파 업데이트

						_float		fAlpha = 1.f;

						if (FADE_NONE == m_tBufferDesc.eType_Fade)
						{
							fAlpha = 1.f;
						}
						else if (FADE_OUT == m_tBufferDesc.eType_Fade)
						{
							_float fValue = m_vecParticleInfoDesc[i].fMaxPosY - pModelInstance[i].vTranslation.y;

							if (0.f != m_vecParticleInfoDesc[i].fMaxPosY && 0.f != fValue)
								fAlpha = max(min(fValue / m_vecParticleInfoDesc[i].fMaxPosY, 1.f), 0.f);

						}
						else if (FADE_IN == m_tBufferDesc.eType_Fade)
						{					
							if (0.f != m_vecParticleInfoDesc[i].fMaxPosY && 0.f != pModelInstance[i].vTranslation.y)
								fAlpha = max(min((pModelInstance[i].vTranslation.y / m_vecParticleInfoDesc[i].fMaxPosY), 1.f), 0.f);
						}

						m_vecParticleShaderInfoDesc[i].vCurrentColors.w = fAlpha;
						m_vecParticleShaderInfoDesc[i].fCurAddAlpha = m_vecParticleInfoDesc[i].fAddAlpha * fAlpha;

					}
					else if (TYPE_FADE_TAKES_END == m_tBufferDesc.eType_Fade_Takes)
					{
						// 알파 업데이트 안함

					}


#pragma region 알파 업데이트 끝


				} // 방출이 됐으면

			} // 죽은게 아니면


		}	// 반복문 끝

		m_pContext->Unmap(m_pVBInstance, 0);
	}
}


HRESULT CVIBuffer_Effect_Model_Instance::Bind_VIBuffers(_uint iMeshContainerIndex)
{

	__super::Bind_VIBuffers(iMeshContainerIndex);

	return S_OK;
}


HRESULT CVIBuffer_Effect_Model_Instance::Render(_int iMeshIndex)
{

	if (m_tBufferDesc.bMorph)	// 모프가 True이면 (박쥐 모델)
	{
		Bind_VIBuffers(m_tBufferDesc.eCurModelNum);
		m_pContext->DrawIndexedInstanced(m_vecInstanceMesh[m_tBufferDesc.eCurModelNum]->Get_NumIndices(), m_iNumInstance, 0, 0, 0);

		return S_OK;
	}
	else
	{
		Bind_VIBuffers(iMeshIndex);
		m_pContext->DrawIndexedInstanced(m_vecInstanceMesh[iMeshIndex]->Get_NumIndices(), m_iNumInstance, 0, 0, 0);
	}


	return S_OK;
}


_bool CVIBuffer_Effect_Model_Instance::Write_Json(json& Out_Json)
{
	/* 인스턴스 개수 */
	Out_Json["Com_VIBuffer"]["iCurNumInstance"] = m_tBufferDesc.iCurNumInstance;	

	/* States */
	Out_Json["Com_VIBuffer"]["eType_Mode"] = m_tBufferDesc.eType_Mode;
	Out_Json["Com_VIBuffer"]["bRecycle"] = m_tBufferDesc.bRecycle;
	Out_Json["Com_VIBuffer"]["bReverse"] = m_tBufferDesc.bReverse;
	Out_Json["Com_VIBuffer"]["eType_Action"] = m_tBufferDesc.eType_Action;


	/* LifeTime */
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxLifeTime"], m_tBufferDesc.vMinMaxLifeTime);


	/* Emitter */
	Out_Json["Com_VIBuffer"]["fEmissionTime"] = m_tBufferDesc.fEmissionTime;
	Out_Json["Com_VIBuffer"]["iAddEmitCount"] = m_tBufferDesc.iAddEmitCount;


	/* Morph */
	Out_Json["Com_VIBuffer"]["bMorph"] = m_tBufferDesc.bMorph;
	Out_Json["Com_VIBuffer"]["fMorphTimeTerm"] = m_tBufferDesc.fMorphTimeTerm;


	/* RigidBody */
	Out_Json["Com_VIBuffer"]["bUseRigidBody"] = m_tBufferDesc.bUseRigidBody;
	Out_Json["Com_VIBuffer"]["bKinetic"] = m_tBufferDesc.bKinetic;
	Out_Json["Com_VIBuffer"]["bUseGravity"] = m_tBufferDesc.bUseGravity;
	Out_Json["Com_VIBuffer"]["eForce_Mode"] = m_tBufferDesc.eForce_Mode;

	Out_Json["Com_VIBuffer"]["fGravity"] = m_tBufferDesc.fGravity;
	Out_Json["Com_VIBuffer"]["fFriction"] = m_tBufferDesc.fFriction;
	Out_Json["Com_VIBuffer"]["fSleepThreshold"] = m_tBufferDesc.fSleepThreshold;
	Out_Json["Com_VIBuffer"]["byFreezeAxis"] = m_tBufferDesc.byFreezeAxis;

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxPower"], m_tBufferDesc.vMinMaxPower);


	/* For.Position */
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vMinCenterOffsetPos"], m_tBufferDesc.vMinCenterOffsetPos);
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vMaxCenterOffsetPos"], m_tBufferDesc.vMaxCenterOffsetPos);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxAddRange"], m_tBufferDesc.vMinMaxAddRange);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxPosY"], m_tBufferDesc.vMinMaxPosY);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxTheta"], m_tBufferDesc.vMinMaxTheta);


	/* For.Rotation */
	Out_Json["Com_VIBuffer"]["eType_Dir"] = m_tBufferDesc.eType_Dir;

	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vRadian"], m_tBufferDesc.vRadian);

	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);


	/* For.Scale */
	Out_Json["Com_VIBuffer"]["bScaleRatio"] = m_tBufferDesc.bScaleRatio;
	Out_Json["Com_VIBuffer"]["bUseScaleLerp"] = m_tBufferDesc.bUseScaleLerp;
	Out_Json["Com_VIBuffer"]["eType_ScaleLerp"] = m_tBufferDesc.eType_ScaleLerp;
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vScaleLerp_Up_Pos"], m_tBufferDesc.vScaleLerp_Up_Pos);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vScaleLerp_Down_Pos"], m_tBufferDesc.vScaleLerp_Down_Pos);

	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vStartScale"], m_tBufferDesc.vStartScale);
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vEndScale"], m_tBufferDesc.vEndScale);


	/* For.Color */
	Out_Json["Com_VIBuffer"]["eType_ColorLerp"] = m_tBufferDesc.eType_ColorLerp;
	Out_Json["Com_VIBuffer"]["bDynamic_Color"] = m_tBufferDesc.bDynamic_Color;
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRed"], m_tBufferDesc.vMinMaxRed);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxGreen"], m_tBufferDesc.vMinMaxGreen);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxBlue"], m_tBufferDesc.vMinMaxBlue);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxAlpha"], m_tBufferDesc.vMinMaxAlpha);

	Out_Json["Com_VIBuffer"]["eType_Fade"] = m_tBufferDesc.eType_Fade;
	Out_Json["Com_VIBuffer"]["eType_Fade_Takes"] = m_tBufferDesc.eType_Fade_Takes;



	return true;
}

void CVIBuffer_Effect_Model_Instance::Load_FromJson(const json& In_Json)
{
	/* 인스턴스 개수 */
	m_tBufferDesc.iCurNumInstance = In_Json["Com_VIBuffer"]["iCurNumInstance"];

	/* States */
	m_tBufferDesc.eType_Mode = In_Json["Com_VIBuffer"]["eType_Mode"];

	if (In_Json["Com_VIBuffer"].contains("bRecycle"))	// 다시 저장 후 삭제
		m_tBufferDesc.bRecycle = In_Json["Com_VIBuffer"]["bRecycle"];

	if (In_Json["Com_VIBuffer"].contains("bReverse"))	// 다시 저장 후 삭제
		m_tBufferDesc.bReverse = In_Json["Com_VIBuffer"]["bReverse"];

	if(In_Json["Com_VIBuffer"].contains("eType_Action"))	// 다시 저장 후 삭제
		m_tBufferDesc.eType_Action = In_Json["Com_VIBuffer"]["eType_Action"];


	/* LifeTime */
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxLifeTime"], m_tBufferDesc.vMinMaxLifeTime);


	/* Emitter */
	if (In_Json["Com_VIBuffer"].contains("fEmissionTime"))	// 다시 저장 후 삭제
	{
		m_tBufferDesc.fEmissionTime = In_Json["Com_VIBuffer"]["fEmissionTime"];
		m_tBufferDesc.iAddEmitCount = In_Json["Com_VIBuffer"]["iAddEmitCount"];
	}
		

	/* Morph */
	m_tBufferDesc.bMorph = In_Json["Com_VIBuffer"]["bMorph"];
	m_tBufferDesc.fMorphTimeTerm = In_Json["Com_VIBuffer"]["fMorphTimeTerm"];


	/* RigidBody */
	m_tBufferDesc.bUseRigidBody = In_Json["Com_VIBuffer"]["bUseRigidBody"];
	m_tBufferDesc.bKinetic = In_Json["Com_VIBuffer"]["bKinetic"];
	m_tBufferDesc.bUseGravity = In_Json["Com_VIBuffer"]["bUseGravity"];
	m_tBufferDesc.eForce_Mode = In_Json["Com_VIBuffer"]["eForce_Mode"];

	m_tBufferDesc.fGravity = In_Json["Com_VIBuffer"]["fGravity"];
	m_tBufferDesc.fFriction = In_Json["Com_VIBuffer"]["fFriction"];
	m_tBufferDesc.fSleepThreshold = In_Json["Com_VIBuffer"]["fSleepThreshold"];
	m_tBufferDesc.byFreezeAxis = In_Json["Com_VIBuffer"]["byFreezeAxis"];

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxPower"], m_tBufferDesc.vMinMaxPower);


	/* For.Position */
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vMinCenterOffsetPos"], m_tBufferDesc.vMinCenterOffsetPos);
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vMaxCenterOffsetPos"], m_tBufferDesc.vMaxCenterOffsetPos);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);
	if (In_Json["Com_VIBuffer"].contains("vMinMaxAddRange"))	// 다시 저장 후 삭제
	{
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxAddRange"], m_tBufferDesc.vMinMaxAddRange);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxPosY"], m_tBufferDesc.vMinMaxPosY);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxTheta"], m_tBufferDesc.vMinMaxTheta);
	}


	/* For.Rotation */
	if (In_Json["Com_VIBuffer"].contains("eType_Dir")) // 다시 저장 후 삭제
		m_tBufferDesc.eType_Dir = In_Json["Com_VIBuffer"]["eType_Dir"];

	if (In_Json["Com_VIBuffer"].contains("vRadian")) // 다시 저장 후 삭제
		CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vRadian"], m_tBufferDesc.vRadian);

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);



	/* For.Scale */
	if (In_Json["Com_VIBuffer"].contains("bScaleRatio")) // 다시 저장 후 삭제
	{
		m_tBufferDesc.bScaleRatio = In_Json["Com_VIBuffer"]["bScaleRatio"];
	}

	if (In_Json["Com_VIBuffer"].contains("bUseScaleLerp")) // 다시 저장 후 삭제
	{
		m_tBufferDesc.bUseScaleLerp = In_Json["Com_VIBuffer"]["bUseScaleLerp"];

		m_tBufferDesc.eType_ScaleLerp = In_Json["Com_VIBuffer"]["eType_ScaleLerp"];

		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vScaleLerp_Up_Pos"], m_tBufferDesc.vScaleLerp_Up_Pos);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vScaleLerp_Down_Pos"], m_tBufferDesc.vScaleLerp_Down_Pos);

		CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vStartScale"], m_tBufferDesc.vStartScale);
		CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vEndScale"], m_tBufferDesc.vEndScale);
	}


	/* For.Color */
	if (In_Json["Com_VIBuffer"].contains("eType_ColorLerp")) // 다시 저장 후 삭제
	{
		m_tBufferDesc.eType_ColorLerp = In_Json["Com_VIBuffer"]["eType_ColorLerp"];
		m_tBufferDesc.bDynamic_Color = In_Json["Com_VIBuffer"]["bDynamic_Color"];
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRed"], m_tBufferDesc.vMinMaxRed);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxGreen"], m_tBufferDesc.vMinMaxGreen);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxBlue"], m_tBufferDesc.vMinMaxBlue);
		CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxAlpha"], m_tBufferDesc.vMinMaxAlpha);


		m_tBufferDesc.eType_Fade = In_Json["Com_VIBuffer"]["eType_Fade"];
		m_tBufferDesc.eType_Fade_Takes = In_Json["Com_VIBuffer"]["eType_Fade_Takes"];
	}


}


_float3 CVIBuffer_Effect_Model_Instance::Update_Kinetic(_uint iNum, _float fTimeDelta)
{
	/* 중력 사용 시 아래로 떨어짐 */
	if (m_tBufferDesc.bUseGravity)
		m_vecParticleRigidbodyDesc[iNum].vVelocity.y += m_tBufferDesc.fGravity * fTimeDelta;

	/* V += A * TD */
	/* m_vVelocity += m_vAccel * fTimeDelta; */
	XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity) += XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vAccel) * fTimeDelta);


	/* 마찰력에 의한 반대방향으로의 가속도(감속) */
	if (1.f > m_tBufferDesc.fFriction)
	{
		/* (m_vVelocity * (1.f - m_fFriction)) */
		XMStoreFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity, XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity) * (1.f - m_tBufferDesc.fFriction));
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

void CVIBuffer_Effect_Model_Instance::Update_Kinematic(_uint iNum)
{
	/* 질량 무시, 직접 속도 변화 부여 (키네마틱, 즉 리지드바디 사용안함) */
	Clear_Force(iNum, FORCE_MODE::VELOCITY_CHANGE);
}


void CVIBuffer_Effect_Model_Instance::Add_Force(_uint iNum, _fvector vForce, FORCE_MODE eMode)
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

void CVIBuffer_Effect_Model_Instance::Clear_Force(_uint iNum, const FORCE_MODE& eMode)
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


void CVIBuffer_Effect_Model_Instance::Clear_Power(_uint iNum)
{
	ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vAccel, sizeof(_float3));
	ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vVelocity, sizeof(_float3));
}


const _bool CVIBuffer_Effect_Model_Instance::Check_Sleep(_uint iNum)
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

CVIBuffer_Effect_Model_Instance* CVIBuffer_Effect_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Effect_Model_Instance* pInstance = new CVIBuffer_Effect_Model_Instance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Effect_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Effect_Model_Instance::Clone(void* pArg)
{
	CVIBuffer_Effect_Model_Instance* pInstance = new CVIBuffer_Effect_Model_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Effect_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Effect_Model_Instance::Free()
{
	__super::Free();

}