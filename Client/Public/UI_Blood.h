#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Blood : public CUI
{
private:
	CUI_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Blood(const CUI& rhs);
	virtual ~CUI_Blood() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	Ready_UI();

public:
	static  CUI_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END