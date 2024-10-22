#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CHitPoint_Crane final : public CWeapon
{
protected:
	CHitPoint_Crane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CHitPoint_Crane(const CHitPoint_Crane& rhs);
	virtual ~CHitPoint_Crane() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

protected:
	virtual HRESULT Ready_Components();

public:
	static CHitPoint_Crane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END