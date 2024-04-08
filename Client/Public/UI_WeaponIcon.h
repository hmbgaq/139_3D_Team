#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_WeaponIcon final : public CUI
{
public:
	enum UI_Animation { UNLOCK, JUMP, UIANIM_END };
	enum TEXTUREKIND { LOCK, NONACTIVE, ACTIVE, TEXTURE_END };

private:
	CUI_WeaponIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_WeaponIcon(const CUI_WeaponIcon& rhs);
	virtual ~CUI_WeaponIcon() = default;

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

private:
	void					Check_LevelChange(_float fTimeDelta);
	void					Check_Animation(_float fTimeDelta);
	void					Check_IconSize();

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

public:
	virtual HRESULT			Set_ParentTransform(CTransform* pParentTransformCom) override;

private:
	void					Compute_OwnerCamDistance();
	_bool					In_Frustum();

public:
	json					Save_Desc(json& out_json);
	void					Load_Desc();

private:
	CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
	UI_Animation m_eAnimType = UIANIM_END;
	string			m_strChangeAnim = "";
	_bool			m_bWeapon = false;

public:
	static CUI_WeaponIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

