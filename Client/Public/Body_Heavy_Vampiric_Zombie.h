#pragma once

#include "Client_Defines.h"
#include "Body_Bandit_Heavy.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Heavy_Vampiric_Zombie final : public CBody_Bandit_Heavy
{

protected:
	CBody_Heavy_Vampiric_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Heavy_Vampiric_Zombie(const CBody_Heavy_Vampiric_Zombie& rhs);
	virtual ~CBody_Heavy_Vampiric_Zombie() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_CSM(_uint i) override;

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void Check_Frustum() override;


public:
	static CBody_Heavy_Vampiric_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END