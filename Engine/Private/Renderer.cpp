#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_SSAO.h"
#include "AlphaObject.h"
#include "Shader.h"
#include "SMath.h"
#include "RenderTarget.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	/* 버퍼 객체 생성 */
	FAILED_CHECK(Create_Buffer());

	/* 셰이더 객체 생성 */
	FAILED_CHECK(Create_Shader());

	/* 렌더타겟 생성 + MRT 그룹 지정 */
	FAILED_CHECK(Create_RenderTarget());

	/* ID3D11DepthStencilView 생성 */
	FAILED_CHECK(Create_DepthStencil());

#ifdef _DEBUG
	/* 앞서 만든 MRT를 디버그렌더에 그리는용도 */
	FAILED_CHECK(Ready_DebugRender()); 
#endif

	m_tHBAO_Option.bHBAO_Active = m_bSSAO_Active;
	m_tFog_Option.bFog_Active = m_bFog_Active;
	m_tHDR_Option.bHDR_Active = m_bHDR_Active;
	m_tScreen_Option.bFXAA_Active = m_bFXAA_Active;

	return S_OK;
}

#pragma region Renderer_Initialize

HRESULT CRenderer::Create_Buffer()
{
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	m_pPerlinNoiseTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/T_Perlin_Noise_M.dds"));
	NULL_CHECK_RETURN(m_pPerlinNoiseTextureCom, E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Create_Shader()
{
	m_pShader[SHADER_TYPE::SHADER_DEFERRED] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_DEFERRED], E_FAIL);

	m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING], E_FAIL);

	m_pShader[SHADER_TYPE::SHADER_BLUR] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Blur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_BLUR], E_FAIL);

	m_pShader[SHADER_TYPE::SHADER_OUTLINE] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Outline.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_OUTLINE], E_FAIL);

	m_pShader[SHADER_TYPE::SHADER_FXAA] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Fxaa3_11.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_FXAA], E_FAIL);

	m_pShader[SHADER_TYPE::SHADER_FINAL] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Final.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_FINAL], E_FAIL);


	/* Shader_UI */
	{ // error : 같은 셰이더 배열에 넣어서
	  //m_pShader[SHADER_TYPE::SHADER_DEFERRED_UI] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	  //NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_DEFERRED], E_FAIL);
	}

	return S_OK;
}

HRESULT CRenderer::Create_RenderTarget()
{
	D3D11_VIEWPORT		Viewport;
	_uint				iNumViewport = { 1 };

	m_pContext->RSGetViewports(&iNumViewport, &Viewport);

	/* RenderTarget */
	{
		/* MRT_Priority - 스카이박스 등 우선그리는용도 */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Priority"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

		/* MRT_GameObject - Target_ViewNormal 없애도 될듯 - SSAO 용도 */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f))); /* 깊이버퍼 그 깊이 */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_ORM"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_ViewNormal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); /* 넣은거 알아서 블러되도록 처리함 */
		{
			/* MRT_Bloom_Blur*/
			FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
		}
		/* MRT_OutLine*/
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_OutLine"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		
		/* MRT_Shadow */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

		/* MRT_Cascade_Shadow */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade_Shadow"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 0.0f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade1"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade2"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade3"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f)));

		/* MRT_LightAcc */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));

		/* MRT_SSAO	*/
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_DownSampling"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Horizontal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Vertical"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_UpSampling"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

		/* MRT_HBAO+ */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HBAO"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f)));

		
		/* MRT_PrePostProcess */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_PrePostProcess"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	}
	/* PostProcessing */
	{
		/* MRT_HDR */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HDR"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

		/* MRT_RadialBlur */
		//FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		/* MRT_GodRay */
		//FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_GodRay"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

		/* MRT_FXAA */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_FXAA"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

	}
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	

	/* UI_Target */
	{
		/* 타겟 순서 셰이더 out이랑 맞춰야한다. */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse_UI"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f)));		// 색상
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal_UI"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f)))	// 노말
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth_UI"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));	// 깊이
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade_UI"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));	// 셰이드
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular_UI"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));// 스펙큘러
	}

	/* MRT*/
	{
		/* MRT_Priority */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Priority"), TEXT("Target_Priority")));

		/* MRT_GameObject */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ORM")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ViewNormal")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Bloom"))); /* 오브젝트 외곽선 */
		{
			/* MRT_Bloom_Blur*/
			FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Blur"), TEXT("Target_Bloom_Blur")));
		}

		/* MRT_OutLine */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_OutLine"), TEXT("Target_OutLine")));

		/* MRT_LightAcc */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")));

		/* MRT_Shadow */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth")));

		/* MRT_Cascade_Shadow */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade_Shadow"), TEXT("Target_Cascade_Shadow")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade1"), TEXT("Target_Cascade1")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade2"), TEXT("Target_Cascade2")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade3"), TEXT("Target_Cascade3")));

		/* Blur -  여러군데서 사용하는 블러를 하나로 병합하기위해 Target_Blur_DownSampling에 g_값 으로 넘겨서 하나로 사용하기위해 나눠둠*/
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_DownSampling"), TEXT("Target_Blur_DownSampling")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Horizontal"), TEXT("Target_Blur_Horizontal")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Vertical"), TEXT("Target_Blur_Vertical")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_UpSampling"), TEXT("Target_Blur_UpSampling")));


		/* MRT_GodRay */
		//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GodRay"), TEXT("Target_GodRay")));

		/* MRT_RadialBlur*/
		//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_RaidalBlur"), TEXT("Target_RadialBlur")));

		/* MRT_PrePostProcessScene */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_PrePostProcessScene"), TEXT("Target_PrePostProcess")));

		/* MRT_FXAA*/
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_FXAA"), TEXT("Target_FXAA")));

		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR")));
