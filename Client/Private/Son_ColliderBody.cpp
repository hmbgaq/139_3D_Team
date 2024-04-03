#include "stdafx.h"
#include "Son_ColliderBody.h"
#include "GameInstance.h"
#include "Character.h"
#include "Son.h"
#include "Player.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "Transform.h"
#include "Bone.h"
#include "Effect.h"
#include "Clone_Manager.h"
#include "Mother.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"

CSon_ColliderBody::CSon_ColliderBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CSon_Weapon(pDevice, pContext, strPrototypeTag)
{

}

CSon_ColliderBody::CSon_ColliderBody(const CSon_ColliderBody& rhs)
	: CSon_Weapon(rhs)
{
}

HRESULT CSon_ColliderBody::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSon_ColliderBody::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pMother = CData_Manager::GetInstance()->Get_Mother();

	return S_OK;
}

void CSon_ColliderBody::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSon_ColliderBody::Tick(_float fTimeDelta)
{
	/*auto start = chrono::high_resolution_clock::now();*/

	__super::Tick(fTimeDelta);   

// 	auto End = chrono::high_resolution_clock::now();
// 	chrono::duration<double> duration0 = End - start;
// 	cout << "Son ColliderBody 실행시간 : " << duration0.count() << endl;
}

void CSon_ColliderBody::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSon_ColliderBody::Render()
{
	/*auto start = chrono::high_resolution_clock::now();*/
	if (FAILED(__super::Render()))
		return E_FAIL;

// 	auto End = chrono::high_resolution_clock::now();
// 	chrono::duration<double> duration0 = End - start;
// 	cout << "Son ColliderBody 실행시간 : " << duration0.count() << endl;

	return S_OK;
}

HRESULT CSon_ColliderBody::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	//if (FAILED(__super::Ready_Components()))
	//	return E_FAIL;

	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.fRadius = { 2.5f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc)))
		return E_FAIL;


	return S_OK;
}

void CSon_ColliderBody::OnCollisionEnter(CCollider* other)
{
	CAttackObject* pAttackObject = Get_Target_AttackObject(other);

	if (pAttackObject != nullptr)
	{
		if (other->Get_Layer() == ECast(COLLISION_LAYER::PLAYER_ATTACK))
		{
			m_pMother->Get_Damaged(pAttackObject->Get_Damage());
		}
	}
}

void CSon_ColliderBody::OnCollisionStay(CCollider* other)
{
	CAttackObject* pAttackObject = Get_Target_AttackObject(other);

	if (pAttackObject != nullptr)
	{
		if (other->Get_Layer() == ECast(COLLISION_LAYER::PLAYER_ATTACK))
		{
			m_pMother->Get_Damaged(pAttackObject->Get_Damage());
		}
	}

	CCharacter* pTarget_Character = Get_Target_Character(other);
	if (nullptr != pTarget_Character)
	{
		_vector vTargetPos = pTarget_Character->Get_Position_Vector();

		pTarget_Character->Add_Force(Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1, 9.f * m_pGameInstance->Get_TimeDelta());
	}
}

void CSon_ColliderBody::OnCollisionExit(CCollider* other)
{
}

CSon_ColliderBody* CSon_ColliderBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CSon_ColliderBody* pInstance = new CSon_ColliderBody(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSon_ColliderBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon_ColliderBody::Clone(void* pArg)
{
	CSon_ColliderBody* pInstance = new CSon_ColliderBody(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSon_ColliderBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon_ColliderBody::Pool()
{
	return new CSon_ColliderBody(*this);
}

void CSon_ColliderBody::Free()
{
	__super::Free();

}
