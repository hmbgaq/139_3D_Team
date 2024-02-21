#pragma once

#include "Client_Defines.h"
#include "Weapon_Player.h"

BEGIN(Client)

class CPlayer_Weapon_Winchester : public CWeapon_Player
{
private:
	CPlayer_Weapon_Winchester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CPlayer_Weapon_Winchester(const CPlayer_Weapon_Winchester& rhs);
	virtual ~CPlayer_Weapon_Winchester() = default;

public:
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Ready_Components();
public:
	static CPlayer_Weapon_Winchester* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END