#pragma region ADD_MRT_UI
		Add_MRT_UI();
#pragma endregion End
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));
	//XMStoreFloat4x4(&m_MinimapProj, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1600.f / 900.f, 0.2f, 1000.0f));

	return S_OK;
}

HRESULT CRenderer::Create_DepthStencil()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture));

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	FAILED_CHECK(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthDSV));

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Ready_DebugRender()
{
	_float fSizeX = 150.f;
	_float fSizeY = 100.f;

	/* MRT_GameObject - 좌상단 Default */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ORM"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Bloom"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Bloom_Blur"),	(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RimLight"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 13.f), fSizeX, fSizeY));

	///* MRT_LightAcc */
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse_UI"),	(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade"),		(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"),		(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));

	{
		/* !성희 : 우상단 - UI Debug */
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse_UI"), (g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse_UI"), (fSizeX / 2.f * 24.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal_UI"), (fSizeX / 2.f * 24.5f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth_UI"), (fSizeX / 2.f * 24.5f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade_UI"), (fSizeX / 2.f * 24.5f), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular_UI"), (fSizeX / 2.f * 24.5f), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
	}

	return S_OK;
}

HRESULT CRenderer::Ready_CascadeShadow()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	//ShadowDepth
	for (_uint i = 0; i < 3; ++i)
	{
		ID3D11Texture2D* pDepthStencilTexture = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = g_iWinsizeX;
		TextureDesc.Height = g_iWinsizeY;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture));

		FAILED_CHECK(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pCascadeShadowDSV[i]));
	}

	return S_OK;
}

HRESULT CRenderer::Control_HotKey()
{
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		cout << " ----------------------------- " << endl;
		cout << " DIK_1 : NONE ON/OFF " << endl;
		cout << " DIK_2 : HBAO+ ON/OFF " << endl;
		cout << " DIK_3 : BlurBloom ON/OFF " << endl;
		cout << " DIK_4 : HDR ON/OFF " << endl;
		cout << " DIK_5 : FXAA ON/OFF " << endl;
		cout << " DIK_6 : RimLight ON/OFF " << endl;
		cout << " DIK_7 : Fog ON/OFF " << endl;
		cout << " DIK_8 : Bloom Color Increase " << endl;
		cout << " DIK_9 : Dont use !! Renderpass BloomBlur Test " << endl;

		if (true == m_tHBAO_Option.bHBAO_Active)
			cout << "HBAO+ : true " << endl;
		else
			cout << "HBAO+ : false " << endl;

		if (true == m_tHDR_Option.bHDR_Active)
			cout << "HDR : true " << endl;
		else
			cout << "HDR : false " << endl;

		if (true == m_tScreen_Option.bFXAA_Active)
			cout << "FXAA : true " << endl;
		else
			cout << "FXAA : false " << endl;

		if (true == m_bBloom_Active)
			cout << "Bloom Blur : true " << endl;
		else
			cout << "Bloom Blur : false " << endl;

		cout << " ----------------------------- " << endl;
	}
	if (m_pGameInstance->Key_Down(DIK_1))
		m_bOutline_Active = !m_bOutline_Active;
	if (m_pGameInstance->Key_Down(DIK_2))
		m_tHBAO_Option.bHBAO_Active = !m_tHBAO_Option.bHBAO_Active;
	if (m_pGameInstance->Key_Down(DIK_3))
		m_bBloom_Active = !m_bBloom_Active;
	if (m_pGameInstance->Key_Down(DIK_4))
		m_tHDR_Option.bHDR_Active = !m_tHDR_Option.bHDR_Active;
	if (m_pGameInstance->Key_Down(DIK_5))
		m_tScreen_Option.bFXAA_Active = !m_tScreen_Option.bFXAA_Active;
	if (m_pGameInstance->Key_Down(DIK_7))
		m_tFog_Option.bFog_Active = !m_tFog_Option.bFog_Active;

	return S_OK;
}
#endif // _DEBUG

#pragma endregion

HRESULT CRenderer::Draw_RenderGroup()
{
#ifdef _DEBUG
	Control_HotKey();
#endif // _DEBUG

	FAILED_CHECK(Render_Priority());			/* SkyBox */

	/* Pre-PostProcessing */
	FAILED_CHECK(Render_NonBlend());			/* MRT_GameObjects*/
	
	//FAILED_CHECK(Render_Cascade_Shadow());		/* MRT_Cascade */

	if (true == m_tHBAO_Option.bHBAO_Active)
	{
		FAILED_CHECK(Render_HBAO_PLUS());
	}

	FAILED_CHECK(Render_Decal());	/* MRT_LightAcc */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     

	/* 외곽선 */
	FAILED_CHECK(Render_OutLine()); 
	
	FAILED_CHECK(Render_Effect());		/* - 외곽선 필요유무에 따라 위치가 달라질듯? */

	FAILED_CHECK(Render_Shadow());		/* MRT_Shadow */

	FAILED_CHECK(Render_LightAcc());	/* MRT_LightAcc */

	FAILED_CHECK(Render_Bloom()); /* 림라이트 밀릴수있음 주의 */

	/* ★ Deferred ★*/
	FAILED_CHECK(Render_Deferred());

	FAILED_CHECK(Render_SSR());

	//FAILED_CHECK(Render_Blend());

	FAILED_CHECK(Render_NonLight());

	/* ★ PostProcessing ★*/
	FAILED_CHECK(Render_PostProcess()); /* 모션블러, Radial 블러 등등 */

	/* 그리기 */
	/* 마지막화면용 - 마지막 체크 위해서 */

	FAILED_CHECK(Render_Final());

	FAILED_CHECK(Render_UI()); /* 디버그에서 렌더타겟 한장으로 그린거 콜하는 그룹 여기 */

	//FAILED_CHECK(Render_UI_EFFECT()); /* 그외 변하거나 이미지에 효과가 들어가야하는 UI 렌더그룹이 속하는곳 */

	FAILED_CHECK(Render_Blend()); // 현재 effect, particle 그려지는 RenderPass -> MRT 편입전까지 여기서 상위에서 홀로 그려지도록 해놓음 
	/* ------------------------ */

	/* 효과넣어주기 */
	//FAILED_CHECK(Render_Lights_UI());	// 빛
	//FAILED_CHECK(Render_Deferred_UI());
	
	/* 최종 완성본 그리기 */
	//FAILED_CHECK(Render_UI_Final());

#ifdef _DEBUG
	if(true == m_bRenderDebug)
		FAILED_CHECK(Render_Debug());
#endif

	return S_OK;
}

