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
	FAILED_CHECK(Create_Buffer());

	FAILED_CHECK(Create_Shader());

	/* 렌더타겟 생성 + MRT 그룹 지정 */
	FAILED_CHECK(Create_RenderTarget());

	FAILED_CHECK(Create_DepthStencil());

#ifdef _DEBUG
	FAILED_CHECK(Ready_DebugRender());
#endif

	m_tBloomRim_Option.bRimBloom_Blur_Active = true;
	m_tHBAO_Option.bHBAO_Active = false;
	m_tFog_Option.bFog_Active = false;

	m_tRadial_Option.bRadial_Active = false;
	m_tHDR_Option.bHDR_Active = false;
	m_tDOF_Option.bDOF_Active = false;
	m_tHSV_Option.bScreen_Active = false;
	m_tAnti_Option.bFXAA_Active = false;

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
#ifdef _DEBUG
	Control_HotKey();
	if (true == m_bDebugCom)
		FAILED_CHECK(Render_DebugCom()) /* Debug Component -> MRT 타겟에 저장해서 Finaml 에서 추가연산한다. */
#endif // _DEBUG

	m_iCurrentLevel = m_pGameInstance->Get_NextLevel();

	FAILED_CHECK(Render_Priority());	/* MRT_Priority - Target_Priority 저장  */

	FAILED_CHECK(Render_NonLight());	/* RenderGroup*/

	/* --- Pre-Post Processing --- */
	FAILED_CHECK(Render_NonBlend());	/* MRT_GameObjects - Diffuse, Normal, Depth, Bloom, RimLight, - */

	FAILED_CHECK(Render_Shadow());		/* MRT_Shadow - Target_ShadowDepth 저장  */

	FAILED_CHECK(Render_LightAcc());	/* MRT_LightAcc */

	if (m_tHBAO_Option.bHBAO_Active)
		FAILED_CHECK(Render_HBAO_PLUS());

	FAILED_CHECK(Render_Deferred()); /*  MRT_Deferred -> Target_Deferred에 저장  */

	FAILED_CHECK(Render_RimBloom());

	FAILED_CHECK(Deferred_Effect()); 

	/* --- Post Processing --- */
	
	if (true == m_tRadial_Option.bRadial_Active) /* 이미지 블러효과를 추가하는것 */
		FAILED_CHECK(Render_RadialBlur());

	if(true == m_tDOF_Option.bDOF_Active)
		FAILED_CHECK(Render_DOF());

	if (true == m_tHDR_Option.bHDR_Active)
		FAILED_CHECK(Render_HDR()); /* 톤매핑 - 렌더링 파이프라인의 초기단계에서 적용됨  */

	if (true == m_tAnti_Option.bFXAA_Active)
		FAILED_CHECK(Render_FXAA()); /* 안티앨리어싱 */

	if(true == m_tHSV_Option.bScreen_Active)
		FAILED_CHECK(Render_HSV()); /* 컬러 그레이딩 - 최종장면 */

	if (true == m_bUI_MRT)
		FAILED_CHECK(Render_UI_Tool()); /* Tool에서 체크할 때  */

	/* 최종 합성 */ 
	FAILED_CHECK(Render_Final());

	FAILED_CHECK(Render_Blend());  


	if (false == m_bUI_MRT)
		FAILED_CHECK(Render_UI()); /* GamePlay에서 확인할때 여기활성화 */

#ifdef _DEBUG
	if (true == m_bDebugRenderTarget)
		FAILED_CHECK(Render_DebugTarget());
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

HRESULT CRenderer::Render_LightAcc()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc")));

	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	_float		CamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_Deferred, "g_NormalTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTarget"));

	m_pGameInstance->Render_Lights(m_pShader_Deferred, m_pVIBuffer);

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
		_float fFar = 1000.f;
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

