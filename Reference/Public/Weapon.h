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
	_float4x4	Get_WeaponWorldMatrix() { return m_WorldMatrix; }
	void		Set_WeaponWorldMatrix(_float4x4 _WorldMatrix) { m_WorldMatrix = _WorldMatrix; }
	_float3		Get_Weapon_Pos() { return { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 }; }

	CCharacter* Get_Target_Character(CCollider* other);
	CCharacter* Get_PartOwner();

	CWeapon* Set_Damage(_float _iDamage);
	CWeapon* Set_Direction(Direction _eHitDirection);
	CWeapon* Set_Power(Power _eHitPower);
	CWeapon* Activate_Collisions(_bool _bActivate);
	CWeapon* Set_Force(_float _fForce);
	CWeapon* Set_Dir(_float3 _vDir);
	CWeapon* Set_KnockUp(_bool _bKnockUp);


public:
	void	Set_Enable_Collisions(_bool bEnable);

	_float3	Get_WorldPosition();
	_float3 Get_MuzzlePos();

public:
	void Set_Colliders(vector<CCollider*> _pColliders) { m_pColliders = _pColliders; }
	vector<CCollider*> Get_Colliders() { return m_pColliders; }

public:
	void	Set_Synced(_bool _bSynced) { m_bSynced = _bSynced; }
	_bool	Get_Synced() { return m_bSynced; }

public:
	_float3 Calc_Front_Pos(_float3 vDiff = _float3(0.f, 0.f, 1.f));

public:
	void Fire(const wstring& strBulletTag, const wstring& strLayerTag, _float3 vTargetPos = _float3(0.f, 0.f, 1.f));
	virtual void Fire(_float3 vTargetPos = _float3(0.f, 0.f, 1.f)) {};

public:
	void Set_Follow(_bool _bIsFollow) { m_bIsFollow = _bIsFollow; };
	_bool Get_Follow() { return m_bIsFollow; }


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
	_float3 m_vMuzzlePos_Local = { 0.f, 0.f, 0.f };

protected:
	vector<CCollider*> m_pColliders = { nullptr };
	_uint	m_iColliderSize = { 0 };

	Direction	m_eHitDirection = { Direction::Front };
	Power		m_eHitPower = { Power::Light };
	_float		m_fStiffnessRate = { 1.f };

	_float		m_fDamage = { 0.f };
	_float		m_fForce = { 0.f };
	_float3		m_vDir = { 0.f, 0.f, 0.f };
	_bool		m_bKnockUp = { false };

protected://몬스터가 Player 잡아서 던지는거 쓸때 쓰는거
	_bool	m_bSynced = false;

protected:
	_bool m_bIsFollow = { true };



public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

};

END