#pragma region RenderCall

#pragma region RenderGroup

HRESULT CRenderer::Render_Priority()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Priority")));

	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pLightDepthDSV));

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOW])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render_Shadow();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOW].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = g_iWinsizeX;
	ViewPortDesc.Height = g_iWinsizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	/* 기존에 셋팅되어있던 백버퍼를 빼내고 Diffuse와 Normal을 장치에 바인딩한다. */
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects")));

	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	/* 백버퍼를 원래 위치로 다시 장치에 바인딩한다. */
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return ((CAlphaObject*)pSour)->Get_CamDistance() > ((CAlphaObject*)pDest)->Get_CamDistance();
		});

	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Add_MRT_UI()
{
	// MRT_GameObjects_UI
	{
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects_UI"), TEXT("Target_Diffuse_UI")))
			return E_FAIL;
		//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects_UI"), TEXT("Target_Normal_UI")))
		//	return E_FAIL;
		//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects_UI"), TEXT("Target_Depth_UI")))
			return E_FAIL;
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Lights_UI"), TEXT("Target_Shade_UI")))
			return E_FAIL;
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Lights_UI"), TEXT("Target_Specular_UI")))
			return E_FAIL;

		//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects_UI"), TEXT("Target_Bloom_UI")))
		//	return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	//if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects_UI"))))
	//	return E_FAIL;
	
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	//if (FAILED(m_pGameInstance->End_MRT()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Text()
{
	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_UI()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects_UI"))))
		return E_FAIL;

	for (auto& iter : m_RenderObjects[RENDER_NONBLEND_UI])
	{
		if (nullptr == iter)
			continue;

		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_NONBLEND_UI].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights_UI()
{
	/* Shade */
	/* 여러개 빛의 연산 결과를 저장해 준다. */
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights_UI")));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	_float		CamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_NormalTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DepthTexture"));

	m_pGameInstance->Render_Lights(m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);

	/* 0번째에 백버퍼렌더타겟이 올라갔다. */
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_OutLine_UI()
{
	return S_OK;
}

HRESULT CRenderer::Render_Deferred_UI()
{
	///* 여러개 빛의 연산 결과를 저장해 준다. */
	//FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights_UI")));

	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	//_float		CamFar = m_pGameInstance->Get_CamFar();
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float)));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));

	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_NormalTexture"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DepthTexture"));

	//m_pGameInstance->Render_Lights(m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);

	///* 0번째에 백버퍼렌더타겟이 올라갔다. */
	//FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_UI_Minimap()
{
	return S_OK;
}

HRESULT CRenderer::Render_UI_Minimap_Icon()
{
	return S_OK;
}

HRESULT CRenderer::Render_UIEffectNonBlend()
{
	return S_OK;
}

HRESULT CRenderer::Render_UIEffectBlend()
{
	return S_OK;
}

HRESULT CRenderer::Render_Screen_Effect()
{
	return S_OK;
}

HRESULT CRenderer::Render_UI_Final()
{
	/* 디퍼드에 의한 최종장면 (UI) */
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	_float			CamFar = m_pGameInstance->Get_CamFar();
	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinsizeX / g_iWinsizeY, 0.1f, CamFar));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float))); // 카메라

	/* MRT_GameObject */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DiffuseTexture"));	// 색상
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DepthTexture"));		// 깊이

	/* Post Processing */
	{
		// 넣어줄 효과의 값 바인드1
		// FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED_UI]->Bind_RawValue("g_bSSAO_Active", &m_bSSAO_Active, sizeof(_bool)));
		if (true == m_bSSAO_Active)
		{
			/* 효과1 */
		}

		// 넣어줄 효과의 값 바인드2
		//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_bBloom_Active", &m_bBloom_Active, sizeof(_bool)));
		if (true == m_bBloom_Active)
		{
			//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Bloom_Blur"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_BloomTarget"));
		}

		// 넣어줄 효과의 값 바인드3
		// FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED_UI]->Bind_RawValue("g_Outline_Active", &m_bOutline_Active, sizeof(_bool)));
		if (true == m_bOutline_Active)
		{
			/* 효과3 */
		}
	}

	m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Begin(ECast(DEFERRED_SHADER::DEFERRED_UI));

	m_pVIBuffer->Bind_VIBuffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Cursor()
{
	return S_OK;
}

