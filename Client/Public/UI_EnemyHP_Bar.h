#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(ENGINE)
class CCharacter;
END

/* 체력 프레임 */
class CUI_EnemyHP_Bar final : public CUI
{
public:
	enum TEXTUREKIND { HPBAR_WHITE, HPBAR_RED, HP_DECAL, TEXTURE_END };

private:
	CUI_EnemyHP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_EnemyHP_Bar(const CUI_EnemyHP_Bar& rhs);
	virtual ~CUI_EnemyHP_Bar() = default;

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
	HRESULT					Setting_Owner();
	_bool					In_Frustum();

public:
	json					Save_Desc(json& out_json);
	void					Load_Desc();

private:
	CTexture*				m_pTextureCom[TEXTURE_END] = { nullptr };
	CCharacter*				m_pCharacterOwner = { nullptr };
	_float					m_fPreHP = 100.f;
	_float					m_fCurHP = 100.f;
	_float					m_fMaxHP = 100.f;
	_float					m_fTimeAcc = 0.f;
	_float					m_fVariationSpeed = 2.0f;
	_bool					m_bLerp = false;

public:
	static CUI_EnemyHP_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

