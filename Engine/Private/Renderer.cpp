#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "AlphaObject.h"
#include "Shader.h"

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

	/* ID3D11DepthStencilView 셍상 */
	FAILED_CHECK(Create_DepthStencil());

#ifdef _DEBUG
	/* 앞서 만든 MRT를 디버그렌더에 그리는용도 */
	FAILED_CHECK(Ready_DebugRender()); 
#endif

	/* ssao용도 - hqao+ 랑 고민중 */
	FAILED_CHECK(Ready_SSAO());


	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroupID, CGameObject * pGameObject)
{
	if (nullptr == pGameObject ||
		eGroupID >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eGroupID].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugRender(CComponent * pDebugCom)
{
	#ifdef _DEBUG
		m_DebugComponent.push_back(pDebugCom);

		Safe_AddRef(pDebugCom);
	#endif

	return S_OK;
}


HRESULT CRenderer::Draw_RenderGroup()
{
	FAILED_CHECK(Render_Priority());	/* SkyBox */
	FAILED_CHECK(Render_Shadow());		/* MRT_Shadow */
	FAILED_CHECK(Render_NonLight()); 
	FAILED_CHECK(Render_NonBlend());	/* MRT_GameObjects*/
	FAILED_CHECK(Render_LightAcc());	/* MRT_LightAcc */
	FAILED_CHECK(Render_Deferred());
	FAILED_CHECK(Render_OutLine());	/* MRT_OutLine */
	FAILED_CHECK(Render_Blend());
	FAILED_CHECK(Render_UI());

#ifdef _DEBUG
	FAILED_CHECK(Render_Debug());
#endif

	return S_OK;
}

HRESULT CRenderer::Create_Buffer()
{
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Create_Shader()
{
	m_pShader[SHADER_TYPE::SHADER_DEFERRED] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_DEFERRED], E_FAIL);

	m_pShader[SHADER_TYPE::SHADER_OUTLINE] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_OutLine.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_OUTLINE], E_FAIL);

	int a = 0;
	//m_pShader[SHADER_TYPE::SHADER_BLUR] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Blur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	//NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_BLUR], E_FAIL);
	//
	//m_pShader[SHADER_TYPE::SHADER_SSAO] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Ssao.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	//NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_SSAO], E_FAIL);
	//
	//m_pShader[SHADER_TYPE::SHADER_FINAL] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Final.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	//NULL_CHECK_RETURN(m_pShader[SHADER_TYPE::SHADER_FINAL], E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Create_RenderTarget()
{
	D3D11_VIEWPORT		Viewport;
	_uint				iNumViewport = { 1 };

	m_pContext->RSGetViewports(&iNumViewport, &Viewport);

	/* RenderTarget */
	{
		/* MRT_GameObject */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));
		
		/* MRT_Shadow */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)));
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
		
		/* MRT_LightAcc */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));
		
		/* MRT_SSAO	*/
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));

		/* MRT_OutLine */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_OutLine"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 0.f)));

		/* MRT_GodRay */
		FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_GodRay"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	}

	/* MRT*/
	{
		/* MRT_GameObject */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth")));

		/* MRT_LightAcc - Q. Ambient 추가하는가 ? */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade")));
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")));
		
		/* MRT_Shadow */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth")));

		/* MRT_SSAO	*/
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO")));
		
		/* MRT_OutLine */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_OutLine"), TEXT("Target_OutLine")));
		
		/* MRT_GodRay */
		FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GodRay"), TEXT("Target_GodRay")));
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

	/* MRT_GameObject */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"),	(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 1.f) , fSizeX, fSizeY));
	//FAILED_CHECK(m_pGameInstance->Render_Font(TEXT("Font_Arial"), L"Target_Diffuse", float2((fSizeX / 2.f * 1.f), (fSizeY / 2.f * 1.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"),	(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"),	(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 5.f),  fSizeX, fSizeY));

	/* MRT_LightAcc */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade"),	(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"), (fSizeX / 2.f * 3.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));

	/* MRT_Shadow */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_LightDepth"), (1280.f - fSizeX *0.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));

	/* MRT_SSAO	*/
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_OutLine"),	(fSizeX / 2.f * 5.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));

	/* MRT_OutLine */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_GodRay"),	(fSizeX / 2.f * 5.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));

	/* MRT_GodRay */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_SSAO"),		(fSizeX / 2.f * 5.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));

	return S_OK;
}

