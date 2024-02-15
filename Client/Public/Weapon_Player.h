#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)

class CWeapon_Player final : public CWeapon
{
private:
	CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Player(const CWeapon_Player& rhs);
	virtual ~CWeapon_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(LEVEL eLevel);
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CWeapon_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END