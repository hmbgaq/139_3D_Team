#pragma once

#include "Body_Infected.h"

BEGIN(Client)

class CBody_Infected_A : public CBody_Infected
{

private:
	CBody_Infected_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Infected_A(const CBody_Infected_A& rhs);
	virtual ~CBody_Infected_A() = default;

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
	static CBody_Infected_A* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END