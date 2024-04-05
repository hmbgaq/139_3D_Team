#pragma once
#include "Projectile.h"

BEGIN(Client)
class CEffect_Trail;
class CEffect;

class CExplosion_TNTPack final : public CProjectile
{
private:
	CExplosion_TNTPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CExplosion_TNTPack(const CExplosion_TNTPack& rhs);
	virtual ~CExplosion_TNTPack() = default;
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


public:
	static CExplosion_TNTPack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END