HRESULT CRenderer::Render_RimBloom()
{
	if (m_tBloomRim_Option.bRimBloom_Blur_Active)
	{
		FAILED_CHECK(Render_Blur(TEXT("Target_RimBloom"), TEXT("MRT_RB_Blur"),
								ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
								ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
								ECast(BLUR_SHADER::BLUR_UP_ADD), true));
		m_bBloomBlur_Clear = false;
	}
	else
	{
		if (false == m_bBloomBlur_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_RB_Blur"));

			m_bBloomBlur_Clear = true;
		}
	}
	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Deferred")));

	/* 디퍼드에 의한 최종장면 */
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	_float			CamFar = m_pGameInstance->Get_CamFar();
	_float4			CamPos = m_pGameInstance->Get_CamPosition();
	_float			LightFar = m_pGameInstance->Get_ShadowLightFar(m_iCurrentLevel);

	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_iCurrentLevel)));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_iCurrentLevel)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_LightFar", &LightFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4)));

	/* 활성여부 */
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_bSSAO_Active", &m_tHBAO_Option.bHBAO_Active, sizeof(_bool)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_bFog_Active", &m_tFog_Option.bFog_Active, sizeof(_bool)));

	/* 타겟에 값올리기 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority"), m_pShader_Deferred, "g_PriorityTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shade"), m_pShader_Deferred, "g_ShadeTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader_Deferred, "g_SpecularTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShadowDepth"), m_pShader_Deferred, "g_ShadowDepthTexture"));

	/* 선택사항*/
	if (true == m_tHBAO_Option.bHBAO_Active)
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO"), m_pShader_Deferred, "g_SSAOTexture")); /* ssao 추가 */

	if (true == m_tFog_Option.bFog_Active)
	{
		FAILED_CHECK(m_pShader_Deferred->Bind_Struct("g_Fogdesc", &m_tFog_Option, sizeof(FOG_DESC)));
		FAILED_CHECK(m_pPerlinNoiseTextureCom->Bind_ShaderResource(m_pShader_Deferred, "g_PerlinNoiseTexture"));
	}

	FAILED_CHECK(m_pShader_Deferred->Begin(ECast(DEFERRED_SHADER::DEFERRED)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_EffectBloomBlur()
{
	/* MRT_Effect_Blur -> Target_Effect_RR_Blur로 결과 나옴 */
	Render_Blur(TEXT("Target_Effect_RimBloom"), TEXT("MRT_Effect_Blur"), 
				ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
				ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
				ECast(BLUR_SHADER::BLUR_UP_ADD), true);

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	return E_NOTIMPL;
}

HRESULT CRenderer::Render_HDR()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_HDR"))); /* Target_FXAA*/

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	/* 값 컨트롤용도 */
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_max_white", &m_tHDR_Option.fmax_white, sizeof(_float)));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::HDR), m_pShader_PostProcess, "g_ProcessingTarget"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_HDR)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	/* 블러 적용한 타겟화면 */
	wstring Before = Current_Target(POST_TYPE::DOF);

	Render_Blur(Before, TEXT("MRT_DOFBlur"),
				ECast(BLUR_SHADER::BLUR_HORIZON_LOW),
				ECast(BLUR_SHADER::BLUR_VERTICAL_LOW),
				ECast(BLUR_SHADER::BLUR_UP_ADD), true); 

	/* DOF 적용할 렌더 */
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_DOF"))); 

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	_float		CamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_fCamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_DOF", &m_tDOF_Option, sizeof(DOF_DESC)));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_PostProcess, "g_DepthTarget")); /* 뷰공간 깊이화면 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_DOFBlur"), m_pShader_PostProcess, "g_BlurTarget")); /* 블러 화면 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Before, m_pShader_PostProcess, "g_ProcessingTarget")); /* 일반 화면 */
	
	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_DOF)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_RaidalBlur")));

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_Radial_Blur", &m_tRadial_Option, sizeof(RADIAL_DESC)));
	
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::RADIAL_BLUR), m_pShader_PostProcess, "g_ProcessingTarget"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_RADIAL)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_FXAA()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_FXAA"))); /* Target_FXAA*/

	FAILED_CHECK(m_pShader_Fxaa->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Fxaa->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Fxaa->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Fxaa->Bind_RawValue("g_bFxaa", &m_tAnti_Option.bFXAA_Active, sizeof(ANTI_DESC)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::FXAA), m_pShader_Fxaa, "g_FinalTarget"));
	
	FAILED_CHECK(m_pShader_Fxaa->Begin(0));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_HSV()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_HSV"))); /* Target_FXAA*/

	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Final->Bind_RawValue("g_HSV_DESC", &m_tHSV_Option, sizeof(HSV_DESC)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::HSV), m_pShader_Final, "g_FinalTarget"));

	FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::FINAL_HSV)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::FINAL), m_pShader_Final, "g_FinalTarget")); /* 이전까지 그린 타겟 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_UI_Diffuse"), m_pShader_Final, "g_UI_Target")); /* ui그린거 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Debug"), m_pShader_Final, "g_DebugTarget")); /* DebugCom 그린거 */

	FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::FINAL)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	return S_OK;
}

