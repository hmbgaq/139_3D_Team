#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_Player_Skill_Icon final : public CUI
{
	enum TEXTUREKIND { ICON_LOCK, ICON_COOLDOWN, ICON_ACTIVE, TEXTURE_END };

private:
	CUI_Player_Skill_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_Player_Skill_Icon(const CUI_Player_Skill_Icon& rhs);
	virtual ~CUI_Player_Skill_Icon() = default;

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

public:
	void					Set_SkillState(SKILLSTATE eState) { m_eSkillState = eState; }
	void					Set_SkillUnlock(_bool bUnlock) { m_bUnlock = bUnlock; }
	_bool					Get_SkillUnlock() { return m_bUnlock; }

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

public:
	json					Save_Desc(json& out_json);
	void					Load_Desc();

private:
	CTexture*				m_pTextureCom[TEXTURE_END] = { nullptr };
	SKILLSTATE				m_eSkillState = SKILLSTATE::ACTIVE;
	_bool					m_bUnlock = false/*true*/;

public:
	static CUI_Player_Skill_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

