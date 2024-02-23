#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, _float4 vClearColor)
{
	m_vClearColor = vClearColor;
	//m_wstrTag = strTargetTag;
	
#ifdef _DEBUG
	XMStoreFloat4(&m_vFontColor, (XMVectorSet( 1.f , 1.f, 1.f, 1.f ) - XMLoadFloat4(&m_vClearColor)));

	m_vFontColor.z = 1.f;
#endif

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);	
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	D3D11_VIEWPORT		Viewport;
	_uint				iNumViewport = { 1 };

	m_pContext->RSGetViewports(&iNumViewport, &Viewport);

	m_WorldMatrix._41 = fX - Viewport.Width * 0.5f;
	m_WorldMatrix._42 = -fY + Viewport.Height * 0.5f;

	/* 디버그로 렌더타겟을 스크린에 올려서 볼 애들만 네임태그를 저장해서 렌더돌리기 */
	m_wstrTag = strTargetTag;

	XMStoreFloat4(&m_vFontColor, (XMVectorSet(1.f, 1.f, 1.f, 1.f) - XMLoadFloat4(&m_vClearColor)));
	m_vFontColor.w = 1.f;

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader * pShader, CVIBuffer * pVIBuffer)
{
	FAILED_CHECK(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));

	FAILED_CHECK(pShader->Bind_SRV("g_DiffuseTexture", m_pSRV));
	
	pShader->Begin(0);

	pVIBuffer->Bind_VIBuffers();

	return pVIBuffer->Render();
}

void CRenderTarget::Create_TargetTexture()
{
	SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/DataFiles/Data_UI/Target_Texture/UITargetTexture.dds"));
}

#endif

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, _float4 vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderTarget::Free()
{
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
