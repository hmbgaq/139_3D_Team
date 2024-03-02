#include "VIBuffer_Effect_Model_Instance.h"
#include "Mesh.h"

#include "SMath.h"

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

	Safe_AddRef(m_tBufferDesc.pModel);

	CModel* pModel = m_tBufferDesc.pModel;

	vector<CMesh*> Meshes = pModel->Get_Meshes();
	m_iNumMeshes = (_int)Meshes.size();

	for (_int i = 0; i < m_iNumMeshes; ++i)
	{
		m_vecInstanceMesh.push_back(Meshes[i]);
		Safe_AddRef(Meshes[i]);
	}

	m_iNumMaterials = pModel->Get_NumMaterials();

	Init_Instance(m_tBufferDesc.iCurNumInstance);


	return S_OK;
}

HRESULT CVIBuffer_Effect_Model_Instance::Bind_VIBuffers(_uint iMeshContainerIndex)
{
	
	__super::Bind_VIBuffers(iMeshContainerIndex);

	return S_OK;
}

void CVIBuffer_Effect_Model_Instance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pModelInstance = ((VTXMODELINSTANCE*)SubResource.pData);
	
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		// 리지드 바디 사용이면
		if (m_tBufferDesc.bUseRigidBody)
		{
			if (!m_vecParticleRigidbodyDesc.empty())
			{
				if (!Check_Sleep(i))	// 슬립이 아니면 리지드바디 업데이트
				{
					if (m_vecParticleRigidbodyDesc[i].bKinetic)
					{
						Update_Kinetic(i, fTimeDelta);

						// Translate : vMovePos = vPos + Get_State(CTransform::STATE_POSITION);
						_vector vMovePos = (XMLoadFloat3(&m_vecParticleRigidbodyDesc[i].vVelocity) * fTimeDelta) + XMLoadFloat4(&pModelInstance[i].vTranslation);
						XMVectorSetW(vMovePos, 1.f);

						// 최종 위치 이동
						XMStoreFloat4(&pModelInstance[i].vTranslation, vMovePos);
					}
					else
					{
						Update_Kinematic(i, fTimeDelta);
					}
				}
			}
		}
		else // 리지드 바디 사용이 아니면
		{
			// 테스트용 원점 위치로 고정
			XMStoreFloat4(&pModelInstance[i].vTranslation, m_tBufferDesc.vCenterPosition);
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
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


	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		PARTICLE_RIGIDBODY_DESC ParticleRigidbody = {};
		if (m_tBufferDesc.bUseRigidBody)
			m_vecParticleRigidbodyDesc.push_back(ParticleRigidbody);

		pModelInstance[i].vRight		= _float4(1.f, 0.f, 0.f, 0.f)	/* * 크기 */;
		pModelInstance[i].vUp			= _float4(0.f, 1.f, 0.f, 0.f)	/* * 크기 */;
		pModelInstance[i].vLook			= _float4(0.f, 0.f, 1.f, 0.f)	/* * 크기 */;
		pModelInstance[i].vTranslation	= _float4(0.f, 0.f, 0.f, 1.f);


		// 테스트용 원점 위치로 고정
		XMStoreFloat4(&pModelInstance[i].vTranslation, m_tBufferDesc.vCenterPosition);

		// 리지드 바디 사용이면
		if (m_tBufferDesc.bUseRigidBody)
		{
			_vector		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			vDir = XMVector3Normalize(vDir) * SMath::fRandom(m_tBufferDesc.vMinMaxRange.x, m_tBufferDesc.vMinMaxRange.y);

			m_tBufferDesc.vRotationOffset = { SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetX.x, m_tBufferDesc.vMinMaxRotationOffsetX.y)
											, SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetY.x, m_tBufferDesc.vMinMaxRotationOffsetY.y)
											, SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetZ.x, m_tBufferDesc.vMinMaxRotationOffsetZ.y) };


			_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_tBufferDesc.vRotationOffset.x, m_tBufferDesc.vRotationOffset.y, m_tBufferDesc.vRotationOffset.z);
			_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

			_vector vForce = XMVector3TransformNormal(vDir, RotationMatrix) * SMath::fRandom(m_tBufferDesc.vMinMaxPower.x , m_tBufferDesc.vMinMaxPower.y);

			Add_Force(i, vForce, m_tBufferDesc.eForce_Mode);
		}

	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pModelInstance;

	Create_Buffer(&m_pVBInstance);

	Safe_Delete_Array(pModelInstance);

}

