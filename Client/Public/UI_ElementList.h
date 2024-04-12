#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_ElementList final : public CUI
{
	enum TEXTUREKIND { NONACTIVE_HABO, NONACTIVE_FOG, NONACTIVE_RADIAL_BLUR, NONACTIVE_DOF, NONACTIVE_HDR, NONACTIVE_SHADOW, NONACTIVE_SSR, NONACTIVE_HSV, NONACTIVE_PBR,
					   ACTIVE_HABO, ACTIVE_FOG, ACTIVE_RADIAL_BLUR, ACTIVE_DOF, ACTIVE_HDR, ACTIVE_SHADOW, ACTIVE_SSR, ACTIVE_HSV, ACTIVE_PBR,
					   SELECT, TEXTURE_END };

private:
	CUI_ElementList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_ElementList(const CUI_ElementList& rhs);
	virtual ~CUI_ElementList() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	/* State */
	virtual void	UI_Ready(_float fTimeDelta);
	virtual void	UI_Enter(_float fTimeDelta);
	virtual void	UI_Loop(_float fTimeDelta);
	virtual void	UI_Exit(_float fTimeDelta);

public:
	void			Check_Picking(_float fTimeDelta);

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

public:
	json				 Save_Desc(json& out_json);
	void				 Load_Desc();

private:
	CTexture*	m_pTextureCom[TEXTURE_END] = { nullptr };
	_bool		m_bHABO_Active = false;
	_bool		m_bFOG_Active = false;
	_bool		m_bRadial_Blur_Active = false;
	_bool		m_bDof_Active = false;
	_bool		m_bHDR_Active = false;
	_bool		m_bShadow_Active = false;
	_bool		m_bHSV_Active = false;
	_bool		m_bPBR_Active = false;

public:
	static CUI_ElementList* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

