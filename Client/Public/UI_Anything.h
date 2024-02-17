#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_Anything final : public CUI
{
private:
	CUI_Anything(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Anything(const CUI_Anything& rhs);
	virtual ~CUI_Anything() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

private:
	void					Compute_OwnerCamDistance();
	_bool					In_Frustum();

public:
	json				 Save_Desc(json& out_json);
	void				 Load_Desc();

public:
	static CUI_Anything* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;
};

