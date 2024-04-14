#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CCharacter;
class CCollider;

class ENGINE_DLL CAttackObject abstract : public CGameObject
{

protected:
	CAttackObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CAttackObject(const CAttackObject& rhs);
	virtual ~CAttackObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CGameObject* Get_Target_Object(CCollider * other);
	CCharacter* Get_Target_Character(CCollider * other);
	CAttackObject* Get_Target_AttackObject(CCollider * other);

	CCharacter* Get_PartOwner();

	CAttackObject* Set_Damage(_float _iDamage);
	CAttackObject* Set_Direction(Direction _eHitDirection);
	CAttackObject* Set_Power(Power _eHitPower);
	CAttackObject* Set_Force(_float _fForce);
	CAttackObject* Set_KnockUp(_bool _bKnockUp);


public:
	_float Get_Damage() { return m_fDamage; }
	void Refill_Hp() { m_fHp = m_fMaxHp; };
	_bool Is_Melee() { return m_bIsMelee; };


public:
	void Get_Damaged(_float _fDamage) { m_fHp -= _fDamage; };
	_float Get_Hp() { return m_fHp; };

protected:
	Direction	m_eHitDirection = { Direction::Front };
	Power		m_eHitPower = { Power::Light };
	_float		m_fStiffnessRate = { 1.f };

	_float		m_fDamage = { 0.f };
	_float		m_fForce = { 0.f };
	_bool		m_bKnockUp = { false };

	_float		m_fHp = { 1.f };
	_float		m_fMaxHp = { 1.f };

	_bool		m_bIsMelee = { false };


public:
	virtual void Free() override;

};

END