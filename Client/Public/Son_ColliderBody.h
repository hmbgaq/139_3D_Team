#pragma once

#include "Client_Defines.h"
#include "Son_Weapon.h"

BEGIN(Client)


class CSon_ColliderBody : public CSon_Weapon
{
private:
	CSon_ColliderBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CSon_ColliderBody(const CSon_ColliderBody& rhs);
	virtual ~CSon_ColliderBody() = default;

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
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;
private:
	CMother* m_pMother = { nullptr };
	_bool m_bFirst = true;
public:
	static CSon_ColliderBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	virtual CGameObject* Clone(void* pArg) override;

	virtual CGameObject* Pool();

	virtual void Free() override;
};

END