#pragma endregion

HRESULT CRenderer::Render_Effect()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect")));

	/* CEffect_Void - CAlphaObject 자식클래스 확인 */
	m_RenderObjects[RENDER_EFFECT].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return ((CAlphaObject*)pSour)->Get_CamDistance() > ((CAlphaObject*)pDest)->Get_CamDistance();
		});

	for (auto& pGameObject : m_RenderObjects[RENDER_EFFECT])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_EFFECT].clear();

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

HRESULT CRenderer::Deferred_Effect()
{
	/* Effect 추가해서 사용할거면 추가가능합니다~ */
	FAILED_CHECK(Render_Effect());	/* MRT_Effect : Target_Effect_Diffuse, Target_Effect_Normal, Target_Effect_Depth, Target_Effect_RimBloom */
	
	FAILED_CHECK(Render_EffectBloomBlur());	/* Render에 블룸블러 효과 MRT_Effect_Blur -> Target_Effect_RR_Blur에 저장됨 */

	//FAILED_CHECK(Render_EffectDistortion());	/* Render에 블룸블러 효과 MRT_Effect_Blur -> Target_Effect_RR_Blur에 저장됨 */

	FAILED_CHECK(Render_Effect_Final()); /* Deferred + Effect + EffectBloomBlur */

	m_ePrevTarget = POST_TYPE::DEFERRED;

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Final()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect_Final")));

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	/* 기존에 그린 Deferred + Object BloomBlur + Effect BloomBlur + Effect Diffuse + Effect_Solid */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_PostProcess, "g_Deferred_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_RB_BlurActive"), m_pShader_PostProcess, "g_RimBlur_Target"));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Diffuse"), m_pShader_PostProcess, "g_Effect_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Solid"), m_pShader_PostProcess, "g_Effect_Solid"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_RR_Blur"), m_pShader_PostProcess, "g_EffectBlur_Target"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_EFFECTMIX)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Deferred_UI()
{
	/* UI 추가해서 사용할거면 추가가능합니다~ */
	FAILED_CHECK(Render_UI()); 

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI_Tool()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_UI")));

	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}


HRESULT CRenderer::Render_Blur(const wstring& strStartTargetTag, const wstring& strFinalMRTTag, _int eHorizontalPass, _int eVerticalPass, _int eBlendType, _bool bClear)
{
	FAILED_CHECK(Render_Blur_DownSample(strStartTargetTag));

	FAILED_CHECK(Render_Blur_Horizontal(eHorizontalPass));

	FAILED_CHECK(Render_Blur_Vertical(eVerticalPass));

	FAILED_CHECK(Render_Blur_UpSample(strFinalMRTTag, bClear, eBlendType));

	return S_OK;
}

#pragma region Blur

