#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

private:
	map<const wstring, class CRenderTarget*>			m_RenderTargets;
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;

private:
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END