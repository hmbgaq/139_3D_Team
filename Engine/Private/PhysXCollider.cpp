#include "..\Public\PhysXCollider.h"
#include "GameInstance.h"

_uint CPhysXCollider::m_iClonedColliderIndex = 0;

CPhysXCollider::CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CPhysXCollider::CPhysXCollider(const CPhysXCollider& rhs)
	: CComponent(rhs)
{
}

HRESULT CPhysXCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysXCollider::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_iColliderIndex = m_iClonedColliderIndex++;

	if (nullptr != pArg)
	{
		m_PhysXColliderDesc = *(PHYSXCOLLIDERDESC*)pArg;

		CreatePhysXActor(m_PhysXColliderDesc);
	}

	m_pGameInstance->Register_PhysXCollider(this);

	return S_OK;
}

void CPhysXCollider::CreatePhysXActor(PHYSXCOLLIDERDESC& PhysXColliderDesc)
{
	m_PhysXColliderDesc = PhysXColliderDesc;

	PxTransform	Transform;
	Transform.p = PxVec3(
		XMVectorGetX(PhysXColliderDesc.vPosition),
		XMVectorGetY(PhysXColliderDesc.vPosition),
		XMVectorGetZ(PhysXColliderDesc.vPosition));

	_float4 vQuaternion;
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYawFromVector(PhysXColliderDesc.vAngles));
	Transform.q = PxQuat(
		vQuaternion.x,
		vQuaternion.y,
		vQuaternion.z,
		vQuaternion.w);

	switch (PhysXColliderDesc.eActorType)
	{
	case PHYSXACTOR_TYPE::DYNAMIC:
	case PHYSXACTOR_TYPE::YFIXED_DYNAMIC:
		Create_DynamicActor(PhysXColliderDesc, Transform);
		break;
	case PHYSXACTOR_TYPE::STATIC:
		Create_StaticActor(PhysXColliderDesc, Transform);
		break;
	case PHYSXACTOR_TYPE::ZONE:

		break;
	default:
		//MSG_BOX("Failed to CreateActor : eCollidertype is wrong");
		break;
	}
}

void CPhysXCollider::Create_Geometry()
{
	switch (m_PhysXColliderDesc.eShape)
	{
	case PHYSXCOLLIDER_TYPE::SPHERE:
		m_pGeometry.push_back(new PxSphereGeometry(XMVectorGetX(m_PhysXColliderDesc.vScale)));
		break;

	case PHYSXCOLLIDER_TYPE::BOX:
		m_pGeometry.push_back(new PxBoxGeometry(
			XMVectorGetX(m_PhysXColliderDesc.vScale) * 0.5f,
			XMVectorGetY(m_PhysXColliderDesc.vScale) * 0.5f,
			XMVectorGetZ(m_PhysXColliderDesc.vScale) * 0.5f));


		break;

	case PHYSXCOLLIDER_TYPE::CYLINDER:
	case PHYSXCOLLIDER_TYPE::CONVEXMESH:
	{
		PxConvexMeshGeometry* pPxGeometry = new PxConvexMeshGeometry();
		pPxGeometry->convexMesh = m_PhysXColliderDesc.pConvecMesh;
		PxMeshScale	vScale;
		vScale.scale.x = XMVectorGetX(m_PhysXColliderDesc.vScale);
		vScale.scale.y = XMVectorGetY(m_PhysXColliderDesc.vScale);
		vScale.scale.z = XMVectorGetZ(m_PhysXColliderDesc.vScale);
		pPxGeometry->scale = vScale;
		m_pGeometry.push_back(pPxGeometry);
		break;
	}

	case PHYSXCOLLIDER_TYPE::CONVEXMODEL:
	{
		for (size_t i = 0; i < m_ConvexMeshes.size(); ++i)
		{
			PxConvexMeshGeometry* pPxGeometry = new PxConvexMeshGeometry();
			pPxGeometry->convexMesh = m_ConvexMeshes[i];
			PxMeshScale	vScale;
			vScale.scale.x = XMVectorGetX(m_PhysXColliderDesc.vScale);
			vScale.scale.y = XMVectorGetY(m_PhysXColliderDesc.vScale);
			vScale.scale.z = XMVectorGetZ(m_PhysXColliderDesc.vScale);
			pPxGeometry->scale = vScale;
			m_pGeometry.push_back(pPxGeometry);
		}
		break;
	}

	case PHYSXCOLLIDER_TYPE::MESH:
	{
		PxTriangleMeshGeometry* PxGeometry = new PxTriangleMeshGeometry();
		PxGeometry->triangleMesh = m_TriangleMesh.front();
		PxMeshScale	vScale;
		vScale.scale.x = XMVectorGetX(m_PhysXColliderDesc.vScale);
		vScale.scale.y = XMVectorGetY(m_PhysXColliderDesc.vScale);
		vScale.scale.z = XMVectorGetZ(m_PhysXColliderDesc.vScale);
		PxGeometry->scale = vScale;
		m_pGeometry.push_back(PxGeometry);
		break;
	}

	case PHYSXCOLLIDER_TYPE::MODEL:
	{
		for (size_t i = 0; i < m_TriangleMesh.size(); ++i)
		{
			PxTriangleMeshGeometry* PxGeometry = new PxTriangleMeshGeometry();
			PxGeometry->triangleMesh = m_TriangleMesh[i];
			PxMeshScale	vScale;
			vScale.scale.x = XMVectorGetX(m_PhysXColliderDesc.vScale);
			vScale.scale.y = XMVectorGetY(m_PhysXColliderDesc.vScale);
			vScale.scale.z = XMVectorGetZ(m_PhysXColliderDesc.vScale);
			PxGeometry->scale = vScale;
			m_pGeometry.push_back(PxGeometry);
		}
		break;
	}

	case PHYSXCOLLIDER_TYPE::TYPE_END:
		MSG_BOX("Failed to create DynamicActor : eShape is wrong");
		break;
	default:
		break;
	}
}

