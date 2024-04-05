#pragma once
#include "Projectile.h"

BEGIN(Client)
class CEffect_Trail;
class CEffect;

class CBullet_Arrow_Electric final : public CProjectile
{
private:
	CBullet_Arrow_Electric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBullet_Arrow_Electric(const CBullet_Arrow_Electric& rhs);
	virtual ~CBullet_Arrow_Electric() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void	OnCollisionEnter(CCollider* other);
	virtual void	OnCollisionStay(CCollider* other);
	virtual void	OnCollisionExit(CCollider* other);

public:
	virtual HRESULT Ready_Components();

public:
	CEffect* m_pEffect = { nullptr };


private:
	CEffect_Trail* m_pTrail = { nullptr };


public:
	static CBullet_Arrow_Electric* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END