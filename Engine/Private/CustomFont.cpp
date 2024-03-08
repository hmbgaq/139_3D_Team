#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const wstring & strFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);

	m_pFont = new SpriteFont(m_pDevice, strFontFilePath.c_str());
	
	return S_OK;
}

HRESULT CCustomFont::Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, fRotation, vOrigin, fScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