#pragma endregion

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade */
	/* 여러개 빛의 연산 결과를 저장해 준다. */
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc")));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	_float		CamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_NormalTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DepthTexture"));

	m_pGameInstance->Render_Lights(m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);

	/* 0번째에 백버퍼렌더타겟이 올라갔다. */
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_PrePostProcessScene")));

	/* 디퍼드에 의한 최종장면 */
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	
	_float			CamFar = m_pGameInstance->Get_CamFar();
	_float4x4		ViewMatrix, ProjMatrix;
	_float4			CamPos = m_pGameInstance->Get_CamPosition();

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinsizeX / g_iWinsizeY, 0.1f, CamFar));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_LightViewMatrix", &ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_LightProjMatrix", &ProjMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float))); 
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4)));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_bFog_Active", &m_tFog_Option.bFog_Active, sizeof(_bool)));

	if(true == m_tFog_Option.bFog_Active)
	{
		/* test fog */
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Struct("g_Fogdesc", &m_tFog_Option, sizeof(FOG_DESC)));
	}

	/* MRT_GameObject */
	FAILED_CHECK(m_pPerlinNoiseTextureCom->Bind_ShaderResource(m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_PerlinNoiseTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_PriorityTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DiffuseTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DepthTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shade"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_ShadeTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_SpecularTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_LightDepth"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_LightDepthTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Bloom_Blur"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_BloomTarget"));

	/* Post Processing */
	{
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_bSSAO_Active", &m_tHBAO_Option.bHBAO_Active, sizeof(_bool)));
		if (true == m_tHBAO_Option.bHBAO_Active)
		{
			FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_SSAOTexture")); /* ssao 추가 */
		}

		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_bBloom_Active", &m_bBloom_Active, sizeof(_bool)));

		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_Outline_Active", &m_bOutline_Active, sizeof(_bool)));
		if (true == m_bOutline_Active)
		{
			m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_OutLine"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_OutlineTarget");
		}
	}

	m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Begin(ECast(DEFERRED_SHADER::DEFERRED));

	m_pVIBuffer->Bind_VIBuffers();

	m_pVIBuffer->Render();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Cascade_Shadow()
{
	// Viewport -> RSSet -> OM DepthStencilSet -> Clear -> Matrix Tick -> Shader bind 

	for (_uint i = 0; i < MAX_CASCADES; ++i)
	{
		wstring szTargetName = L"MRT_Cascade";
		szTargetName += to_wstring(i + 1); /* MRT_Cascade1, 2, 3 들어가도록 */

		FAILED_CHECK(m_pGameInstance->Begin_MRT(szTargetName)); /* Target_OutLine저장 */

		m_pContext->ClearDepthStencilView(m_pCascadeShadowDSV[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		for (auto& iter : m_CascadeObjects[i])
		{
			if (FAILED(iter->Render_Cascade_Shadow(i)))
				return E_FAIL;
			Safe_Release(iter);
		}
		m_CascadeObjects[i].clear();

		FAILED_CHECK(m_pGameInstance->End_MRT());
	}
}

HRESULT CRenderer::Render_Decal()
{
	return S_OK;
}

HRESULT CRenderer::Render_OutLine()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_OutLine"))); /* Target_OutLine저장 */

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	{
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader[SHADER_TYPE::SHADER_OUTLINE], "g_NormalTarget"));

		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4)));
	}

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Begin(0));

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_HBAO_PLUS()
{
	GFSDK_SSAO_InputData_D3D11 Input;
	Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	Input.DepthData.pFullResDepthTextureSRV = m_pGameInstance->Get_DepthSRV();

	_matrix ProjMatrix = {};

	if (true == m_bInit)
	{
		_float fNear = 0.1f;
		_float fFar = 2000.f;
		_float fFovY = DirectX::XMConvertToRadians(60.0f);
		_float fAspect = g_iWinsizeX / g_iWinsizeY;

		ProjMatrix = DirectX::XMMatrixPerspectiveFovLH(fFovY, fAspect, fNear, fFar); /*DirectX 네임스페이스 없으면 함수 못찾음 */
	}
	else
		ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)&ProjMatrix);
	Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	Input.DepthData.MetersToViewSpaceUnits = 1.f;

	GFSDK_SSAO_Parameters Params;
	Params.Radius = m_tHBAO_Option.fRadius;
	Params.Bias = m_tHBAO_Option.fBias;
	Params.PowerExponent = m_tHBAO_Option.fPowerExponent;
	Params.Blur.Enable = true;
	Params.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;
	Params.Blur.Sharpness = m_tHBAO_Option.fBlur_Sharpness;

	ID3D11RenderTargetView* pView = {};
	pView = m_pGameInstance->Find_RenderTarget(TEXT("Target_HBAO"))->Get_RTV();
	NULL_CHECK_RETURN(pView, E_FAIL);

	GFSDK_SSAO_Output_D3D11 Output;
	Output.pRenderTargetView = pView;

	Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

	GFSDK_SSAO_Status status;
	status = m_pGameInstance->Get_AOContext()->RenderAO(m_pContext, Input, Params, Output);
	assert(status == GFSDK_SSAO_OK);

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	FAILED_CHECK(Render_Blur(L"Target_Bloom", L"MRT_Bloom_Blur",
							 ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER), 
							 ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER), 
							 ECast(BLUR_SHADER::BLUR_UP_ADD), true));
	//FAILED_CHECK(Render_Blur_DownSample(L"Target_Bloom"));
	//
	//FAILED_CHECK(Render_Blur_Horizontal(ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER)));
	//
	//FAILED_CHECK(Render_Blur_Vertical(ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER)));
	//
	//FAILED_CHECK(Render_Blur_UpSample(L"MRT_Bloom_Blur", true, ECast(BLUR_SHADER::BLUR_UP_MAX)));

	return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
	FAILED_CHECK(Render_HDR()); /* HDR - 톤맵핑 */

	FAILED_CHECK(Render_FXAA()); /* 안티앨리어싱 - 최종장면 */

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	//FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_RaidalBlur"))); /* Target SSAO 단독 */

	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	///* 변수 올리기 */
	//{
	//	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_fQuality", &m_fRadialBlurQuality, sizeof(_float4)));
	//	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_RawValue("g_fRadialPower", &m_fRadialBlurPower, sizeof(_float4)));
	//}

	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blend"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_BlendTarget"));

	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Begin(ECast(DEFERRED_SHADER::SHADER_END)));

	//FAILED_CHECK(m_pVIBuffer->Render());

	//FAILED_CHECK(m_pGameInstance->End_MRT());

	////FAILED_CHECK(Render_AlphaBlendTargetMix(L"Target_RadialBlur", L"MRT_Blend", true))) 
	////-> mix 시키고 deferred셰이더에 값 다른곳에 던지고 render -> clear함 

	return S_OK;
}

