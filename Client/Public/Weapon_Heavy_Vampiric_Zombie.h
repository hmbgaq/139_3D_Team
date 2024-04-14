#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
END

BEGIN(Client)

class CWeapon_Heavy_Vampiric_Zombie final : public CWeapon
{
protected:
	CWeapon_Heavy_Vampiric_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CWeapon_Heavy_Vampiric_Zombie(const CWeapon_Heavy_Vampiric_Zombie& rhs);
	virtual ~CWeapon_Heavy_Vampiric_Zombie() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual	HRESULT Render_Shadow() override;

public:
	void Attack(CCollider* other);

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

public:
	void Play_Sound_Attack();

public:
	void	Play_Trail(_bool bPlay);

private:
	HRESULT			Load_Json();
	HRESULT			Option_Setting();

protected:
	virtual HRESULT Ready_Components();
	HRESULT			Bind_ShaderResources();
private:
	CEffect*				m_pEffect = { nullptr };
	class CEffect_Trail*	m_pTrail		= { nullptr };	//! 유정 : 트레일 추가
	class CEffect_Trail*	m_pTrail_Post	= { nullptr };	//! 유정 : 트레일 추가 // 포스트 디스토션 트레일

public:
	_bool m_bAttack = false;
public:
	static CWeapon_Heavy_Vampiric_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END