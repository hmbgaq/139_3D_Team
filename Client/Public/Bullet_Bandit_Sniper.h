#pragma once
#include "Projectile.h"

BEGIN(Client)
class CEffect_Trail;

class CBullet_Bandit_Sniper final : public CProjectile
{
private:
	CBullet_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBullet_Bandit_Sniper(const CBullet_Bandit_Sniper& rhs);
	virtual ~CBullet_Bandit_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override { return S_OK; }

public:
	virtual void		Set_Enable(_bool _Enable) override;

public:
	virtual void	OnCollisionEnter(CCollider* other);
	virtual void	OnCollisionStay(CCollider* other);
	virtual void	OnCollisionExit(CCollider* other);

	void			Bullet_Return_Pool() { Set_Enable(false); }
	void			Set_BulletDir(_float4 vDir) { m_vMoveDir = vDir; }
private:
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources();

private:
	CEffect_Trail*	m_pTrail = { nullptr };

private:
	_float4			m_vTargetPos = {};
	_float4			m_vMoveDir = {};

public:
	static CBullet_Bandit_Sniper* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END