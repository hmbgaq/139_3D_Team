#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Initialize()
{
	Add_Font(TEXT("Font_Gulim"), TEXT("../Bin/Resources/Fonts/Gulim.spritefont"));

	return S_OK;
}

HRESULT CFont_Manager::Add_Font(const wstring & strFontTag, const wstring & strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(m_pDevice, m_pContext, strFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	CCustomFont*		pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPosition, vColor, fScale, vOrigin, fRotation);	
}

CCustomFont * CFont_Manager::Find_Font(const wstring & strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

CFont_Manager * CFont_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFont_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}