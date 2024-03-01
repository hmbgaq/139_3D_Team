#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CCharacter;
class CCollider;
class CTexture;
class CShader;
class CModel;
class CBone;

class ENGINE_DLL CWeapon abstract : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		class CBone* m_pSocketBone = { nullptr };
		class CTransform* m_pParentTransform = { nullptr };

	}WEAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	void Attack(CCharacter* pCharacter, _float3 vLocalPos);

public:
	_float4x4	Get_WeaponWorldMatrix() { return m_WorldMatrix; }
	void		Set_WeaponWorldMatrix(_float4x4 _WorldMatrix) { m_WorldMatrix = _WorldMatrix; }

public:
	void Set_Use(_bool _bUse) {
		m_bUse = _bUse;
	}

	_bool Get_Use() {
		return m_bUse;
	}

public:




protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Bind_ShaderResources();



protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	//CTexture* m_pDissolveTexture = { nullptr };
	

protected:
	CTransform* m_pParentTransform = { nullptr };
	CBone* m_pSocketBone = { nullptr };

	_float4x4	m_WorldMatrix = {};

	//CCollider* m_pCollider = { nullptr };

protected:
	vector<CCollider*> m_pColliders = { nullptr };
	_uint	m_iColliderSize = { 0 };
	_bool	m_bIsAttack = { false };

	Direction	m_eHitDirection = { Direction::Front };
	Power		m_eHitPower = { Power::Light };
	_float		m_fStiffnessRate = { 1.f };

	_uint		m_iDamage = { 0 };
	_float3		m_vForce = { 0.f, 0.f, 0.f };

	_bool		m_bUse = { true };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

};

END