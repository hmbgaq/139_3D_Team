#pragma once

#include "Base.h"
/* 내 컴퓨터에 꽂혀있는 그래픽카드를 사용할 수 있도록 초기호한다. */

/* 1. 추후, 그래픽카드를 통해 어떤 일을 할 수 있도록 디바이스를 대표하는 컴 객체를 생성하낟. */
/* ID3D11Device, ID3D11DeviceContext */

/* 2. 스왑체인 객체를 생성한다.(IDXGISwapChain : 백버퍼를 만든다. ) */
/* 3. 백버퍼 텍스쳐와 뎁스스텐실 텍스쳐를 생성한다. */
/* 4. 만들어낸 두개의 텍스쳐를 장치에 바인딩하여 이후 이 텍스쳐에 그릴 수 있도록 하낟. */
/* 5. 뷰포트에 대한 설정을 한다. */


BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	ID3D11RenderTargetView* Get_BackBufferRTV() const { return m_pBackBufferRTV; }
	ID3D11DepthStencilView* Get_DSV() const { return m_pDepthStencilView;	}

	GRAPHIC_DESC*			Get_GraphicDesc() { return &m_tGraphicDesc; }

public:
	/* 그래픽 디바이스의 초기화. */
	HRESULT Initialize(const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppContext);

	/* 백버퍼를 지운다. */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);

	/* 깊이버퍼 + 스텐실버퍼를 지운다. */
	HRESULT Clear_DepthStencil_View();

	/* 후면 버퍼를 전면버퍼로 교체한다.(백버퍼를 화면에 직접 보여준다.) */
	HRESULT Present();

private:
	// IDirect3DDevice9* == LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext

	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드) 컴객체의 생성과 관련된 역활.  */
	ID3D11Device*				m_pDevice = nullptr;

	/* 기능실행.(바인딩작업, 정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	/* 그린다. Draw */
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	/* 후면버퍼와 전면버퍼를 교체해가면서 화면에 보여주는 역활. */
	IDXGISwapChain*				m_pSwapChain = nullptr;


	/* IDirect3DTexture9 */
	/* ID3D11Texture2D : 텍스쳐를 표현하는 사전객체 타입이다.
	why? Bind 용도에 맞는 실질적으로 사용하기위한 텍스쳐객체를 만들어내기위해.  */


	/* ID3D11ShaderResourceView : 셰이더에 전달될 수 있는 텍스쳐타입. */
	/* ID3D11RenderTargetView : 렌더타겟용으로 사용될 수 있는  텍스쳐타입. */
	/* ID3D11DepthStencilView : 깊이스텐실버퍼로서 사용될 수 있는 타입.  */
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
	ID3D11Texture2D*			m_pDepthStencilTexture = nullptr;

	GRAPHIC_DESC				m_tGraphicDesc = {};


public: 	/* 이하 모두 HBO+ 준비사항 */
	ID3D11ShaderResourceView* Get_DepthSRV() { return m_pDepthStencilSRV; }
	GFSDK_SSAO_Context_D3D11* Get_AOContext() { return m_pAOContext; }

private:
	ID3D11ShaderResourceView* m_pDepthStencilSRV = nullptr;
	GFSDK_SSAO_Context_D3D11* m_pAOContext = nullptr;


private:
	/* 스왑체인에게 필수적으로 필요한 데이터는 백버퍼가 필요하여. 하여 백버퍼를 생성하기위한 정보를 던져준다. */
	/* 스왑체인을 만들었다 == 백버퍼(텍스쳐)가 생성된다. */
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END