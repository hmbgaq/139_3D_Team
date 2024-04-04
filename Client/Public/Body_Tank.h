#pragma once

#include "Client_Defines.h"
#include "Body_Monster.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Tank : public CBody_Monster
{

protected:
	CBody_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Tank(const CBody_Tank& rhs);
	virtual ~CBody_Tank() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void Check_Frustum() override;

public:
	static CBody_Tank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END