HRESULT CRenderer::Render_Blur_DownSample(const wstring& strStartTargetTag)
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_DownSampling")));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(strStartTargetTag, m_pShader_Blur, "g_BlurTarget"));

	_float4x4 WorldMatrix = m_WorldMatrix;
	WorldMatrix._11 = m_WorldMatrix._11 * 0.5f;
	WorldMatrix._22 = m_WorldMatrix._22 * 0.5f;

	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_WorldMatrix", &WorldMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Blur->Begin(ECast(BLUR_SHADER::BLUR_DOWN)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blur_Horizontal(_int eHorizontalPass)
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Horizontal")));

	/* 다운샘플링이 끝난걸 수행하므로 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_DownSampling"), m_pShader_Blur, "g_BlurTarget"));

	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Blur->Begin(eHorizontalPass));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blur_Vertical(_int eVerticalPass)
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Vertical")));

	/* 수평샘플링이 끝난걸 수행 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_Horizontal"), m_pShader_Blur, "g_BlurTarget"));

	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Blur->Begin(eVerticalPass));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Blur_UpSample(const wstring& strFinalMrtTag, _bool bClear, _int eBlendType)
{
	/* 원래사이즈로 다시 되돌림 */
	FAILED_CHECK(m_pGameInstance->Begin_MRT(strFinalMrtTag, nullptr, bClear));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_Vertical"), m_pShader_Blur, "g_BlurTarget"));

	_float4x4 WorldMatrix = m_WorldMatrix;
	WorldMatrix._11 = m_WorldMatrix._11 * 2.f;
	WorldMatrix._22 = m_WorldMatrix._22 * 2.f;

	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_WorldMatrix", &WorldMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Blur->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pShader_Blur->Begin(eBlendType));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Deferred_Shadow()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pLightDepthDSV));

	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOW])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render_Shadow();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOW].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	FAILED_CHECK(Render_ShadowBlur());

	return S_OK;
}

HRESULT CRenderer::Render_ShadowBlur()
{
	FAILED_CHECK(Render_Blur(TEXT("Target_ShadowDepth"), TEXT("MRT_ShadowBlur"),
				 ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
				 ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
				 ECast(BLUR_SHADER::BLUR_UP_ADD), true));

	return S_OK;
}

HRESULT CRenderer::Render_Alphablend(const wstring& Begin_MRT_Tag, const wstring& Blend_Target_Tag)
{
	// Render_Alphablend(TEXT("MRT_Final"), TEXT("Target_Deferred"));
	// MRT의 타겟으로 BlendTarget과 섞는것 

	FAILED_CHECK(m_pGameInstance->Begin_MRT(Begin_MRT_Tag, m_pLightDepthDSV));

	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Blend_Target_Tag, m_pShader_Final, "g_BlendMixTarget")); /* Nonblend 결과 - SV_3번 깊이값 */

	FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::FINAL_MIX)));
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

wstring CRenderer::Current_Target(POST_TYPE eCurrType)
{
	/* 순서 : Radial, dof, hdr, fxaa, hsv */

	if (eCurrType == POST_TYPE::RADIAL_BLUR)
	{
		strCurrentTarget = TEXT("Target_Effect_Final");
		m_ePrevTarget = POST_TYPE::RADIAL_BLUR;
	}
	else 
	{
		switch (m_ePrevTarget)
		{
		case POST_TYPE::DEFERRED:
			strCurrentTarget = TEXT("Target_Effect_Final");
			m_ePrevTarget = eCurrType;
			break;
		
		case POST_TYPE::RADIAL_BLUR:
			strCurrentTarget = TEXT("Target_RadialBlur");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::DOF:
			strCurrentTarget = TEXT("Target_DOF");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::HDR:
			strCurrentTarget = TEXT("Target_HDR");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::FXAA:
			strCurrentTarget = TEXT("Target_FXAA");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::HSV:
			strCurrentTarget = TEXT("Target_HSV");
			m_ePrevTarget = eCurrType;
			break;


		}
	}
	return strCurrentTarget;
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
	NULL_CHECK_RETURN(pDebugCom, S_OK);

	m_DebugComponent.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);
