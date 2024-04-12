#pragma once
#include "Projectile.h"

BEGIN(Client)
class CMother;

class CMotherShakeTreeProjectile final : public CProjectile
{
private:
	CMotherShakeTreeProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMotherShakeTreeProjectile(const CMotherShakeTreeProjectile& rhs);
	virtual ~CMotherShakeTreeProjectile() = default;
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

	_float				m_fRadian = 0.f;
	_bool				m_bFirst = true;
private:
	CEffect* m_pEffect = { nullptr };

	CMother* m_pMother = { nullptr };
public:
	static CMotherShakeTreeProjectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END
