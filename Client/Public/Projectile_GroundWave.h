#pragma once
#pragma once
#include "Projectile.h"

BEGIN(Client)
class CEffect_Trail;
class CEffect;

class CProjectile_GroundWave final : public CProjectile
{
private:
	CProjectile_GroundWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CProjectile_GroundWave(const CProjectile_GroundWave& rhs);
	virtual ~CProjectile_GroundWave() = default;
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
	static CProjectile_GroundWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END