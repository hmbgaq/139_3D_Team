#pragma once

#include "Body_Infected.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Infected_B : public CBody_Infected
{

private:
	CBody_Infected_B(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Infected_B(const CBody_Infected_B& rhs);
	virtual ~CBody_Infected_B() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_CSM(_uint i) override;

private:
	HRESULT OptionSetting();
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Infected_B* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END