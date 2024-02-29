#include "..\Public\PhysXCollider.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "MyAIMesh.h"
#include "MyAIScene.h"
#include "SMath.h"


_uint CPhysXCollider::m_iClonedColliderIndex = 0;

CPhysXCollider::CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CPhysXCollider::CPhysXCollider(const CPhysXCollider& rhs)
	: CComponent(rhs)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
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

void CPhysXCollider::Create_Collider()
{
	if (!m_pRigidDynamic && !m_pRigidStatic)
	{
		CreatePhysXActor(m_PhysXColliderDesc);
	}
}

void CPhysXCollider::Delete_Collider()
{
	if (m_pRigidDynamic)
	{
		m_pRigidDynamic->release();
		m_pRigidDynamic = nullptr;
	}

	if (m_pRigidStatic)
	{
		m_pRigidStatic->release();
		m_pRigidStatic = nullptr;
	}
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

void CPhysXCollider::Add_PhysXActorAtSceneWithOption(const PxVec3& In_MassSpaceInertiaTensor, const PxReal In_fMass)
{
	if (m_pRigidDynamic && m_pRigidStatic)
	{
		// 둘 다 존재하면 안된다.
		//DEBUG_ASSERT;
	}

	else if (m_pRigidDynamic)
	{
		// 저항값
		m_pRigidDynamic->setAngularDamping(0.5f);
		m_pRigidDynamic->setMass(In_fMass);
		m_pRigidDynamic->setLinearDamping(0.f);
		// 속도
		//DynamicActor.setLinearVelocity(velocity);
		m_pRigidDynamic->setMassSpaceInertiaTensor(In_MassSpaceInertiaTensor);
		//m_pRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);

		m_pGameInstance->Add_DynamicActorAtCurrentScene(*m_pRigidDynamic);
	}

	else if (m_pRigidStatic)
	{
		m_pGameInstance->Add_StaticActorAtCurrentScene(*m_pRigidStatic);
	}

	else
	{
		// 생성된 PhysXActor가 없음. Create부터 할 것.
#ifdef _DEBUG
		cout << "No Actor was created." << endl;
#endif // _DEBUG
	}

	for (auto& elem : m_pGeometry)
	{
		Safe_Delete(elem);
	}
}

void CPhysXCollider::Add_PhysXActorAtScene()
{
	if (m_pRigidDynamic && m_pRigidStatic)
	{
		// 둘 다 존재하면 안된다.
		//DEBUG_ASSERT;
	}

	else if (m_pRigidDynamic)
	{
		m_pGameInstance->Add_DynamicActorAtCurrentScene(*m_pRigidDynamic);
	}

	else if (m_pRigidStatic)
	{
		m_pGameInstance->Add_StaticActorAtCurrentScene(*m_pRigidStatic);
	}

	else
	{
		// 생성된 PhysXActor가 없음. Create부터 할 것.
#ifdef _DEBUG
		cout << "No Actor was created." << endl;
#endif // _DEBUG

	}

	for (auto& elem : m_pGeometry)
	{
		Safe_Delete(elem);
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

void CPhysXCollider::PhysXCollisionEnter(CPhysXCollider* pOtherCollider)
{
	m_pOwner->OnPhysXCollisionEnter(pOtherCollider);
}

void CPhysXCollider::PhysXCollisionStay(CPhysXCollider* pOtherCollider)
{
	m_pOwner->OnPhysXCollisionStay(pOtherCollider);
}

void CPhysXCollider::PhysXCollisionExit(CPhysXCollider* pOtherCollider)
{
	m_pOwner->OnPhysXCollisionExit(pOtherCollider);
}

void CPhysXCollider::Init_MeshCollider(CMyAIMesh* pMeshData, const vector<INSTANCE_MESH_DESC>* In_ParticleDescs)
{
	PxU32 iNumVertices;
	PxU32 iNumFaces;
	PxVec3* pVertices;
	FACEINDICES32* pIndices;
	PxTriangleMeshDesc meshDesc;

	if (In_ParticleDescs)
	{
		for (size_t i = 0; i < In_ParticleDescs->size(); ++i)
		{
			iNumVertices = pMeshData->Get_NumVertices();
			iNumFaces = pMeshData->Get_NumFaces();
			pVertices = new PxVec3[iNumVertices];
			pIndices = new FACEINDICES32[iNumFaces];

			SMath::Convert_PxVec3FromMeshDataWithTransformMatrix(pVertices, pMeshData, (*In_ParticleDescs)[i].Get_Matrix());

			meshDesc.points.count = iNumVertices;
			meshDesc.points.stride = sizeof(PxVec3);
			meshDesc.points.data = pVertices;

			for (PxU32 i = 0; i < iNumFaces; ++i)
			{
				memcpy(&pIndices[i], &pMeshData->Get_Face(i), sizeof(FACEINDICES32));
			}

			meshDesc.triangles.count = iNumFaces;
			meshDesc.triangles.stride = 3 * sizeof(PxU32);
			meshDesc.triangles.data = pIndices;

			m_TriangleMesh.push_back(nullptr);

			m_pGameInstance->Create_MeshFromTriangles(meshDesc, &m_TriangleMesh.back());

			Safe_Delete_Array(pVertices);
			Safe_Delete_Array(pIndices);
		}

	}
	else
	{
		iNumVertices = pMeshData->Get_NumVertices();
		iNumFaces = pMeshData->Get_NumFaces();
		pVertices = new PxVec3[iNumVertices];
		pIndices = new FACEINDICES32[iNumFaces];

		SMath::Convert_PxVec3FromMeshData(pVertices, pMeshData);

		meshDesc.points.count = iNumVertices;
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = pVertices;

		for (PxU32 i = 0; i < iNumFaces; ++i)
		{
			memcpy(&pIndices[i], &pMeshData->Get_Vertice(i), sizeof(FACEINDICES32));
		}

		meshDesc.triangles.count = iNumFaces;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = pIndices;

		m_TriangleMesh.push_back(nullptr);

		m_pGameInstance->Create_MeshFromTriangles(meshDesc, &m_TriangleMesh.back());

		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
	}

}

void CPhysXCollider::Init_ConvexMeshCollider(CMyAIMesh* pMeshData, const vector<INSTANCE_MESH_DESC>* In_ParticleDescs)
{
	PxU32 iNumVertices;
	PxVec3* pVertices;
	PxConvexMeshDesc meshDesc;

	if (In_ParticleDescs)
	{
		for (size_t i = 0; i < In_ParticleDescs->size(); ++i)
		{
			iNumVertices = pMeshData->Get_NumVertices();
			pVertices = new PxVec3[iNumVertices];

			SMath::Convert_PxVec3FromMeshDataWithTransformMatrix(pVertices, pMeshData, (*In_ParticleDescs)[i].Get_Matrix());

			meshDesc.points.count = iNumVertices;
			meshDesc.points.stride = sizeof(PxVec3);
			meshDesc.points.data = pVertices;

			meshDesc.flags |= PxConvexFlag::Enum::ePLANE_SHIFTING;
			meshDesc.flags |= PxConvexFlag::Enum::eGPU_COMPATIBLE;
			meshDesc.flags |= PxConvexFlag::Enum::eCOMPUTE_CONVEX;

			m_ConvexMeshes.push_back(nullptr);

			m_pGameInstance->Create_ConvexMesh(meshDesc, &m_ConvexMeshes.back());

			Safe_Delete_Array(pVertices);
		}
	}

	else
	{
		iNumVertices = pMeshData->Get_NumVertices();
		pVertices = new PxVec3[iNumVertices];

		SMath::Convert_PxVec3FromMeshData(pVertices, pMeshData);

		meshDesc.points.count = iNumVertices;
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = pVertices;

		meshDesc.flags |= PxConvexFlag::Enum::ePLANE_SHIFTING;
		meshDesc.flags |= PxConvexFlag::Enum::eGPU_COMPATIBLE;
		meshDesc.flags |= PxConvexFlag::Enum::eCOMPUTE_CONVEX;

		m_ConvexMeshes.push_back(nullptr);

		m_pGameInstance->Create_ConvexMesh(meshDesc, &m_ConvexMeshes.back());

		Safe_Delete_Array(pVertices);
	}
}

void CPhysXCollider::Init_ModelCollider(CMyAIScene* pModelData, const _bool In_isConvex)
{
	if (nullptr == pModelData)
	{
		return;
	}

	for (_uint i = 0; i < pModelData->Get_NumMeshes(); ++i)
	{
		if (In_isConvex)
		{
			Init_ConvexMeshCollider(&pModelData->Get_Mesh(i));
		}
		else
		{
			Init_MeshCollider(&pModelData->Get_Mesh(i));
		}
	}
}

void CPhysXCollider::Init_ModelInstanceCollider(CMyAIScene* pModelData, const vector<INSTANCE_MESH_DESC>& In_ParticleDescs, const _bool In_isConvex)
{
	if (nullptr == pModelData)
	{
		DEBUG_ASSERT;
	}

	for (_uint i = 0; i < pModelData->Get_NumMeshes(); ++i)
	{
		if (In_isConvex)
		{
			Init_ConvexMeshCollider(&(pModelData->Get_Mesh(i)), &In_ParticleDescs);
		}
		else
		{
			Init_MeshCollider(&pModelData->Get_Mesh(i), &In_ParticleDescs);
		}
	}
}

void CPhysXCollider::Synchronize_Collider(CTransform* pTransform, _fvector In_vOffset)
{
	_vector vPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	vPos += In_vOffset;
	vPos.m128_f32[3] = 1.f;
	_vector vQuaternion = XMQuaternionRotationMatrix(SMath::Get_RotationMatrix(pTransform->Get_WorldMatrix()));
	Set_Position(vPos, vQuaternion);
}

void CPhysXCollider::PutToSleep()
{
	if (m_pRigidDynamic)
		m_pRigidDynamic->putToSleep();
}

void CPhysXCollider::WakeUp()
{
	if (m_pRigidDynamic)
		m_pRigidDynamic->wakeUp();
}

HRESULT CPhysXCollider::Set_Position(_vector _vPos, _vector _vQuaternion)
{
	PxTransform	Transform;
	PxVec3	vPos(XMVectorGetX(_vPos), XMVectorGetY(_vPos), XMVectorGetZ(_vPos));
	PxQuat	vQuaternion(XMVectorGetX(_vQuaternion), XMVectorGetY(_vQuaternion), XMVectorGetZ(_vQuaternion), XMVectorGetW(_vQuaternion));

	Transform.p = vPos;
	Transform.q = vQuaternion;

	if (m_pRigidDynamic)
		m_pRigidDynamic->setGlobalPose(Transform);

	if (m_pRigidStatic)
		m_pRigidStatic->setGlobalPose(Transform);

	return S_OK;
}

HRESULT CPhysXCollider::Set_Position(_vector _vPos)
{
	PxTransform	Transform;
	PxVec3	vPos(XMVectorGetX(_vPos), XMVectorGetY(_vPos), XMVectorGetZ(_vPos));

	if (m_pRigidDynamic)
		Transform = m_pRigidDynamic->getGlobalPose();

	if (m_pRigidStatic)
		Transform = m_pRigidStatic->getGlobalPose();

	Transform.p = vPos;

	if (m_pRigidDynamic)
		m_pRigidDynamic->setGlobalPose(Transform);

	if (m_pRigidStatic)
		m_pRigidStatic->setGlobalPose(Transform);

	return S_OK;
}

CPhysXCollider* CPhysXCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysXCollider* pInstance = new CPhysXCollider(pDevice, pContext);

	return pInstance;
}

CComponent* CPhysXCollider::Clone(void* pArg)
{

	CPhysXCollider* pInstance = new CPhysXCollider(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysXCollider::Free()
{
	Safe_Release(m_pGameInstance);
}
