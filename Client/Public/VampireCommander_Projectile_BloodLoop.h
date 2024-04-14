#pragma once
#include "Projectile.h"

BEGIN(Client)

class CVampireCommander_Projectile_BloodLoop final : public CProjectile
{
private:
	CVampireCommander_Projectile_BloodLoop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CVampireCommander_Projectile_BloodLoop(const CVampireCommander_Projectile_BloodLoop& rhs);
	virtual ~CVampireCommander_Projectile_BloodLoop() = default;
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

private:
	CCharacter* m_pMonster = { nullptr };
	CEffect* m_pEffect = { nullptr };
	_float				m_fEffectTimeAcc = { 0.f };

public:
	static CVampireCommander_Projectile_BloodLoop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END