void CPhysXCollider::Create_DynamicActor(PHYSXCOLLIDERDESC& tPhysXColliderDesc, PxTransform Transform)
{
	m_pRigidDynamic = m_pGameInstance->Create_DynamicActor(Transform);
	Create_Geometry();

	// 지오메트리 개수만큼 셰이프를 만든다.
	for (size_t i = 0; i < m_pGeometry.size(); ++i)
	{
		PxShapeFlags shapeFlags;

		if (tPhysXColliderDesc.bTrigger)
		{
			shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
			m_FilterData.word2 = 1;

		}
		else
		{
			shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
			m_FilterData.word2 = 1;
		}

		PxShape* pShape = nullptr;

		m_pGameInstance->Create_Shape(*m_pGeometry[i], m_PhysXColliderDesc.pMaterial, false, shapeFlags, &pShape);

		m_FilterData.word0 = (1 << m_PhysXColliderDesc.iFilterType);
		m_FilterData.word1 = m_pGameInstance->Get_PhysXFilterGroup(m_PhysXColliderDesc.iFilterType);
		m_FilterData.word3 = m_iColliderIndex;

		//FilterData.word1 = m_PhysXColliderDesc.iFilterType;

		if (!pShape)
		{
			// Shape가 생성되지 않음.
			//DEBUG_ASSERT;
		}

		pShape->setSimulationFilterData(m_FilterData);

		m_pRigidDynamic->attachShape(*pShape);
		m_pRigidDynamic->userData = &m_iColliderIndex;

		m_pShape.push_back(pShape);
	}
}

void CPhysXCollider::Create_StaticActor(PHYSXCOLLIDERDESC& tPhysXColliderDesc, PxTransform Transform)
{
	m_pRigidStatic = m_pGameInstance->Create_StaticActor(Transform);
	Create_Geometry();


	// 지오메트리 개수만큼 셰이프를 만든다.
	for (size_t i = 0; i < m_pGeometry.size(); ++i)
	{

		PxShapeFlags shapeFlags;

		if (tPhysXColliderDesc.bTrigger)
		{
			shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
			m_FilterData.word2 = 1;
		}
		else
		{
			shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
			m_FilterData.word2 = 1;
		}

		PxShape* pShape = nullptr;

		m_pGameInstance->Create_Shape(*m_pGeometry[i], m_PhysXColliderDesc.pMaterial, false, shapeFlags, &pShape);

		m_FilterData.word0 = (1 << m_PhysXColliderDesc.iFilterType);
		m_FilterData.word1 = m_pGameInstance->Get_PhysXFilterGroup(m_PhysXColliderDesc.iFilterType);
		m_FilterData.word3 = m_iColliderIndex;

		if (!pShape)
		{
			// Shape가 생성되지 않음.
			//DEBUG_ASSERT;
		}

		pShape->setSimulationFilterData(m_FilterData);

		m_pRigidStatic->attachShape(*pShape);
		m_pRigidStatic->userData = &m_iColliderIndex;

		m_pShape.push_back(pShape);
	}
}

void CPhysXCollider::Free()
{
	Safe_Release(m_pGameInstance);
}