HRESULT CVIBuffer_Effect_Model_Instance::Render(_int iMeshIndex)
{
	CModel* pModel = m_tBufferDesc.pModel;

	if(nullptr == pModel)
		return E_FAIL;

	
	Bind_VIBuffers(iMeshIndex);

	m_pContext->DrawIndexedInstanced(m_vecInstanceMesh[iMeshIndex]->Get_NumIndices(), m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Effect_Model_Instance::ReSet()
{
	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pModelInstance = ((VTXMODELINSTANCE*)SubResource.pData);

	m_iNumInstance = m_tBufferDesc.iCurNumInstance;
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		// 원점 위치로
		XMStoreFloat4(&pModelInstance[i].vTranslation, m_tBufferDesc.vCenterPosition);


		// 리지드 바디 사용이면
		if (m_tBufferDesc.bUseRigidBody)
		{
			if (!m_vecParticleRigidbodyDesc.empty())
			{
				// 힘 초기화
				Clear_Power(i);

				_vector		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
				vDir = XMVector3Normalize(vDir) * SMath::fRandom(m_tBufferDesc.vMinMaxRange.x, m_tBufferDesc.vMinMaxRange.y);

				m_tBufferDesc.vRotationOffset = { SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetX.x, m_tBufferDesc.vMinMaxRotationOffsetX.y)
												, SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetY.x, m_tBufferDesc.vMinMaxRotationOffsetY.y)
												, SMath::fRandom(m_tBufferDesc.vMinMaxRotationOffsetZ.x, m_tBufferDesc.vMinMaxRotationOffsetZ.y) };


				_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_tBufferDesc.vRotationOffset.x, m_tBufferDesc.vRotationOffset.y, m_tBufferDesc.vRotationOffset.z);
				_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

				_vector vForce = XMVector3TransformNormal(vDir, RotationMatrix) * SMath::fRandom(m_tBufferDesc.vMinMaxPower.x, m_tBufferDesc.vMinMaxPower.y);

				Add_Force(i, vForce, m_tBufferDesc.eForce_Mode);

			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

_bool CVIBuffer_Effect_Model_Instance::Write_Json(json& Out_Json)
{
	Out_Json["Com_VIBuffer"]["bUseRigidBody"] = m_tBufferDesc.bUseRigidBody;
	Out_Json["Com_VIBuffer"]["eForce_Mode"] = m_tBufferDesc.eForce_Mode;

	Out_Json["Com_VIBuffer"]["iCurNumInstance"] = m_tBufferDesc.iCurNumInstance;

	CJson_Utility::Write_Float4(Out_Json["Com_VIBuffer"]["vCurrentPosition"], m_tBufferDesc.vCenterPosition);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);


	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);
	CJson_Utility::Write_Float3(Out_Json["Com_VIBuffer"]["vRotationOffset"], m_tBufferDesc.vRotationOffset);


	CJson_Utility::Write_Float2(Out_Json["Com_VIBuffer"]["vMinMaxPower"], m_tBufferDesc.vMinMaxPower);


	return true;
}

void CVIBuffer_Effect_Model_Instance::Load_FromJson(const json& In_Json)
{

	m_tBufferDesc.bUseRigidBody = In_Json["Com_VIBuffer"]["bUseRigidBody"];
	m_tBufferDesc.eForce_Mode = In_Json["Com_VIBuffer"]["eForce_Mode"];


	m_tBufferDesc.iCurNumInstance = In_Json["Com_VIBuffer"]["iCurNumInstance"];

	CJson_Utility::Load_Float4(In_Json["Com_VIBuffer"]["vCurrentPosition"], m_tBufferDesc.vCenterPosition);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRange"], m_tBufferDesc.vMinMaxRange);


	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetX"], m_tBufferDesc.vMinMaxRotationOffsetX);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetY"], m_tBufferDesc.vMinMaxRotationOffsetY);
	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxRotationOffsetZ"], m_tBufferDesc.vMinMaxRotationOffsetZ);
	CJson_Utility::Load_Float3(In_Json["Com_VIBuffer"]["vRotationOffset"], m_tBufferDesc.vRotationOffset);

	CJson_Utility::Load_Float2(In_Json["Com_VIBuffer"]["vMinMaxPower"], m_tBufferDesc.vMinMaxPower);

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

_float3 CVIBuffer_Effect_Model_Instance::Update_Kinetic(_int iNum, _float fTimeDelta)
{
	/* 중력 사용 시 아래로 떨어짐 */
	if (m_vecParticleRigidbodyDesc[iNum].bUseGravity)
		m_vecParticleRigidbodyDesc[iNum].vVelocity.y += m_vecParticleRigidbodyDesc[iNum].fGravity * fTimeDelta;

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
	if (m_vecParticleRigidbodyDesc[iNum].byFreezeAxis)
	{
		for (_int i = 0; i < 3; ++i)
		{
			if (m_vecParticleRigidbodyDesc[iNum].byFreezeAxis & 1 << i) // 특정 비트가 1인지 확인(1이면 잠긴 축?)
			{
				*((_float*)&m_vecParticleRigidbodyDesc[iNum].vVelocity + i) = 0.f;
			}
		}
	}

	return m_vecParticleRigidbodyDesc[iNum].vVelocity;
}

void CVIBuffer_Effect_Model_Instance::Update_Kinematic(_int iNum, _float fTimeDelta)
{
	/* 질량 무시, 직접 속도 변화 부여 (키네마틱, 즉 리지드바디 사용안함) */
	Clear_Force(iNum, FORCE_MODE::VELOCITY_CHANGE);
}


void CVIBuffer_Effect_Model_Instance::Add_Force(_int iNum, _fvector vForce, FORCE_MODE eMode)
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

void CVIBuffer_Effect_Model_Instance::Clear_Force(_int iNum, const FORCE_MODE& eMode)
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


void CVIBuffer_Effect_Model_Instance::Clear_Power(_int iNum)
{
	ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vAccel, sizeof(_float3));
	ZeroMemory(&m_vecParticleRigidbodyDesc[iNum].vVelocity, sizeof(_float3));
}


const _bool CVIBuffer_Effect_Model_Instance::Check_Sleep(_int iNum)
{
	if (m_vecParticleRigidbodyDesc[iNum].bSleep)
		return TRUE;

	/* 선형 속도의 크기가 m_fSleepThreshold보다 작으면 슬립(연산안함) */
	_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vecParticleRigidbodyDesc[iNum].vVelocity)));
	if (!m_vecParticleRigidbodyDesc[iNum].bUseGravity && m_vecParticleRigidbodyDesc[iNum].fSleepThreshold > fLength)
	{
		Sleep(iNum);
		return TRUE;
	}

	return FALSE;
}