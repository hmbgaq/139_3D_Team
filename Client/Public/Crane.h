#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;

END

BEGIN(Client)

class CCrane : public CGameObject
{
protected:
	CCrane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CCrane(const CCrane& rhs);
	virtual ~CCrane() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Tick(_float fTimeDelta) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;


public:
	virtual void		OnCollisionEnter(CCollider* other)	override;
	virtual void		OnCollisionStay(CCollider* other)	override;
	virtual void		OnCollisionExit(CCollider* other)	override;

protected:
	virtual HRESULT			Ready_Components();
	virtual HRESULT			Bind_ShaderResources();

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	//CCollider* m_pColliderCom = { nullptr };


public:
	static CCrane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END