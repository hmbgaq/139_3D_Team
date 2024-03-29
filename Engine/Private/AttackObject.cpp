#include "AttackObject.h"
#include "GameInstance.h"
#include "Character.h"

CAttackObject::CAttackObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CAttackObject::CAttackObject(const CAttackObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAttackObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAttackObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAttackObject::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAttackObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CAttackObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CAttackObject::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

CCharacter* CAttackObject::Get_Target_Character(CCollider* other)
{
	if (nullptr == other || nullptr == other->Get_Owner() || nullptr == other->Get_Owner()->Get_Object_Owner())
		return nullptr;

	CCharacter* pTarget_Character = dynamic_cast<CCharacter*>(other->Get_Owner()->Get_Object_Owner());
	if (nullptr == pTarget_Character)
		return nullptr;

	return pTarget_Character;
}

CAttackObject* CAttackObject::Get_Target_AttackObject(CCollider* other)
{
	if (nullptr == other || nullptr == other->Get_Owner())
		return nullptr;

	CAttackObject* pTargetAttackObject = dynamic_cast<CAttackObject*>(other->Get_Owner());
	if (nullptr == pTargetAttackObject)
		return nullptr;

	return pTargetAttackObject;
}

CCharacter* CAttackObject::Get_PartOwner()
{
	if (nullptr == m_pOwner || m_pOwner->Is_Dead() || false == m_pOwner->Get_Enable())
		return nullptr;

	return dynamic_cast<CCharacter*>(m_pOwner);
}

CAttackObject* CAttackObject::Set_Damage(_float _fDamage)
{
	m_fDamage = _fDamage;

	return this;
}

CAttackObject* CAttackObject::Set_Direction(Direction _eHitDirection)
{
	m_eHitDirection = _eHitDirection;

	return this;
}

CAttackObject* CAttackObject::Set_Power(Power _eHitPower)
{
	m_eHitPower = _eHitPower;

	return this;
}

CAttackObject* CAttackObject::Set_Force(_float _fForce)
{
	m_fForce = _fForce;

	return this;
}

CAttackObject* CAttackObject::Set_KnockUp(_bool _bKnockUp)
{
	m_bKnockUp = _bKnockUp;

	return this;
}


void CAttackObject::Free()
{
	__super::Free();
}
