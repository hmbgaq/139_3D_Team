#pragma once
#include "Weapon_Infected.h"

BEGIN(Engine)
END

BEGIN(Client)

class CWeapon_Infected_D : public CWeapon_Infected
{
protected:
	CWeapon_Infected_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CWeapon_Infected_D(const CWeapon_Infected_D& rhs);
	virtual ~CWeapon_Infected_D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual	HRESULT Render_Shadow() override { return S_OK; };

protected:
	virtual HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Infected_D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END