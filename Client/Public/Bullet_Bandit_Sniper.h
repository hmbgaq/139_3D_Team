#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CBullet_Bandit_Sniper final : public CGameObject
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

private:
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources();

private:
	CCollider* m_pCollider = { nullptr };

public:
	static CBullet_Bandit_Sniper* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END