#pragma once
#include "Component.h"

BEGIN(Engine)

class CGameInstance;

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


private:
	CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXCollider(const CPhysXCollider& rhs);
	virtual ~CPhysXCollider() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;

public:
	_uint	Get_PColliderIndex() const { return m_iColliderIndex; }

public:
	void		CreatePhysXActor(PHYSXCOLLIDERDESC& PhysXColliderDesc);

//public:
//	CPhysXCollider* CallBack_CollisionEnter;
//	CPhysXCollider* CallBack_CollisionStay;
//	CPhysXCollider* CallBack_CollisionExit;

private:
	static	_uint		m_iClonedColliderIndex;
	_uint				m_iColliderIndex = { 0 };

private:
	PHYSXCOLLIDERDESC		m_PhysXColliderDesc;

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	void Free();
};

END