HRESULT CRenderer::Render_Blur(const wstring& strStartTargetTag, const wstring& strFinalTragetTag, _int eHorizontalPass, _int eVerticalPass, _int eBlendType, _bool bClear)
{
	FAILED_CHECK(Render_Blur_DownSample(strStartTargetTag));

	FAILED_CHECK(Render_Blur_Horizontal(eHorizontalPass));

	FAILED_CHECK(Render_Blur_Vertical(eVerticalPass));

	FAILED_CHECK(Render_Blur_UpSample(strFinalTragetTag, bClear, eBlendType));

	return S_OK;
}

HRESULT CRenderer::Render_GodRay()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_GodRay")));

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_FXAA()
{
	/* MSAA와 비교하여 FXAA의 목표는 더 빠르고 메모리 점유율이 더 낮으며, 픽셀이 흐릿해지는 현상이 일어나지 않는다.
	* 화면의 색깔변화를 통해 경계를 찾는 알고리즘을 이용 (엣지 디텍션 -> 픽셀수정 -> 리샘플링 -> 블러처리)후처리이기 때문에 
	* 화면 전체에 AA 적용됨 광원효과, 텍스쳐, 후처리 효과역시 AA 대상 성능 타격이 거의 없음
	* 알고리즘 특성상 흐려지거나 적용이 안되는 한계가 있을수 있음 */

	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_FXAA"))); /* Target_FXAA*/

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FXAA]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FXAA]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FXAA]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	/* 변수 올리기 */
	{
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FXAA]->Bind_RawValue("g_bFxaa", &m_tScreen_Option.bFXAA_Active, sizeof(_bool)));

		/* deferred 이후에 post process가 생긴다면 그걸로 타겟을 바꿔야함 일단 지금은 deferred가 그린 그림위에 만드는것 */
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HDR"), m_pShader[SHADER_TYPE::SHADER_FXAA], "g_FinalTarget"));
	}

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FXAA]->Begin(0));

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	if (m_pGameInstance->Key_Down(DIK_O))
	{
		m_tHDR_Option.fmax_white -= 0.1f;
	}
	else if (m_pGameInstance->Key_Down(DIK_P))
		m_tHDR_Option.fmax_white += 0.1f;

	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_HDR"))); /* Target_FXAA*/

	/* 변수 올리기 */
	{
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	
		/* deferred 이후에 post process가 생긴다면 그걸로 타겟을 바꿔야함 일단 지금은 deferred가 그린 그림위에 만드는것 */
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_RawValue("g_bHDR_Active", &m_tHDR_Option.bHDR_Active, sizeof(_bool)));
		/* 값 컨트롤용도 */
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_RawValue("g_max_white", &m_tHDR_Option.fmax_white, sizeof(_float)));
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_PrePostProcess"), m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING], "g_ProcessingTarget"));
	}

	if (false == m_tHDR_Option.bHDR_Active)
	{
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Begin(ECast(POST_SHADER::POST_ORIGIN)));
	}
	else
	{
		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Begin(ECast(POST_SHADER::POST_HDR)));
	}

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_SSR()
{
	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if (m_pGameInstance->Key_Down(DIK_T))
		m_tScreen_Option.fFinal_Brightness += 0.1f;
	if (m_pGameInstance->Key_Down(DIK_Y))
		m_tScreen_Option.fFinal_Brightness -= 0.1f;
	if (m_pGameInstance->Key_Down(DIK_U))
		m_tScreen_Option.fFinal_Saturation += 0.1f;
	if (m_pGameInstance->Key_Down(DIK_I))
		m_tScreen_Option.fFinal_Saturation -= 0.1f;

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FINAL]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FINAL]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FINAL]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	/* 명도 채도 관리 */
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FINAL]->Bind_RawValue("g_brightness", &m_tScreen_Option.fFinal_Brightness, sizeof(_float)));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FINAL]->Bind_RawValue("g_saturation", &m_tScreen_Option.fFinal_Saturation, sizeof(_float)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_FXAA"), m_pShader[SHADER_TYPE::SHADER_FINAL], "g_FinalTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse_UI"), m_pShader[SHADER_TYPE::SHADER_FINAL], "g_Diffuse_UITexture"));	// MRT_GameObjects_UI

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_FINAL]->Begin(0));

	FAILED_CHECK(m_pVIBuffer->Render());

	return S_OK;
}

