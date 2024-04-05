#pragma once

#include "Client_Defines.h"
#include "DestructableProp.h"

BEGIN(Engine)
END

BEGIN(Client)

class CTNTCrate final : public CDestructableProp
{
private:
	CTNTCrate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CTNTCrate(const CTNTCrate& rhs);
	virtual ~CTNTCrate() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;

public:
	void Explode();
	void OnCollision(CCollider* other);

public: 
	virtual void		OnCollisionEnter(CCollider* other)	override;
	virtual void		OnCollisionStay(CCollider* other)	override;
	virtual void		OnCollisionExit(CCollider* other)	override;

protected:
	virtual HRESULT		Ready_Components() override;


public:
	static CTNTCrate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END