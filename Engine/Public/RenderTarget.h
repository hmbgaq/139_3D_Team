#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }

public:
	HRESULT Initialize(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, _float4 vClearColor);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	HRESULT Clear();
	void	Create_TargetTexture();

public:
	ID3D11Texture2D* Get_Texture2D() { return m_pTexture2D; }

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer* pVIBuffer);
	_float Get_SizeX() { return m_WorldMatrix._11; }
	_float Get_SizeY() { return m_WorldMatrix._22; }
	_float Get_PosX() { return m_WorldMatrix._41; }
	_float Get_PosY() { return m_WorldMatrix._42; }
	_float4 Get_FontColor() { return m_vFontColor; }
#endif
	wstring Get_TargetTag() { return m_wstrTag; }

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };

	_float4						m_vClearColor;

#ifdef _DEBUG
private:
	_float4x4				m_WorldMatrix;
	_float4					m_vFontColor;
#endif
	wstring					m_wstrTag;

	/* For. Portfolid */
public:
	void Set_RenderDebug_Option(_bool bOption) { m_bRenderDebug = bOption; }
	_bool Get_RenderDebug_Option() { return m_bRenderDebug; }

private:
	_bool					m_bRenderDebug = { false };

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, _float4 vClearColor);
	virtual void Free() override;
};

END