HRESULT CRenderer::Render_OutLineGroup()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_OUTLINE])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_OutLine();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_OUTLINE].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	for (auto& pDebugCom : m_DebugComponent)
	{
		pDebugCom->Render();
		//AnimTool 작업중에 애 있으면 삭제시에 터져버림 
		Safe_Release(pDebugCom);
	}
	m_DebugComponent.clear();

	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GameObjects"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Bloom_Blur"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_SSAO"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_SSAO_Blur"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_OutLine"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GodRay"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
		/* UI */
	{
		m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GameObjects_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
		m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Lights_UI"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);

	}
	return S_OK;
}
#endif

#pragma endregion

#pragma region Blur

HRESULT CRenderer::Render_Blur_DownSample(const wstring& strStartTargetTag)
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_DownSampling")));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(strStartTargetTag, m_pShader[SHADER_TYPE::SHADER_BLUR], "g_BlurTarget"));

	_float4x4 WorldMatrix = m_WorldMatrix;
	WorldMatrix._11 = m_WorldMatrix._11 * 0.5f;
	WorldMatrix._22 = m_WorldMatrix._22 * 0.5f;

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_WorldMatrix", &WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Begin(ECast(BLUR_SHADER::BLUR_DOWN)));

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blur_Horizontal(_int eHorizontalPass)
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Horizontal")));

	/* 다운샘플링이 끝난걸 수행하므로 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_DownSampling"), m_pShader[SHADER_TYPE::SHADER_BLUR], "g_BlurTarget"));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Begin(eHorizontalPass));

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blur_Vertical(_int eVerticalPass)
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Vertical")));

	/* 수평샘플링이 끝난걸 수행 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_Horizontal"), m_pShader[SHADER_TYPE::SHADER_BLUR], "g_BlurTarget"));


	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Begin(eVerticalPass));

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blur_UpSample(const wstring& strFinalMrtTag, _bool bClear, _int eBlendType)
{
	/* 원래사이즈로 다시 되돌림 */
	FAILED_CHECK(m_pGameInstance->Begin_MRT(strFinalMrtTag, nullptr, bClear));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_Vertical"), m_pShader[SHADER_TYPE::SHADER_BLUR], "g_BlurTarget"));

	_float4x4 WorldMatrix = m_WorldMatrix;
	WorldMatrix._11 = m_WorldMatrix._11 * 2.f;
	WorldMatrix._22 = m_WorldMatrix._22 * 2.f;

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_WorldMatrix", &WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_BLUR]->Begin(eBlendType));

	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}
void CRenderer::Calc_Blur_GaussianWeights(_int sigma, _int iSize , _Out_ void* Weights)
{
	/* Calc_Blur_GaussianWeights(3, g_fWeight_middle[7]) 이런식으로 들어올예정 */
	/* sigma 는 작을수록 가중치가 빠르게 감소. 클수록 천천히 감소해서 부드러운 블러가 나타난다.  */
	/* */

	/*example : sigma = 1, iSize = 3*/
	_float coeff = 1.0f / (2.0f * sigma * sigma);

	// 가중치를 계산하고 저장
	_float* Weight = static_cast<float*>(Weights);
	_int halfSize = iSize / 2; /* 중앙의 인덱스 */

	_float sum = 0.0f;

	for (_int i = -halfSize; i <= halfSize; ++i)
	{
		_int x = i + halfSize;
		Weight[x] = exp(-static_cast<float>(i * i) * coeff);
	}

	// 가중치 정규화
	for (_int i = 0; i < iSize; ++i)
	{
		sum += Weight[i];
	}

	for (_int i = 0; i < iSize; ++i)
	{
		Weight[i] /= sum;
	}
}

#pragma endregion

#pragma region Add RenderGroup / Debug_Render

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroupID, CGameObject* pGameObject)
{
	if (nullptr == pGameObject ||
		eGroupID >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eGroupID].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugRender(CComponent* pDebugCom)
{
#ifdef _DEBUG
	if (nullptr == pDebugCom)
		return S_OK;

	m_DebugComponent.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);
#endif

	return S_OK;
}

HRESULT CRenderer::Pre_Setting()
{
	return S_OK;
}

#pragma endregion

#pragma region Create / Free

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderer::Free()
{
	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);
		ObjectList.clear();
	}

#ifdef _DEBUG

	for (auto pDebugCom : m_DebugComponent)
		Safe_Release(pDebugCom);
	m_DebugComponent.clear();

#endif

	for (_int i = 0; i < ECast(SHADER_TYPE::SHADER_END); ++i)
		Safe_Release(m_pShader[i]);

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pLightDepthDSV);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}

#pragma endregion