#endif // _DEBUG

HRESULT CRenderer::Ready_SSAO()
{
	/* Quad 준비 */
	FAILED_CHECK(Initialize_ScreenQuad());

	/* 절두체 갱신 */
	BuildFrustumFarCorners();

	BuildOffsetVectors();

	//m_pRandomVectorTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Texture/ETC/RandomTexture.png"));
	//if (nullptr == m_pRandomVectorTexture)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize_ScreenQuad()
{
	m_iQuadVerCount = 4;
	m_iQuadIndexCount = 6;

	QuadVertex* pVertices = new QuadVertex[m_iQuadVerCount];
	NULL_CHECK_RETURN(pVertices, E_FAIL);

	_ulong* pIndices = new _ulong[m_iQuadIndexCount];
	NULL_CHECK_RETURN(pIndices, E_FAIL);

	pVertices[0].pos = _float3(-0.5f, -0.5f, 0.0f);
	pVertices[1].pos = _float3(-0.5f, +0.5f, 0.0f);
	pVertices[2].pos = _float3(+0.5f, +0.5f, 0.0f);
	pVertices[3].pos = _float3(+0.5f, -0.5f, 0.0f);

	pVertices[0].ToFarPlaneIndex = _float3(0.0f, 0.0f, 0.0f);
	pVertices[1].ToFarPlaneIndex = _float3(1.0f, 0.0f, 0.0f);
	pVertices[2].ToFarPlaneIndex = _float3(2.0f, 0.0f, 0.0f);
	pVertices[3].ToFarPlaneIndex = _float3(3.0f, 0.0f, 0.0f);

	pVertices[0].tex = _float2(0.0f, 1.0f);
	pVertices[1].tex = _float2(0.0f, 0.0f);
	pVertices[2].tex = _float2(1.0f, 0.0f);
	pVertices[3].tex = _float2(1.0f, 1.0f);

	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(QuadVertex) * m_iQuadVerCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	FAILED_CHECK(m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pQuadVertexBuffer));

	D3D11_BUFFER_DESC  indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(_ulong) * m_iQuadIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	FAILED_CHECK(m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pQuadIndexBuffer));

	Safe_Delete_Array<QuadVertex*>(pVertices);
	Safe_Delete_Array<_ulong*>(pIndices);

	return S_OK;
}

void CRenderer::BuildFrustumFarCorners()
{
	_float4 CamSetting = m_pGameInstance->Get_CamSetting();

	_float fNest = CamSetting.x;
	_float fFar = CamSetting.y;   
	_float fFovY = CamSetting.z; 
	_float fAspect = CamSetting.w; 

	_float fHalfHeight = fFar * tanf(0.5f * fFovY);
	_float fHalfWidth = fAspect * fHalfHeight;

	m_vFrustumFarCorner[0] = _float4(-fHalfWidth, -fHalfHeight, fFar, 0.0f);
	m_vFrustumFarCorner[1] = _float4(-fHalfWidth, +fHalfHeight, fFar, 0.0f);
	m_vFrustumFarCorner[2] = _float4(+fHalfWidth, +fHalfHeight, fFar, 0.0f);
	m_vFrustumFarCorner[3] = _float4(+fHalfWidth, -fHalfHeight, fFar, 0.0f);
}

