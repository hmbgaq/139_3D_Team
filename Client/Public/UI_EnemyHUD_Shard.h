#pragma once
#include "Client_Defines.h"
#include "UI.h"

/* 체력 프레임 */
class CUI_EnemyHUD_Shard final : public CUI
{
	enum TEXTUREKIND { NONE, ATTACK, ELECTRIC, SKULL, SKULLRED, TEXTURE_END };

private:
	CUI_EnemyHUD_Shard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_EnemyHUD_Shard(const CUI_EnemyHUD_Shard& rhs);
	virtual ~CUI_EnemyHUD_Shard() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Set_TargetPosition(_vector vTargetPosition);
	void					Check_TargetWorld();
	void					Ready_ChildHUD();

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;
	CGameObject*			m_pTarget = nullptr;
	_vector					m_vTargetPosition = { 0.f, 0.f, 0.f, 0.f };
	_matrix					m_World = XMMatrixIdentity();
	_float					m_fChangeScale = 1.f;
	_bool					m_bAppear = false;
	TEXTUREKIND				m_eCurState = ATTACK;

public:
	json					Save_Desc(json& out_json);
	void					Load_Desc();

private:
	CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };

public:
	static CUI_EnemyHUD_Shard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