#pragma region ssao_initialize
//
//HRESULT CRenderer::Ready_SSAO()
//{
//	// ssao 객체 생성
//	//	: SSAO 개체를 만들려면 Direct3D 장치, DeviceContext, 화면 크기, 카메라 fov 및 카메라 원거리 평면 거리를 전달해야한다. 
//		
//FAILED_CHECK(SSAO_OnSize());
//
//FAILED_CHECK(BuildFullScreenQuad());/* 왜 이거안하지..? */
//
//BuildOffsetVectors();
//
//BuildRandomVectorTexture();
//
//m_pRandomVectorTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/RandomNormalTexture.jpg"), 1);
//
//NULL_CHECK_RETURN(m_pRandomVectorTexture, E_FAIL);
//
//return S_OK;
//}
//
//HRESULT CRenderer::SSAO_OnSize()
//{
//	/* + BuildFrustumFarCorners */
//	_float4 CamSetting = m_pGameInstance->Get_CamSetting();
//	// { Cam_near, Cam_far, Cam_fovY, Cam_aspectRatio };
//	/* 카메라의 원근투영행렬에서 사용되는값.
//	 * 원근투영의 가장 먼 클리핑 평면을 나타낸다. */
//
//	_float fNear = CamSetting.x;
//	_float fFar = CamSetting.y;
//	_float fFovY = CamSetting.z;
//	_float fAspect = g_iWinsizeX / g_iWinsizeY;
//
//	_float fHalfHeight = fFar * tanf(0.5f * fFovY);
//	_float fHalfWidth = fAspect * fHalfHeight;
//
//	m_vFrustumFarCorner[0] = _float4(-fHalfWidth, -fHalfHeight, fFar, 0.0f);
//	m_vFrustumFarCorner[1] = _float4(-fHalfWidth, +fHalfHeight, fFar, 0.0f);
//	m_vFrustumFarCorner[2] = _float4(+fHalfWidth, +fHalfHeight, fFar, 0.0f);
//	m_vFrustumFarCorner[3] = _float4(+fHalfWidth, -fHalfHeight, fFar, 0.0f);
//
//	return S_OK;
//}
//
//HRESULT CRenderer::BuildFullScreenQuad()
//{
//	m_iQuadVerCount = 4;
//	m_iQuadIndexCount = 6;
//
//	/* Vertex */
//	QuadVertex* pVertices = new QuadVertex[m_iQuadVerCount];
//	NULL_CHECK_RETURN(pVertices, E_FAIL);
//	{
//		pVertices[0].pos = _float3(-1.0f, -1.0f, 0.0f);
//		pVertices[1].pos = _float3(-1.0f, +1.0f, 0.0f);
//		pVertices[2].pos = _float3(+1.0f, +1.0f, 0.0f);
//		pVertices[3].pos = _float3(+1.0f, -1.0f, 0.0f);
//
//		pVertices[0].normal = _float3(0.0f, 0.0f, 0.0f);
//		pVertices[1].normal = _float3(1.0f, 0.0f, 0.0f);
//		pVertices[2].normal = _float3(2.0f, 0.0f, 0.0f);
//		pVertices[3].normal = _float3(3.0f, 0.0f, 0.0f);
//
//		pVertices[0].tex = _float2(0.0f, 1.0f);
//		pVertices[1].tex = _float2(0.0f, 0.0f);
//		pVertices[2].tex = _float2(1.0f, 0.0f);
//		pVertices[3].tex = _float2(1.0f, 1.0f);
//
//		D3D11_BUFFER_DESC vertexBufferDesc;
//		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
//		vertexBufferDesc.ByteWidth = sizeof(QuadVertex) * m_iQuadVerCount;
//		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		vertexBufferDesc.CPUAccessFlags = 0;
//		vertexBufferDesc.MiscFlags = 0;
//		vertexBufferDesc.StructureByteStride = 0;
//
//		D3D11_SUBRESOURCE_DATA vertexData;
//		vertexData.pSysMem = pVertices;
//		vertexData.SysMemPitch = 0;
//		vertexData.SysMemSlicePitch = 0;
//
//		FAILED_CHECK(m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_ScreenQuadVB));
//	}
//
//	/* Index */
//	_ulong* pIndices = new _ulong[m_iQuadIndexCount];
//	NULL_CHECK_RETURN(pIndices, E_FAIL);
//	{
//		D3D11_BUFFER_DESC  indexBufferDesc;
//		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
//		indexBufferDesc.ByteWidth = sizeof(_ulong) * m_iQuadIndexCount;
//		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		indexBufferDesc.CPUAccessFlags = 0;
//		indexBufferDesc.MiscFlags = 0;
//		indexBufferDesc.StructureByteStride = 0;
//
//		pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
//		pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;
//
//		D3D11_SUBRESOURCE_DATA indexData;
//		indexData.pSysMem = pIndices;
//		indexData.SysMemPitch = 0;
//		indexData.SysMemSlicePitch = 0;
//
//		FAILED_CHECK(m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_ScreenQuadIB));
//	}
//
//	Safe_Delete_Array<QuadVertex*>(pVertices);
//	Safe_Delete_Array<_ulong*>(pIndices);
//
//	return S_OK;
//}
//
//void CRenderer::BuildOffsetVectors()
//{
//	// 14개의 균일하게 분포된 벡터로 시작.
//	// 정육면체의 8개의 모서리를 선택, 각 면을 따라 6개 중심점을 선택한다.
//	// 항상 다른 쪽 면을 기준으로 이 점을 번갈아 사용한다. -> 정육면체 반대쪽도 균등하게 가능 
//	// 14개 미만의 샘플링 포인트를 선택할 때에도 벡터를 균등하게 분산시킬 수 있다.
//
//	// 8 cube corners
//	m_vOffsets[0] = _float4(+1.0f, +1.0f, +1.0f, 0.0f);
//	m_vOffsets[1] = _float4(-1.0f, -1.0f, -1.0f, 0.0f);
//
//	m_vOffsets[2] = _float4(-1.0f, +1.0f, +1.0f, 0.0f);
//	m_vOffsets[3] = _float4(+1.0f, -1.0f, -1.0f, 0.0f);
//
//	m_vOffsets[4] = _float4(+1.0f, +1.0f, -1.0f, 0.0f);
//	m_vOffsets[5] = _float4(-1.0f, -1.0f, +1.0f, 0.0f);
//
//	m_vOffsets[6] = _float4(-1.0f, +1.0f, -1.0f, 0.0f);
//	m_vOffsets[7] = _float4(+1.0f, -1.0f, +1.0f, 0.0f);
//
//	// 6 centers of cube faces
//	m_vOffsets[8] = _float4(-1.0f, 0.0f, 0.0f, 0.0f);
//	m_vOffsets[9] = _float4(+1.0f, 0.0f, 0.0f, 0.0f);
//
//	m_vOffsets[10] = _float4(0.0f, -1.0f, 0.0f, 0.0f);
//	m_vOffsets[11] = _float4(0.0f, +1.0f, 0.0f, 0.0f);
//
//	m_vOffsets[12] = _float4(0.0f, 0.0f, -1.0f, 0.0f);
//	m_vOffsets[13] = _float4(0.0f, 0.0f, +1.0f, 0.0f);
//
//	for (_uint i = 0; i < 14; ++i)
//	{
//		// [0.25, 1.0] 사이의 임의의 벡터를 만든다.
//		_float fRandom = SMath::fRandom(0.25f, 1.0f);
//
//		_vector v = fRandom * XMVector4Normalize(XMLoadFloat4(&m_vOffsets[i]));
//
//		XMStoreFloat4(&m_vOffsets[i], v);
//	}
//}
//
//void CRenderer::BuildRandomVectorTexture()
//{
//	/* HLSL에 난수생성기가 없기 때문에 셰이더에서 사용할 수 있는 무작위 벡터로 채워진 텍스쳐를 만들어야한다. */
//	D3D11_TEXTURE2D_DESC textureDesc;
//	textureDesc.Width = 256;
//	textureDesc.Height = 256;
//	textureDesc.MipLevels = 1;
//	textureDesc.ArraySize = 1;
//	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	textureDesc.SampleDesc.Count = 1;
//	textureDesc.SampleDesc.Quality = 0;
//	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
//	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	textureDesc.CPUAccessFlags = 0;
//	textureDesc.MiscFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
//	vertexData.SysMemPitch = 256 * sizeof(XMCOLOR);
//
//	vector<XMCOLOR> color(256 * 256);
//	for (int i = 0; i < 256; ++i)
//	{
//		for (int j = 0; j < 256; ++j)
//		{
//			_float3 vRand = { SMath::fRandom(0.f, 1.f), SMath::fRandom(0.f, 1.f), SMath::fRandom(0.f, 1.f) };
//
//			color[i * 256 + j] = { (uint8_t)vRand.x, (uint8_t)vRand.y, (uint8_t)vRand.z, (uint8_t)0.0f };
//		}
//	}
//	vertexData.pSysMem = color.data();
//
//	ID3D11Texture2D* pTexture = nullptr;
//
//	m_pDevice->CreateTexture2D(&textureDesc, &vertexData, &pTexture);
//
//	m_pDevice->CreateShaderResourceView(pTexture, 0, &m_RandomVectorSRV);
//}
//
//HRESULT CRenderer::RenderScreenQuad()
//{
//	if (nullptr == m_pContext)
//		return E_FAIL;
//
//	_uint stride = sizeof(QuadVertex);
//	_uint offset = 0;
//
//	m_pContext->IASetVertexBuffers(0, 1, &m_ScreenQuadVB, &stride, &offset);
//
//	m_pContext->IASetIndexBuffer(m_ScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
//
//	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	m_pContext->DIndexed(m_iQuadIndexCount, 0, 0);
//
//	return S_OK;
//}

