#pragma once
#include "Client_Defines.h"
#include "UI_Text.h"
#include "Data_Manager.h"

/* 체력 프레임 */
class CUI_LevelUp_ShieldFrame final : public CUI_Text
{
	enum TEXTUREKIND { SHIELD_FRAME, TEXTURE_END };

private:
	CUI_LevelUp_ShieldFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_LevelUp_ShieldFrame(const CUI_LevelUp_ShieldFrame& rhs);
	virtual ~CUI_LevelUp_ShieldFrame() = default;

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
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

private:
	void					Check_Disappear(_float fTimeDelta);

public:
	json				 Save_Desc(json& out_json);
	void				 Load_Desc();
	HRESULT				 Ready_Text();
	void				 Add_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation);
	HRESULT				 Find_Change(const string& strTextTag);
	_float				 m_fChangeScale = 1.f;

private:
	CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };

public:
	static CUI_LevelUp_ShieldFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject* Pool() override;
	virtual void			Free() override;
};

