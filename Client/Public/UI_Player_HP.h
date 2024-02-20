#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Player_HP : public CUI
{
private:
	CUI_Player_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_Player_HP(const CUI& rhs);
	virtual ~CUI_Player_HP() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	Ready_UI();

public:
	static  CUI_Player_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

	// CUI을(를) 통해 상속됨
	
};

END