#pragma once
#include "Component.h"
#include "Delegate.h"

BEGIN(Engine)

class CGameInstance;
class CMyAIMesh;
class CMyAIScene;

class ENGINE_DLL CPhysXCollider final : public CComponent
{
public:
	typedef struct PhysXColliderDesc
	{
		PhysXColliderDesc()
		{
			ZeroMemory(this, sizeof(PhysXColliderDesc));
		}

		PxConvexMesh* pConvecMesh;
		PxMaterial* pMaterial;
		PHYSXACTOR_TYPE		eActorType;
		XMVECTOR			vPosition;
		XMVECTOR			vAngles;
		PHYSXCOLLIDER_TYPE	eShape;
		_uint				iFilterType;
		XMVECTOR			vScale;
		float				fDensity;
		_bool				bTrigger;

	}PHYSXCOLLIDERDESC;


	struct INSTANCE_MESH_DESC
	{
		_float3         vRotation;
		_float3         vScale;
		_float3			vTarnslation;
		_float			fMaxRange;
		_float3			vCenter;

		void Reset()
		{
			ZeroMemory(this, sizeof(INSTANCE_MESH_DESC));
		}

		_matrix Get_Matrix() const
		{
			_matrix TransformationMatrix;
			_matrix RotationMatrix, ScaleMatrix;

			_vector vPitchYawRoll;
			_vector vPosition;

			vPitchYawRoll = XMLoadFloat3(&vRotation);
			vPosition = XMVectorSetW(XMLoadFloat3(&vTarnslation), 1.f);

			RotationMatrix = XMMatrixRotationRollPitchYawFromVector(vPitchYawRoll);
			ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
			TransformationMatrix = ScaleMatrix * RotationMatrix;
			TransformationMatrix.r[3] = vPosition;

			return TransformationMatrix;
		}

		void	Bake_CenterWithMatrix()
		{
			_vector vCenterFromVector = XMLoadFloat3(&vCenter);
			XMStoreFloat3(&vCenter, XMVector3TransformCoord(vCenterFromVector, Get_Matrix()));
		}
	};


private:
	CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXCollider(const CPhysXCollider& rhs);
	virtual ~CPhysXCollider() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;

public:
	_vector	Get_Position();
	_vector	Get_Quaternion();
	_vector	Get_Velocity();
	_float	Get_Mess();
	_vector	Get_AngularVelocity();
	_vector	Get_LinearVelocity();
	_matrix Get_WorldMatrix();

	_uint	Get_PColliderIndex() const { return m_iColliderIndex; }
	PHYSXCOLLIDERDESC Get_ColliderDesc() const { return m_PhysXColliderDesc; }


public:
	void	Create_Collider();
	void	Delete_Collider();

	void	CreatePhysXActor(PHYSXCOLLIDERDESC& PhysXColliderDesc);
	void	Add_PhysXActorAtSceneWithOption(const PxVec3& In_MassSpaceInertiaTensor = { 0.f, 0.f, 0.f }, const PxReal In_fMass = 0.f);
	void	Add_PhysXActorAtScene();

	void	Create_Geometry();
	void	Create_DynamicActor(PHYSXCOLLIDERDESC& PhysXColliderDesc, PxTransform Transform);
	void	Create_StaticActor(PHYSXCOLLIDERDESC& PhysXColliderDesc, PxTransform Transform);


public:
	void	OnCollision(CPhysXCollider* pOtherCollider);
	void	End_CollisionCheck();
	void	PhysXCollisionEnter(CPhysXCollider* pOtherCollider);
	void	PhysXCollisionStay(CPhysXCollider* pOtherCollider);
	void	PhysXCollisionExit(CPhysXCollider* pOtherCollider);

	virtual void Set_Enable(_bool _Enable) override;


public:


	void	Init_MeshCollider(CMyAIMesh* pMeshData, const vector<INSTANCE_MESH_DESC>* In_ParticleDescs = nullptr);
	void	Init_ConvexMeshCollider(CMyAIMesh* pMeshData, const vector<INSTANCE_MESH_DESC>* In_ParticleDescs = nullptr);
	void	Init_ModelCollider(CMyAIScene* pModelData, const _bool In_isConvex);
	void	Init_ModelInstanceCollider(CMyAIScene* pModelData, const vector<INSTANCE_MESH_DESC>& In_ParticleDescs, const _bool In_isConvex);


public:
	void	Synchronize_Collider(CTransform* pTransform, _fvector In_vOffset = { 0.f, 0.f, 0.f });

public:
	void	PutToSleep();
	void	WakeUp();


public:
	HRESULT Set_Position(_vector _vPos, _vector _vQuaternion);
	HRESULT Set_Position(_vector _vPos);


//public:
	FDelegate<CPhysXCollider*> CallBack_CollisionEnter;
	FDelegate<CPhysXCollider*> CallBack_CollisionStay;
	FDelegate<CPhysXCollider*> CallBack_CollisionExit;

private:
	static	_uint		m_iClonedColliderIndex;
	_uint				m_iColliderIndex = { 0 };

private:
	PHYSXCOLLIDERDESC		m_PhysXColliderDesc;
	PxRigidDynamic*			m_pRigidDynamic = { nullptr };
	PxRigidStatic*			m_pRigidStatic = { nullptr };

	vector<PxGeometry*>		m_pGeometry;
	vector<PxShape*>		m_pShape;
	PxFilterData			m_FilterData;

	vector<PxConvexMesh*>	m_ConvexMeshes;
	vector<PxTriangleMesh*>	m_TriangleMesh;


private: /* For. Collision */
	_bool								m_isColl = false;
	list<CPhysXCollider*>				m_pPreOtherColliders;
	list<CPhysXCollider*>				m_pOtherColliders;

	

private:
	CGameInstance* m_pGameInstance = { nullptr };

public:
	static CPhysXCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	void Free();
};

END