void CRenderer::BuildOffsetVectors()
{
	// 14개의 균일하게 분포된 벡터로 시작한다.
	// 큐브의 모서리 점 8개를 선택하고 큐브의 각 면을 따라 중심점을 선택한다.
	// 항상 다른 쪽 면을 기준으로 이 점을 번갈아 사용한다.
	// 이 방법은 14개 미만의 샘플링 포인트를 선택할 때에도 벡터를 균등하게 분산시킬 수 있다.

	{
		// 8개의 큐브 코너 벡터
		m_vOffsets[0] = _float4(+1.0f, +1.0f, +1.0f, 0.0f);
		m_vOffsets[1] = _float4(-1.0f, -1.0f, -1.0f, 0.0f);

		m_vOffsets[2] = _float4(-1.0f, +1.0f, +1.0f, 0.0f);
		m_vOffsets[3] = _float4(+1.0f, -1.0f, -1.0f, 0.0f);

		m_vOffsets[4] = _float4(+1.0f, +1.0f, -1.0f, 0.0f);
		m_vOffsets[5] = _float4(-1.0f, -1.0f, +1.0f, 0.0f);

		m_vOffsets[6] = _float4(-1.0f, +1.0f, -1.0f, 0.0f);
		m_vOffsets[7] = _float4(+1.0f, -1.0f, +1.0f, 0.0f);

		// 6개의 표면 중심점 벡터
		m_vOffsets[8] = _float4(-1.0f, 0.0f, 0.0f, 0.0f);
		m_vOffsets[9] = _float4(+1.0f, 0.0f, 0.0f, 0.0f);

		m_vOffsets[10] = _float4(0.0f, -1.0f, 0.0f, 0.0f);
		m_vOffsets[11] = _float4(0.0f, +1.0f, 0.0f, 0.0f);

		m_vOffsets[12] = _float4(0.0f, 0.0f, -1.0f, 0.0f);
		m_vOffsets[13] = _float4(0.0f, 0.0f, +1.0f, 0.0f);

		m_vOffsets[14] = _float4(-1.0f, 1.0f, 0.0f, 0.0f);
		m_vOffsets[15] = _float4(1.0f, 1.0f, 0.0f, 0.0f);
		m_vOffsets[16] = _float4(0.0f, 1.0f, -1.0f, 0.0f);
		m_vOffsets[17] = _float4(0.0f, 1.0f, 1.0f, 0.0f);

		m_vOffsets[18] = _float4(1.0f, 0.0f, 1.0f, 0.0f);
		m_vOffsets[19] = _float4(-1.0f, 0.0f, 1.0f, 0.0f);
		m_vOffsets[20] = _float4(-1.0f, 0.0f, -1.0f, 0.0f);
		m_vOffsets[21] = _float4(1.0f, 0.0f, -1.0f, 0.0f);

		m_vOffsets[22] = _float4(-1.0f, -1.0f, 0.0f, 0.0f);
		m_vOffsets[23] = _float4(1.0f, -1.0f, 0.0f, 0.0f);
		m_vOffsets[24] = _float4(0.0f, -1.0f, -1.0f, 0.0f);
		m_vOffsets[25] = _float4(0.0f, -1.0f, 1.0f, 0.0f);
	}

	// 난수 데이터 초기화
	mt19937 randEngine;
	randEngine.seed(std::random_device()());
	uniform_real_distribution<_float> randF(0.25f, 1.0f);
	for (_uint i = 0; i < 26; ++i)
	{
		// [0.25, 1.0] 사이의 임의의 벡터를 만든다.
		_float s = randF(randEngine);

		/*m_vOffsets[i].Normalize();
		Vec4 v = s * m_vOffsets[i];
		m_vOffsets[i] = v;*/
	}

}
HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

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
		if (nullptr != pGameObject)
			pGameObject->Render_Shadow();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOW].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280;
	ViewPortDesc.Height = 720;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
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
		if (nullptr != pGameObject)
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
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

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
	/* 디퍼드에 의한 최종장면 */
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1280.0f / 720.0f, 0.1f, m_pGameInstance->Get_CamFar()));

	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_LightViewMatrix", &ViewMatrix));
	FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Bind_Matrix("g_LightProjMatrix", &ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DiffuseTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shade"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_ShadeTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_SpecularTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_DepthTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_LightDepth"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], "g_LightDepthTexture"))

	m_pShader[SHADER_TYPE::SHADER_DEFERRED]->Begin(3);

	m_pVIBuffer->Bind_VIBuffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_OutLine()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_OUTLINE])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_OutLine();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_OUTLINE].clear();

	//FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_OutLine"), m_pLightDepthDSV));

	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	//
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader[SHADER_TYPE::SHADER_OUTLINE],"g_NormalTexture"));
	//
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Bind_RawValue("g_vLineColor", &m_vLineColor, sizeof(_float4)));
	//
	//FAILED_CHECK(m_pShader[SHADER_TYPE::SHADER_OUTLINE]->Begin(0));
	//FAILED_CHECK(m_pVIBuffer->Render());

	//FAILED_CHECK(m_pGameInstance->End_MRT());

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
		Safe_Release(pDebugCom);
	}
	m_DebugComponent.clear();


	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GameObjects"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_LightAcc"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow"), m_pShader[SHADER_TYPE::SHADER_DEFERRED], m_pVIBuffer);

	return S_OK;
}
#endif

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

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

	for (_uint i = 0; i < SHADER_TYPE::SHADER_END; ++i)
		Safe_Release(m_pShader[i]);

	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}


