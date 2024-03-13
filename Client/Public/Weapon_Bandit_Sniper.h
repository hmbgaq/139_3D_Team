#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
END

BEGIN(Client)

class CWeapon_Bandit_Sniper final : public CWeapon
{
protected:
	CWeapon_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CWeapon_Bandit_Sniper(const CWeapon_Bandit_Sniper& rhs);
	virtual ~CWeapon_Bandit_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual	HRESULT Render_Shadow() override { return S_OK; };

	/* น฿ป็ */
public:
	void Sniping(_float4 vDir, _float3 fPos);


protected:
	virtual HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Bandit_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END