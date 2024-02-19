#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_SRVs(rhs.m_SRVs)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

_bool CTexture::Write_Json(json& Out_Json)
{
	return false;
}

void CTexture::Load_FromJson(const json& In_Json)
{
	_int iTest = 0;
}

HRESULT CTexture::Initialize_Prototype(const wstring & strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	m_SRVs.reserve(m_iNumTextures);

	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		/* ..\정의훈\139\Framework\Client\Bin\Resources\Textures\Default.jpg */
		_tchar		szFullPath[MAX_PATH] = TEXT("");

		wsprintf(szFullPath, strTextureFilePath.c_str(), i);

		/* D:\ */
		_tchar		szDrive[MAX_PATH] = TEXT("");

		/* 정의훈\139\Framework\Client\Bin\Resources\Textures\ */
		_tchar		szDirectory[MAX_PATH] = TEXT("");

		/* Default_0 */
		_tchar		szFileName[MAX_PATH] = TEXT("");

		/* .jpg */
		_tchar		szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		ID3D11ShaderResourceView*			pSRV = { nullptr };

		HRESULT			hr = 0;

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		else if (!lstrcmp(szExt, TEXT(".tga")))
		{

		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShader, const _char * pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);
}

HRESULT CTexture::Get_TextureSize(_uint* iWidth, _uint* iHeight, _uint iTextureIndex)
{

	// 인덱스 범위 확인
	if (iTextureIndex >= m_SRVs.size())
		return E_FAIL; // 유효하지 않은 인덱스 처리

	 // 특정 인덱스의 ID3D11ShaderResourceView를 가져옴
	ID3D11ShaderResourceView* pSRV = m_SRVs[iTextureIndex];

	// ID3D11Resource를 가져오기 위해 QueryInterface 호출
	ID3D11Resource* pResource = nullptr;
	pSRV->GetResource(reinterpret_cast<ID3D11Resource**>(&pResource));

	// ID3D11Texture2D로 캐스팅하여 크기 얻기
	ID3D11Texture2D* pTexture2D = static_cast<ID3D11Texture2D*>(pResource);

	D3D11_TEXTURE2D_DESC desc;
	pTexture2D->GetDesc(&desc);

	// 너비와 높이 가져오기
	*iWidth = desc.Width;
	*iHeight = desc.Height;


	// 리소스 해제
	Safe_Release(pResource);

	return S_OK;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}
