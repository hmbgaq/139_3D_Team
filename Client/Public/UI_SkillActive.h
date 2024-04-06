#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_SkillActive final : public CUI
{
public:
	enum TEXTUREKIND { INACTIVE, ACTIVE, TEXTURE_END };

private:
	CUI_SkillActive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_SkillActive(const CUI_SkillActive& rhs);
	virtual ~CUI_SkillActive() = default;

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
	void					Set_MaxCoolTime(_float fMaxCoolTime) { m_fMaxCoolTime = fMaxCoolTime; }
	_float					Get_MaxCoolTime() { return m_fMaxCoolTime; }

	// CurCoolTime은 현재 스킬 쿨타임입니다.
	void					Set_CurCoolTime(_float fCoolTime) { m_fCoolTime = fCoolTime; }
	_float					Get_CurCoolTime() { return m_fCoolTime; }

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

private:
	void					Check_SkillActive(_float fTimeDelta, SKILLSTATE eState);

public:
	json					Save_Desc(json& out_json);
	void					Load_Desc();

private:
	CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
	_float2					m_vCenter = { 0.f, 0.f };
	_float					m_fRadius = 0.f;
	_float					m_fCoolTime = 0.f;
	_float					m_fMaxCoolTime = 10.f;
	_float					m_fChangeScale = 1.0f;
	_float					m_fOriginScaleX = 50.0f;
	_float					m_fOriginScaleY = 50.0f;
	_int					m_iShaderNum = 0;
	_bool					m_bCoolDown = false;
	_bool					m_bMaxCoolDown = false;

public:
	static CUI_SkillActive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

