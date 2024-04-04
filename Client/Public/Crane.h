#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
//class CShader;
//class CModel;
//class CCollider;
END

BEGIN(Client)

class CCrane : public CCharacter
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

protected:
	HRESULT				Ready_Components();
	HRESULT				Ready_PartObjects();

	//virtual HRESULT			Bind_ShaderResources();

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