#endif

	return S_OK;
}
#pragma endregion

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
	m_pShader_Deferred = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_Deferred, E_FAIL);

	m_pShader_PostProcess = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_PostProcess, E_FAIL);

	m_pShader_Blur = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Blur.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_Blur, E_FAIL);

	m_pShader_Fxaa = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Fxaa3_11.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_Fxaa, E_FAIL);

	m_pShader_Final = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Final.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_Final, E_FAIL);

	m_pShader_Effect = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect_Engine.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_Effect, E_FAIL);

	m_pShader_UI = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Engine.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	NULL_CHECK_RETURN(m_pShader_UI, E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Create_RenderTarget()
{
	D3D11_VIEWPORT		Viewport;
	_uint				iNumViewport = { 1 };

	m_pContext->RSGetViewports(&iNumViewport, &Viewport);

	/* MRT_Priority */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Priority"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Priority"), TEXT("Target_Priority")));

	/* MRT_GameObject*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f))); /* 깊이버퍼 그 깊이 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_ORM"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RimBloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); /* 넣은거 알아서 블러되도록 처리함 */
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))); /* 카메라에서 본 깊이버퍼 */
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ORM")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_RimBloom"))); 

	/* MRT_Bloom_Blur*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RB_BlurActive"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_RB_Blur"), TEXT("Target_RB_BlurActive")));

	/* MRT_LightAcc -> Directional , Spotlight, 등 받아온것 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))); /* 빛에서 본 깊이버퍼 */
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")));

	/* MRT_Deferred_Shadow */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow_Deferred"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Deferred"), TEXT("Target_Shadow_Deferred")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Blur"), TEXT("Target_Shadow_Blur")));

	/* MRT_Shadow -> Render_Shadow 로 받아온값 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_ShadowDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_ShadowDepth")));

	/* MRT_Blur	*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_DownSampling"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_DownSampling"), TEXT("Target_Blur_DownSampling")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Horizontal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Horizontal"), TEXT("Target_Blur_Horizontal")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Vertical"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Vertical"), TEXT("Target_Blur_Vertical")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_UpSampling"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_UpSampling"), TEXT("Target_Blur_UpSampling")));

	/* MRT_Distortion */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion")));

	/* MRT_Distortion_Blur*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion_Blur")));

	/* MRT_HBAO+ */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HBAO"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f)))

	/* MRT_Deferred */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Deferred"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Deferred")));

	/* MRT_HDR */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HDR"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR")));

	/* MRT_RadialBlur */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_RaidalBlur"), TEXT("Target_RadialBlur")));

	/* MRT_DOF */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_DOF"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_DOFBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_DOFBlur"), TEXT("Target_DOFBlur")));

	/* MRT_FXAA */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_FXAA"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_FXAA"), TEXT("Target_FXAA")));

	/* MRT_LightShaft */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightShaft"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightShaft"), TEXT("Target_LightShaft")));

	/* MRT_HSV */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HSV"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_HSV"), TEXT("Target_HSV")));

	/* MRT_Final */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final")));
	
	/* MRT_Debug */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Debug"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Debug"), TEXT("Target_Debug")));

	/* MRT_Blend */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blend"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blend"), TEXT("Target_Blend")));

	/* MRT_Effect - clear color가 111의 흰색일경우 이펙트에 묻어나와서 무조건 000 으로 클리어해야함  */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Solid"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));	/* 해골 안투명하게 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))); /* 깊이버퍼 그 깊이 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_RimBloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))); /* 넣은거 알아서 블러되도록 처리함 */
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Solid")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Normal")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Depth"))); /* 카메라에서 본 깊이버퍼 */
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_RimBloom")));
	
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_RR_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Blur"), TEXT("Target_Effect_RR_Blur")));
	
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Final"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Final"), TEXT("Target_Effect_Final")));

	/* MRT_UI */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_UI"), TEXT("Target_UI_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Tool"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_UI_Tool"), TEXT("Target_UI_Tool")));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));

	return S_OK;
}

