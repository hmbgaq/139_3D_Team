#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;

END

BEGIN(Client)

class CDestructableProp abstract : public CGameObject
{
protected:
	CDestructableProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CDestructableProp(const CDestructableProp& rhs);
	virtual ~CDestructableProp() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;


public:
	virtual void		OnCollisionEnter(CCollider* other)	PURE;
	virtual void		OnCollisionStay(CCollider* other)	PURE;
	virtual void		OnCollisionExit(CCollider* other)	PURE;

protected:
	virtual HRESULT			Ready_Components() PURE;
	virtual HRESULT			Bind_ShaderResources();

protected:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };


public:
	//static CDestructableProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;
	virtual void Free() override;

};

END