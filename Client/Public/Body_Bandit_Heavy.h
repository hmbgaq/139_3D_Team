#pragma once

#include "Client_Defines.h"
#include "Body.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Bandit_Heavy abstract : public CBody
{

protected:
	CBody_Bandit_Heavy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Bandit_Heavy(const CBody_Bandit_Heavy& rhs);
	virtual ~CBody_Bandit_Heavy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;



protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();




public:
	/* 원형객체를 생성한다. */
	//static CBody_Bandit_Heavy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	//virtual CGameObject* Clone(void* pArg) override;
	//virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END