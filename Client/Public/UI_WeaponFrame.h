#pragma once
#include "Client_Defines.h"
#include "UI.h"

class CUI_WeaponFrame final : public CUI
{
public:
	enum TEXTUREKIND { LOCK, NONACTIVE, ACTIVE, TEXTURE_END };

private:
	CUI_WeaponFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_WeaponFrame(const CUI_WeaponFrame& rhs);
	virtual ~CUI_WeaponFrame() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	/* State */
	virtual void			UI_Ready(_float fTimeDelta);
	virtual void			UI_Enter(_float fTimeDelta);
	virtual void			UI_Loop(_float fTimeDelta);
	virtual void			UI_Exit(_float fTimeDelta);

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

public:
	virtual HRESULT			Set_ParentTransform(CTransform* pParentTransformCom) override;

public:
	void					Check_Picking(_float fTimeDelta);
	void					Check_State();
	void					Check_LevelChange(_float fTimeDelta);

private:
	void					Compute_OwnerCamDistance();
	_bool					In_Frustum();

public:
	json					Save_Desc(json& out_json);
	void					Load_Desc();

private:
	CTexture*				m_pTextureCom[TEXTURE_END] = { nullptr };
	_bool					m_bFirstFrame = false;
	UI_LEVEL				m_eUI_PreLevel = LEVEL0;

public:
	static CUI_WeaponFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