//HRESULT CRenderer::Render_SSAO()
//{
//	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))); /* Target SSAO 단독 */
//
//	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
//	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
//	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
//
//	/* 변수 올리기 */
//	{
//		/* matViewToTEXsPACE*/
//		_matrix P = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);
//		_matrix PT = XMMatrixMultiply(P, m_mTexture);
//		_float4x4 ViewToTexSpcace = {};
//		XMStoreFloat4x4(&ViewToTexSpcace, PT);
//		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_Matrix("ViewToTexSpcace", &ViewToTexSpcace));
//
//		/* Offset */
//		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_RawValue("g_OffsetVector", &m_vOffsets, sizeof(_float4) * 14));
//		//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_
//		/* Frustum*/
//		//SSAO_OnSize();
//		FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Bind_RawValue("FrustumCorner", &m_vFrustumFarCorner, sizeof(_float4) * 4));
//
//		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ViewNormal"), m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING], "g_NormalDepthTarget"));
//		//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING], "g_RandomVectorTexture"));
//		FAILED_CHECK(m_pRandomVectorTexture->Bind_ShaderResource(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING], "g_RandomVectorTexture"));
//
//		/* SSAO : crysis 게임을 위해 crytek에서 개발한 주변 폐색을 근사화하는 빠른기술.
//		 * 장면의 각 픽셀에 대한 법선 및 깊이정보를 포함하는 렌더대상에 장면을 그린다.
//		 * 이 값들을 샘플링하여 각 픽셀의 폐색값을 계산해 디퍼드 셰이더효과에서 폐색값ㅇ르 샘플링하여 조명계산에서 주변항목을 수정할 수 있도록한다. */
//	}
//
//	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_POSTPROCESSING]->Begin(ECast(SSAO_SHADER::SSAO)));
//
//	//FAILED_CHECK(m_pSSAO_VIBuffer->Render());
//
//	FAILED_CHECK(m_pGameInstance->End_MRT());
//
//	return S_OK;
//}
//
//HRESULT CRenderer::Render_SSAO_Blur()
//{
//	Render_Blur(L"Target_SSAO", L"MRT_SSAO_Blur", true, ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER), ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER), ECast(BLUR_SHADER::BLUR_UP_ADD));
//
//	return S_OK;
//}

#pragma endregion
