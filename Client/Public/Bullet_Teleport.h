#pragma once
#include "Projectile.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CEffect_Trail;
class CEffect;

class CBullet_Teleport final : public CProjectile
{
private:
	CBullet_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBullet_Teleport(const CBullet_Teleport& rhs);
	virtual ~CBullet_Teleport() = default;
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
	virtual HRESULT		Set_InitPosition(const _float3& vPos) override;

private:
	CEffect* m_pEffect = { nullptr };

private:
	CEffect_Trail* m_pTrail = { nullptr };

private:
	CNavigation* m_pNavigationCom = { nullptr };


public:
	static CBullet_Teleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END