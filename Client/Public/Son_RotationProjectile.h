#pragma once
#include "Projectile.h"

BEGIN(Client)
class CSon;

class CSon_RotationProjectile final : public CProjectile
{
private:
	CSon_RotationProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CSon_RotationProjectile(const CSon_RotationProjectile& rhs);
	virtual ~CSon_RotationProjectile() = default;
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
	_vector				m_vPlayerPos = {};
	_vector				m_vMonsterPos = {};
	_bool				m_bFirst = true;
	_float				m_fRadian = 0.f;
private:
	CEffect* m_pEffect = { nullptr };
	CSon* m_pSon = { nullptr };
public:
	static CSon_RotationProjectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END

