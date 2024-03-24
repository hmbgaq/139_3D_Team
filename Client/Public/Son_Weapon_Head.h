#pragma once

#include "Client_Defines.h"
#include "Son_Weapon.h"

BEGIN(Client)
class CEffect_Trail;

class CSon_Weapon_Head : public CSon_Weapon
{
private:
	CSon_Weapon_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CSon_Weapon_Head(const CSon_Weapon_Head& rhs);
	virtual ~CSon_Weapon_Head() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Ready_Components();

private:
	CEffect_Trail* m_pTrail = { nullptr };

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

public:
	static CSon_Weapon_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	virtual CGameObject* Clone(void* pArg) override;

	virtual CGameObject* Pool();

	virtual void Free() override;
};

END