HRESULT CRenderer::Create_DepthStencil()
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

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

	FAILED_CHECK(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthDSV));

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Ready_DebugRender()
{
	_float fSizeX = 150.f;
	_float fSizeY = 100.f;
	_float fBigX = 300.f;
	_float fBigY = 200.f;

	/* MRT_GameObject - 좌상단 Default */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ORM"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RimBloom"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RB_BlurActive"),(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));
	
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade"),		(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"),		(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ShadowDepth"),	(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
	
	/* MRT_Effect 체크 */
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Diffuse"),	((fBigX / 2.f * 7)), (fBigY / 2.f * 1.f), fBigX, fBigY));
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_RimBloom"),	((fBigX / 2.f * 7)), (fBigY / 2.f * 3.f), fBigX, fBigY));
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_RR_Blur"),	((fBigX / 2.f * 7)), (fBigY / 2.f * 5.f), fBigX, fBigY));
	
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

HRESULT CRenderer::Render_DebugCom()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Debug")));

	for (auto& pDebugCom : m_DebugComponent)
	{
		if (nullptr == pDebugCom)
			continue;

		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponent.clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_DebugTarget()
{
	m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GameObjects"), m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_RB_Blur"), m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_LightAcc"), m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow"), m_pShader_Deferred, m_pVIBuffer);
	 
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect"), m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect_Blur"), m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_UI"), m_pShader_Deferred, m_pVIBuffer);

	return S_OK;
}

HRESULT CRenderer::Control_HotKey()
{
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		cout << " ----------------------------- " << endl;
		cout << " DIK_1 : (Pre) Object Blur ON/OFF " << endl;
		cout << " DIK_2 : (Pre) Effect Blur ON/OFF" << endl;
		cout << " DIK_3 : (Pre) HBAO+ ON/OFF " << endl;
		cout << " DIK_4 : (Pre) Fog ON/OFF " << endl;

		cout << " DIK_5 : (Post) Radial Blur ON/OFF " << endl;
		cout << " DIK_6 : (Post) HDR ON/OFF " << endl;
		cout << " DIK_7 : (Post) FXAA ON/OFF " << endl;

		//cout << " DIK_8 : (Blur) BloomBlur " << endl;
		//cout << " DIK_9 : (Blur) RadialBlur " << endl;

		cout << " --                         -- " << endl;

		if (true == m_tBloomRim_Option.bRimBloom_Blur_Active)
			cout << "BloomBlur : true " << endl;
		else
			cout << "BloomBlur : false " << endl;

		if (true == m_tHBAO_Option.bHBAO_Active)
			cout << "HBAO+ : true " << endl;
		else
			cout << "HBAO+ : false " << endl;


		if (true == m_tFog_Option.bFog_Active)
			cout << "Fog : true " << endl;
		else
			cout << "Fog : false " << endl;

		if (true == m_tRadial_Option.bRadial_Active)
			cout << "Radial Blur : true " << endl;
		else
			cout << "Radial Blur : false " << endl;

		if (true == m_tHDR_Option.bHDR_Active)
			cout << "HDR : true " << endl;
		else
			cout << "HDR : false " << endl;

		cout << " ----------------------------- " << endl;
	}
	if (m_pGameInstance->Key_Down(DIK_1))
		m_tBloomRim_Option.bRimBloom_Blur_Active = !m_tBloomRim_Option.bRimBloom_Blur_Active;


	if (m_pGameInstance->Key_Down(DIK_3))
		m_tHBAO_Option.bHBAO_Active = !m_tHBAO_Option.bHBAO_Active;
	if (m_pGameInstance->Key_Down(DIK_4))
		m_tFog_Option.bFog_Active = !m_tFog_Option.bFog_Active;
	if (m_pGameInstance->Key_Down(DIK_5))
		m_tRadial_Option.bRadial_Active = !m_tRadial_Option.bRadial_Active;
	if (m_pGameInstance->Key_Down(DIK_6))
		m_tHDR_Option.bHDR_Active = !m_tHDR_Option.bHDR_Active;
	if (m_pGameInstance->Key_Down(DIK_7))
		m_tAnti_Option.bFXAA_Active = !m_tAnti_Option.bFXAA_Active;
	//if (m_pGameInstance->Key_Down(DIK_8))
	//if (m_pGameInstance->Key_Down(DIK_9))

	return S_OK;
}
#endif // _DEBUG

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

#endif // _DEBUG

	Safe_Release(m_pPerlinNoiseTextureCom);
	Safe_Release(m_pLightDepthDSV);

	Safe_Release(m_pShader_Deferred);
	Safe_Release(m_pShader_PostProcess);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_Fxaa);
	Safe_Release(m_pShader_Final);
	Safe_Release(m_pShader_UI);
	Safe_Release(m_pShader_Effect);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#pragma endregion
