#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render(const wstring& strFontTag, const wstring& strText, const _float2 & vPosition, _fvector vColor, _float fScale, _float2 vOrigin = _float2(0.f, 0.f), _float fRotation = 0.f);

private:
	ID3D11Device*								m_pDevice = { nullptr };
	ID3D11DeviceContext*						m_pContext = { nullptr };
	map<const wstring, class CCustomFont*>		m_Fonts;

private:
	class CCustomFont* Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END