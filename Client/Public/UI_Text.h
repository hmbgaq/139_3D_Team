#pragma once
#include "Client_Defines.h"
#include "UI.h"
#include "Texture.h"

class CUI_Text : public CUI
{
public: /* 각 UI파츠마다 어떤걸 얼마나 가질지 설정해주자. */
	enum TEXTUREKIND { TEXTBOX, TEXTLINE, TUTORIALTEXT, RWARD_BUTTON_MESSAGE, TEXTURE_END };

	typedef struct tagTextInfo
	{
		_float	fScale;
		_float	fRotation;
		_float	fPosX;
		_float	fPosY;
		_float2 vOrigin;

		string	strTextKey;
		string	strFontTag;
		string	strText;

		_vector vColor;
	}TEXTINFO;

protected:
	CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_Text(const CUI_Text& rhs);
	virtual ~CUI_Text() = default;

public:
	virtual HRESULT				Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT				Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void				Priority_Tick(_float fTimeDelta) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

protected:
	virtual HRESULT				Ready_Components() override;
	virtual HRESULT				Bind_ShaderResources() override;
	virtual void				RenderTextWithLineBreak(const string& text, size_t MaxLineLength);

public:
	virtual void				Add_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation);
	virtual TEXTINFO*			Find_Text(const string& strTextTag);
	virtual HRESULT				Find_Change(const string& strTextTag);
	virtual void				Change_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation);
	virtual void				Change_Text(TEXTINFO tTextInfo);

public:
	void						Set_FreeMove(_bool bFreeMove) { m_bFreeMove = bFreeMove; }
	TEXTINFO					Get_TextInfo() { return *m_pTextInfo; }

protected:
	map<string, TEXTINFO*>		m_mapText;
	CTexture*					m_pTextureCom[TEXTURE_END] = { nullptr };

	TEXTINFO*					m_pTextInfo = nullptr;
	TEXTUREKIND					m_eTexture_Kind = TEXTURE_END;

	wstring						m_strFontTag = TEXT("");
	wstring						m_strText = TEXT("");
	string						strTextKey = "";

	_float						m_fPosX = (g_iWinsizeX / 2.f);
	_float						m_fPosY = (g_iWinsizeY / 2.f);
	_float						m_fScale = 1.f;
	_float						m_fRotation = 0;
	_float2						m_vOrigin = { 0.f, 0.f };
	//fScale, vOrigin, fRotation;

	_vector						m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_bool						m_bFreeMove = false;
public:
	virtual json				Save_Desc(json& out_json);
	void						Load_Desc();

public:
	virtual void			Free() override;
};

