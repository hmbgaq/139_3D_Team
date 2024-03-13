#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCharacter;
class CTransform;
class CCollider;
class CTexture;
class CShader;
class CModel;
END


BEGIN(Client)

class CProjectile abstract : public CGameObject
{

protected:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CProjectile(const CProjectile& rhs);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;


public:
	CCharacter* Get_Target_Character(CCollider * other);
	CProjectile* Set_Damage(_int _iDamage);
	CProjectile* Set_Direction(Direction _eHitDirection);
	CProjectile* Set_Power(Power _eHitPower);
	CProjectile* Set_Force(_float _fForce);


public:
	virtual void	OnCollisionEnter(CCollider* other)	PURE;
	virtual void	OnCollisionStay(CCollider* other)	PURE;
	virtual void	OnCollisionExit(CCollider* other)	PURE;


protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Bind_ShaderResources();

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pCollider = { nullptr };

protected:
	Direction	m_eHitDirection = { Direction::Front };
	Power		m_eHitPower = { Power::Light };
	_float		m_fStiffnessRate = { 1.f };
	_uint		m_iDamage = { 0 };
	_float		m_fForce = { 0.f };
	_bool		m_bUse = { true };

protected:
	_float		m_fLifeTime = { 10.f };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

};

END