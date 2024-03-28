#include "stdafx.h"
#include "Weapon_Punch_Tank.h"
#include "GameInstance.h"
#include "Character_Client.h"
#include "Player.h"
#include "Data_Manager.h"
#include "MasterCamera.h"

CWeapon_Punch_Tank::CWeapon_Punch_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CWeapon_Punch_Tank::CWeapon_Punch_Tank(const CWeapon_Punch_Tank& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CWeapon_Punch_Tank::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Punch_Tank::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fDamage = 5.f;

	return S_OK;
}

void CWeapon_Punch_Tank::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Punch_Tank::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Punch_Tank::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Punch_Tank::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Punch_Tank::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	BoundingDesc.fRadius = { 1.6f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Punch_Tank::OnCollisionEnter(CCollider* other)
{
	if (ECast(COLLISION_LAYER::PLAYER_PARRYING) == other->Get_Layer())
	{
		return;
	}

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (pTarget_Character != nullptr)
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.5f;

		_vector vTargetPos = pTarget_Character->Get_Position_Vector();
		_vector vHitDir = Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1;

		pTarget_Character->Set_Hitted(m_fDamage, vHitDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);

		Activate_Collisions(false);
	}
}

void CWeapon_Punch_Tank::OnCollisionStay(CCollider* other)
{
	if (ECast(COLLISION_LAYER::PLAYER_PARRYING) == other->Get_Layer())
	{
		return;
	}

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (pTarget_Character != nullptr)
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.5f;

		_vector vTargetPos = pTarget_Character->Get_Position_Vector();
		_vector vHitDir = Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1;

		pTarget_Character->Set_Hitted(m_fDamage, vHitDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);

		Activate_Collisions(false);
	}
}

void CWeapon_Punch_Tank::OnCollisionExit(CCollider* other)
{
	_int a = 0;

}

CWeapon_Punch_Tank* CWeapon_Punch_Tank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Punch_Tank* pInstance = new CWeapon_Punch_Tank(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Punch_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Punch_Tank::Clone(void* pArg)
{
	CWeapon_Punch_Tank* pInstance = new CWeapon_Punch_Tank(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Punch_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Punch_Tank::Pool()
{
	return new CWeapon_Punch_Tank(*this);
}

void CWeapon_Punch_Tank::Free()
{
	__super::Free();
}
