﻿#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_SSAO.h"
#include "AlphaObject.h"
#include "Shader.h"
#include "SMath.h"
#include "RenderTarget.h"
#include "Light.h"

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
	//FAILED_CHECK(Ready_CascadeShadow());

	FAILED_CHECK(Create_Buffer());

	FAILED_CHECK(Create_Shader());

	FAILED_CHECK(Create_Texture());

	/* 렌더타겟 생성 + MRT 그룹 지정 */
	FAILED_CHECK(Create_RenderTarget());

	FAILED_CHECK(Create_DepthStencil());

#ifdef _DEBUG
	FAILED_CHECK(Ready_DebugRender());

	FAILED_CHECK(GraphicDebug_Shader());
#endif

	/* Logo는 모두 비활성화임 */
	m_tDeferred_Option.bRimBloom_Blur_Active = false;
	m_tDeferred_Option.bShadow_Active = false;
	m_tHBAO_Option.bHBAO_Active = false;
	m_tFog_Option.bFog_Active = false;

	m_tRadial_Option.bRadial_Active = false;
	m_tHDR_Option.bHDR_Active = false;
	m_tDOF_Option.bDOF_Active = false;
	m_tHSV_Option.bScreen_Active = false;
	m_tAnti_Option.bFXAA_Active = false;
	m_tPBR_Option.bPBR_ACTIVE = false;

	/* 이거 픽스 */
	m_tSSR_Option.bSSR_Active = false;
	m_bRenderUI_Tool = false;

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
#ifdef _DEBUG
	if (true == m_bDebugCom)
		FAILED_CHECK(Render_DebugCom()) /* Debug Component -> MRT 타겟에 저장해서 Finaml 에서 추가연산한다. */
#endif // _DEBUG

	FAILED_CHECK(Check_RenderEffect()); /* 플레이어 죽었을때 */

	FAILED_CHECK(Render_Priority());	/* MRT_Priority - Target_Priority  */
	
	FAILED_CHECK(Render_ShadowDepth());		/* Render_Shadow 콜 : Target_ShadowDepth 저장  */

	FAILED_CHECK(Render_NonLight());	/* RenderGroup*/

	/* --- Pre-Post Processing --- */
	FAILED_CHECK(Render_NonBlend());	/* MRT_GameObjects - Diffuse, Normal, Depth, Bloom */

	FAILED_CHECK(Render_OutLine()); /* MRT_OutLine -> Target_OutLine 에 저장 */
	
	FAILED_CHECK(Render_OutLine_Blur());

	FAILED_CHECK(Bake_ViewShadow());		/* 디퍼드에서 하던 shadow 연산 : Target_ViewShadow 에 저장  + 블러 : Target_Blur_ViewShadow 에 저장됨  */
	
	if (true == m_tHBAO_Option.bHBAO_Active)
		FAILED_CHECK(Render_HBAO_PLUS()); /* Target_HBAO */

	FAILED_CHECK(Render_LightAcc());	/* MRT_LightAcc - Target_Shade, Target_Specular */

	if ((true == m_tPBR_Option.bPBR_ACTIVE) && (true == m_tHBAO_Option.bHBAO_Active))
	{
		// 1. case 
		FAILED_CHECK(Render_MyPBR()); /*  MRT_Deferred -> Target_Deferred에 저장  */

		// 2. case 
		//FAILED_CHECK(Render_PBR());
	}
	else
	{
		FAILED_CHECK(Render_Deferred()); /*  MRT_Deferred -> Target_Deferred에 저장  */
		//FAILED_CHECK(Render_Test()); /*  MRT_Deferred -> Target_Deferred에 저장  */
	}	

	FAILED_CHECK(Render_RimBloom()); /* MRT_RB_Blur -> Target_RB_BlurActive에 저장 -> 파이널에서 처리중인데.. */
	
	FAILED_CHECK(Deferred_Effect());

	/*-- - Post Processing-- - */

	FAILED_CHECK(Render_Fog());

	//FAILED_CHECK(Render_SSR());
	//FAILED_CHECK(Render_Godray());

	FAILED_CHECK(Render_DOF());
	
	FAILED_CHECK(Render_HDR()); /* 톤매핑 - 렌더링 파이프라인의 초기단계에서 적용됨  */
	
	/* 이미지 블러효과를 추가하는것 */
	FAILED_CHECK(Render_RadialBlur());
	
	FAILED_CHECK(Render_FXAA()); /* 안티앨리어싱 */
	
	FAILED_CHECK(Render_HSV()); /* 컬러 그레이딩 - 최종장면 */
	
	FAILED_CHECK(Render_Vignette()); /* 컬러 그레이딩 - 최종장면 */
	
	FAILED_CHECK(Render_Chroma());
	
	FAILED_CHECK(Render_LumaSharpen());
	
	//if(true == m_tMotionBlur_Desc.bMotionBLur_Active)
	//	FAILED_CHECK(Render_MotionBlur());
	//m_matPreCameraView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);
	
	/* ------------------------------ */
	if (true == m_bUI_MRT)
		FAILED_CHECK(Render_UI_Tool()); /* Tool에서 체크할 때  */
	
	FAILED_CHECK(Render_Blend());

	/* 최종 합성 */ 
	FAILED_CHECK(Render_Final());

	/* 최종화면 전체 검수 */
	FAILED_CHECK(Render_ShaderResult());
	 

	if (false == m_bUI_MRT && m_bRenderUI_Tool == false)
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

HRESULT CRenderer::Render_ShadowDepth()
{
	if (true == m_tDeferred_Option.bShadow_Active)
	{	
		/* 그림자의 깊이를 만들었음 - Light 기준으로 */
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

	}
	else
	{
		if (false == m_bShadow_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_Shadow"));
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_Cascade()
{
	for (_uint i = 0; i < 3; ++i)
	{
		wstring szTargetName = L"MRT_Cascade";
		szTargetName += to_wstring(i + 1);

		FAILED_CHECK(m_pGameInstance->Begin_MRT(szTargetName, m_pCascadeShadowDSV[i]));

		for (auto& iter : m_CascadeObjects[i])
		{
			FAILED_CHECK(iter->Render_CSM(i));
			Safe_Release(iter);
		}
		m_CascadeObjects[i].clear();
	}

	FAILED_CHECK(m_pGameInstance->End_MRT());

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
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_bPBR", &m_tPBR_Option.bPBR_ACTIVE, sizeof(_bool)));

	/* Target */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_Deferred, "g_NormalTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTarget")); /* GameObject 에서의 Depth */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ORM"), m_pShader_Deferred, "g_ORMTexture"));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO"), m_pShader_Deferred, "g_SSAOTexture")); //

	m_pGameInstance->Render_Lights(m_pShader_Deferred, m_pVIBuffer);

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Bake_ViewShadow()
{
	if (true == m_tDeferred_Option.bShadow_Active)
	{
		FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_ViewShadow")));

		/* Value */
		_float			CamFar = m_pGameInstance->Get_CamFar();
		_float			LightFar = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());

		FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float)));
		FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_LightFar", &LightFar, sizeof(_float)));

		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_iCurrentLevel)));
		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_iCurrentLevel)));
		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW))); //
		FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ))); //

		/* Target */
		//if (FAILED(m_pShader->Set_ShaderResourceView("g_StaticShadowDepthTexture", pRenderTargetManager->Get_SRV(TEXT("Target_StaticShadowDepth"))))) 
		/* 플레이어 위치에 따라 유동적으로 움직이는 빛에대한 타겟 */

		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShadowDepth"), m_pShader_Deferred, "g_ShadowDepthTexture")); /* Render_Shadow 결과  */
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTarget")); /* GameObject의 Depth */

		FAILED_CHECK(m_pShader_Deferred->Begin(ECast(DEFERRED_SHADER::VIEW_SHADOW)));
		FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
		FAILED_CHECK(m_pVIBuffer->Render());

		FAILED_CHECK(m_pGameInstance->End_MRT());

		/* 위의 결과를 블러먹이기 */
		FAILED_CHECK(Render_Blur(TEXT("Target_ViewShadow"), TEXT("MRT_Blur_ViewShadow"),
			ECast(BLUR_SHADER::BLUR_HORIZON_MIDDLE),
			ECast(BLUR_SHADER::BLUR_VERTICAL_MIDDLE),
			ECast(BLUR_SHADER::BLUR_UP_ADD), true));

		m_bShadow_Clear = false;
	}
	else
	{
		if (false == m_bShadow_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_ViewShadow"));
			m_pGameInstance->Clear_MRT(TEXT("MRT_Blur_ViewShadow"));
		}
	}
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

	GFSDK_SSAO_Output_D3D11 Output = {};
	Output.pRenderTargetView = pView;

	Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

	GFSDK_SSAO_Status status = {};
	status = m_pGameInstance->Get_AOContext()->RenderAO(m_pContext, Input, Params, Output);
	//assert(status == GFSDK_SSAO_OK);
	m_bHBAO_Clear = false;

	return S_OK;
}

HRESULT CRenderer::Render_RimBloom()
{
	if (m_tDeferred_Option.bRimBloom_Blur_Active)
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
	_float			CamFar = m_pGameInstance->Get_CamFar();
	_float4			CamPos = m_pGameInstance->Get_CamPosition();
	_float			LightFar = m_pGameInstance->Get_ShadowLightFar(m_iCurrentLevel);
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_LightFar", &LightFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_bSSAO_Active", &m_tHBAO_Option.bHBAO_Active, sizeof(_bool)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_bShadow_Active", &m_tDeferred_Option.bShadow_Active, sizeof(_bool)));

	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_iCurrentLevel)));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_iCurrentLevel)));

	/* 타겟에 값올리기 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority"), m_pShader_Deferred, "g_PriorityTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Ambient"), m_pShader_Deferred, "g_AmbientTexture")); /* Light */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader_Deferred, "g_SpecularTexture")); /* Light */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pShader_Deferred, "g_EmissiveTarget")); /* Emissive -> Texture 정리 다되면 하기 */ 
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_ViewShadow"), m_pShader_Deferred, "g_ShadowResult")); /* Shadow */

	if (true == m_tHBAO_Option.bHBAO_Active)
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO"), m_pShader_Deferred, "g_SSAOTexture"));			 /* HBAO+ */

	FAILED_CHECK(m_pShader_Deferred->Begin(ECast(DEFERRED_SHADER::DEFERRED)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Test()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Deferred")));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority"), m_pShader_Deferred, "g_PriorityTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture"));

	FAILED_CHECK(m_pShader_Deferred->Begin(ECast(DEFERRED_SHADER::DEFERRED)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());
	
	return S_OK;
}


HRESULT CRenderer::Render_MyPBR()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Deferred")));

	/* VS_MAIN */
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	/* PS_MAIN */
	_float			CamFar = m_pGameInstance->Get_CamFar();
	_float4			CamPos = m_pGameInstance->Get_CamPosition();
	_float			LightFar = m_pGameInstance->Get_ShadowLightFar(m_iCurrentLevel);
	LIGHT_DESC		DirectionalLight_Desc = m_pGameInstance->Get_DirectionLight()->Get_LightDesc();
	_float4			vLightColor = DirectionalLight_Desc.vDiffuse;
	_float4			vLightDir = DirectionalLight_Desc.vDirection;

	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_vLightDir", &vLightDir, sizeof(_float))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_LightFar", &LightFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_SunColor", &vLightColor, sizeof(_float3))); // BRDF 함수용 
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_iCurrentLevel)));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_iCurrentLevel)));
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_bShadow_Active", &m_tDeferred_Option.bShadow_Active, sizeof(_bool)));

	/* 타겟에 값올리기 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority"), m_pShader_Deferred, "g_PriorityTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO"), m_pShader_Deferred, "g_SSAOTexture")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_Deferred, "g_NormalTexture"));//
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ORM"), m_pShader_Deferred, "g_ORMTexture")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTarget"));//
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pShader_Deferred, "g_EmissiveTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader_Deferred, "g_SpecularTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Ambient"), m_pShader_Deferred, "g_AmbientTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_ViewShadow"), m_pShader_Deferred, "g_ShadowResult"));
	
	/* Cascade Shadow*/
	//FAILED_CHECK(m_pShader_Deferred->Bind_Matrices("g_CascadeProj", m_pGameInstance->Get_Shadow_Proj(), 3)); //
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Cascade1"), m_pShader_Deferred, "g_CascadeTarget1"))
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Cascade2"), m_pShader_Deferred, "g_CascadeTarget2"))
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Cascade3"), m_pShader_Deferred, "g_CascadeTarget3"))

	/* Texture */
	FAILED_CHECK(m_pBRDFTextureCom->Bind_ShaderResource(m_pShader_Deferred, "g_BRDF_Texture"));

	//LEVEL_STATIC (0), LEVEL_LOGO (1), LEVEL_INTRO(2), LEVEL_INTRO_BOSS(3),LEVEL_SNOWMOUNTAIN(4),LEVEL_SNOWMOUNTAINBOSS(5),
	// LEVEL_LAVA(6), LEVEL_TOOL(7),LEVEL_LOADING(8),LEVEL_GAMEPLAY(9),LEVEL_END
	
	switch (m_pGameInstance->Get_NextLevel())
	{
	case 2: // LEVEL_Intro , 놀이터
		FAILED_CHECK(m_pIrradianceTextureCom[4]->Bind_ShaderResource(m_pShader_Deferred, "g_Irradiance"));
		FAILED_CHECK(m_pPreFilteredTextureCom[4]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFiltered"));
		break;

	case 9: // LEVEL_GamePlay
		FAILED_CHECK(m_pIrradianceTextureCom[0]->Bind_ShaderResource(m_pShader_Deferred, "g_Irradiance"));
		FAILED_CHECK(m_pPreFilteredTextureCom[0]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFiltered"));
		break;

	case 3: // LEVEL_INTRO_BOSS
		FAILED_CHECK(m_pIrradianceTextureCom[1]->Bind_ShaderResource(m_pShader_Deferred, "g_Irradiance"));
		FAILED_CHECK(m_pPreFilteredTextureCom[1]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFiltered"));
		break;

	case 4: // LEVEL_SNOWMOUNTAIN
		FAILED_CHECK(m_pIrradianceTextureCom[2]->Bind_ShaderResource(m_pShader_Deferred, "g_Irradiance"));
		FAILED_CHECK(m_pPreFilteredTextureCom[2]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFiltered"));
		break;
	case 5: // LEVEL_SNOWMOUNTAINBOSS
		FAILED_CHECK(m_pIrradianceTextureCom[3]->Bind_ShaderResource(m_pShader_Deferred, "g_Irradiance"));
		FAILED_CHECK(m_pPreFilteredTextureCom[3]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFiltered"));
		break;
		
	case 7: // Tool
		m_bToolLevel = true;
		break;
	}

	if (m_bToolLevel)
	{
		if (m_iPBRTexture_InsteadLevel >= 9)
		{
			m_iPBRTexture_InsteadLevel = 9; // 혹시모르니 여기도 방어코드 추가 
		}
		FAILED_CHECK(m_pTool_IrradianceTextureCom[m_iPBRTexture_InsteadLevel]->Bind_ShaderResource(m_pShader_Deferred, "g_Irradiance"));
		FAILED_CHECK(m_pTool_PreFilteredTextureCom[m_iPBRTexture_InsteadLevel]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFiltered"));
		
	}

	FAILED_CHECK(m_pShader_Deferred->Begin(ECast(DEFERRED_SHADER::MYPBR)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_PBR()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Deferred")));
	
	/* Value */
	_float			CamFar = m_pGameInstance->Get_CamFar();
	_float4			CamPos = m_pGameInstance->Get_CamPosition();
	_float			fDirIntensity = m_pGameInstance->Get_DirectionLight()->Get_LightDesc().fIntensity;
	_float3			IrrScale = { fDirIntensity , fDirIntensity, fDirIntensity };
	_float			Exposure = m_tPBR_Option.fBrightnessOffset + m_tPBR_Option.fSaturationOffset;

	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_CamFar", &CamFar, sizeof(_float))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_IrradianceColorScale", &IrrScale, sizeof(_float3))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_RawValue("g_fExposure", &Exposure, sizeof(_float))); //

	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW))); //
	FAILED_CHECK(m_pShader_Deferred->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ))); //


	/* 타겟에 값올리기 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader_Deferred, "g_DiffuseTexture")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_Deferred, "g_NormalTexture"));//
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader_Deferred, "g_SpecularTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Ambient"), m_pShader_Deferred, "g_AmbientTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Blur_ViewShadow"), m_pShader_Deferred, "g_ViewShadow"))
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_Deferred, "g_DepthTarget"));//
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ORM"), m_pShader_Deferred, "g_ORMTexture")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_HBAO"), m_pShader_Deferred, "g_SSAOTexture")); //

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority"), m_pShader_Deferred, "g_PriorityTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ShadowDepth"), m_pShader_Deferred, "g_ShadowDepthTexture"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Emissive"), m_pShader_Deferred, "g_EmissiveTarget"));

	/* Texture */
	FAILED_CHECK(m_pBRDFTextureCom->Bind_ShaderResource(m_pShader_Deferred, "g_BRDFTexture"));

	switch (m_pGameInstance->Get_NextLevel())
	{
	case 2: // Level_Intro = 테스트맵 
		FAILED_CHECK(m_pTool_IrradianceTextureCom[3]->Bind_ShaderResource(m_pShader_Deferred, "g_IrradianceTexture"));
		FAILED_CHECK(m_pTool_PreFilteredTextureCom[3]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFilterTexture"));
		break;

	case 9: // LEVEL_GamePlay
		FAILED_CHECK(m_pIrradianceTextureCom[0]->Bind_ShaderResource(m_pShader_Deferred, "g_IrradianceTexture"));
		FAILED_CHECK(m_pPreFilteredTextureCom[0]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFilterTexture"));
		break;

	case 3: // LEVEL_INTRO_BOSS
		FAILED_CHECK(m_pIrradianceTextureCom[1]->Bind_ShaderResource(m_pShader_Deferred, "g_IrradianceTexture"));
		FAILED_CHECK(m_pPreFilteredTextureCom[1]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFilterTexture"));
		break;
	case 4: // LEVEL_SNOWMOUNTAIN
		FAILED_CHECK(m_pIrradianceTextureCom[2]->Bind_ShaderResource(m_pShader_Deferred, "g_IrradianceTexture"));
		FAILED_CHECK(m_pPreFilteredTextureCom[2]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFilterTexture"));
		break;
	case 5: // LEVEL_SNOWMOUNTAINBOSS
		//FAILED_CHECK(m_pIrradianceTextureCom[3]->Bind_ShaderResource(m_pShader_Deferred, "g_IrradianceTexture"));
		//FAILED_CHECK(m_pPreFilteredTextureCom[3]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFilterTexture"));
		break;

	case 7:
		m_bToolLevel = true;
		break;
	}

	if (m_bToolLevel)
	{
		FAILED_CHECK(m_pTool_IrradianceTextureCom[m_iPBRTexture_InsteadLevel]->Bind_ShaderResource(m_pShader_Deferred, "g_IrradianceTexture"));
		FAILED_CHECK(m_pTool_PreFilteredTextureCom[m_iPBRTexture_InsteadLevel]->Bind_ShaderResource(m_pShader_Deferred, "g_PreFilterTexture"));
	}
	
	FAILED_CHECK(m_pShader_Deferred->Begin(ECast(DEFERRED_SHADER::PBR)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_SSR()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_SSR")));
	
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	/* 타겟에 값올리기 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_PostProcess, "g_NormalTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::SSR), m_pShader_PostProcess, "g_ProcessingTarget")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_ORM"), m_pShader_PostProcess, "g_ORMTarget")); //
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_PostProcess, "g_DepthTarget")); //

	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_SSR_Desc", &m_tSSR_Option, sizeof(SSR_DESC)));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_SSR)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());
	
	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Effect_BloomBlur()
{
	/* MRT_Effect_Blur -> Target_Effect_RR_Blur로 결과 나옴 */
	Render_Blur(TEXT("Target_Effect_RimBloom"), TEXT("MRT_Effect_Blur"), 
				ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
				ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
				ECast(BLUR_SHADER::BLUR_UP_ADD), true);

	/* MRT_Effect_Priority_Blur -> Target_Effect_Priority_RR_Blur 에 저장 */
	Render_Blur(TEXT("Target_Effect_Priority_RimBloom"), TEXT("MRT_Effect_Priority_Blur"),
		ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
		ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
		ECast(BLUR_SHADER::BLUR_UP_ADD), true);

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Distortion_Blur()
{
	/* MRT_Effect_Blur -> Target_Effect_RR_Blur로 결과 나옴 */
	Render_Blur(TEXT("Target_Distortion"), TEXT("MRT_DistortionBlur"),
		ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
		ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
		ECast(BLUR_SHADER::BLUR_UP_ADD), true);

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	if (true == m_tHDR_Option.bHDR_Active)
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

		m_bHDR_Clear = false;
	}
	else
	{
		if (false == m_bHDR_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_HDR"));

			m_bHDR_Clear = true;
		}
	}
	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	/* 블러 적용한 타겟화면 */
	if (true == m_tDOF_Option.bDOF_Active)
	{
		wstring Before = Current_Target(POST_TYPE::DOF);

		Render_Blur(Before, TEXT("MRT_DOFBlur"),
			ECast(BLUR_SHADER::BLUR_HORIZON_QUARTER),
			ECast(BLUR_SHADER::BLUR_VERTICAL_QUARTER),
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

		m_bDOF_Clear = false;
	}
	else
	{
		if (false == m_bBloomBlur_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_DOF"));

			m_bDOF_Clear = true;
		}
	}
	return S_OK;
}

HRESULT CRenderer::Render_Fog()
{
	if (true == m_tFog_Option.bFog_Active)
	{
		FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Fog")));

		/* Value */
		_float			CamFar = m_pGameInstance->Get_CamFar();
		_float4			vCampos = m_pGameInstance->Get_CamPosition();

		FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_fCamFar", &CamFar, sizeof(_float)));
		FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_vCamPosition", &vCampos, sizeof(_float4)));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW))); //
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ))); //
		FAILED_CHECK(m_pShader_PostProcess->Bind_Struct("g_Fogdesc", &m_tFog_Option, sizeof(FOG_DESC)));

		/* Target */
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::FOG), m_pShader_PostProcess, "g_ProcessingTarget"));
		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_PostProcess, "g_DepthTarget")); /* GameObject의 Depth */

		/* Texture */
		FAILED_CHECK(m_pPerlinNoiseTextureCom->Bind_ShaderResource(m_pShader_PostProcess, "g_PerlinNoiseTexture"));

		FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_FOG)));
		FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
		FAILED_CHECK(m_pVIBuffer->Render());
		FAILED_CHECK(m_pGameInstance->End_MRT());

		m_bFOG_Clear = false;
	}
	else
	{
		if (false == m_bFOG_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_Fog"));

			m_bFOG_Clear = true;
		}
	}
	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	if (true == m_tRadial_Option.bRadial_Active)
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

		m_bRadialBlur_Clear = false;
	}
	else
	{
		if (false == m_bRadialBlur_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_RaidalBlur"));

			m_bRadialBlur_Clear = true;
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_FXAA()
{

	if (true == m_tAnti_Option.bFXAA_Active)
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

		m_bFxaa_Clear = false;
	}
	else
	{
		if (false == m_bFxaa_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_FXAA"));

			m_bFxaa_Clear = true;
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_HSV()
{
	if (true == m_tHSV_Option.bScreen_Active)
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

		m_bHSV_Clear = false;
	}
	else
	{
		if (false == m_bHSV_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_HSV"));

			m_bHSV_Clear = true;
		}
	}
	return S_OK;
}

HRESULT CRenderer::Render_Vignette()
{
	if (true == m_tVignette_Option.bVignette_Active)
	{
		FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Vignette")));

		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

		FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_Vignette_desc", &m_tVignette_Option, sizeof(VIGNETTE_DESC)));

		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::VIGNETTE), m_pShader_PostProcess, "g_ProcessingTarget"));

		FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_VIGNETTE)));
		FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
		FAILED_CHECK(m_pVIBuffer->Render());

		FAILED_CHECK(m_pGameInstance->End_MRT());

		m_bVignette_Clear = false;
	}
	else
	{
		if (false == m_bVignette_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_Vignette"));

			m_bVignette_Clear = true;
		}
	}
	return S_OK;
}


HRESULT CRenderer::Render_Chroma()
{
	if (true == m_tChroma_Option.bChroma_Active)
	{
		//cout << "Renderer - Chroma Active " << endl;
		FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Chroma")));

		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

		FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_Chroma_Desc", &m_tChroma_Option, sizeof(CHROMA_DESC)));

		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::CHROMA), m_pShader_PostProcess, "g_ProcessingTarget"));

		FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_CHROMA)));
		FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
		FAILED_CHECK(m_pVIBuffer->Render());

		FAILED_CHECK(m_pGameInstance->End_MRT());  // Target_Chroma 에 저장 

		m_Chroma_Clear = false;
	}
	else if(false == m_tChroma_Option.bChroma_Active)
	{
		if (false == m_Chroma_Clear)
		{
			//cout << "Renderer - Chroma NoneActive " << endl;
			m_pGameInstance->Clear_MRT(TEXT("MRT_Chroma"));
			m_Chroma_Clear = true;
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_LumaSharpen()
{
	if (true == m_tLumaSharpen_Desc.bLumaSharpen_Active)
	{
		FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_LumaSharpen")));

		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
		FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

		FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_Luma_Desc", &m_tLumaSharpen_Desc, sizeof(LUMASHARPEN_DESC)));

		FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::LUMASHARPEN), m_pShader_PostProcess, "g_ProcessingTarget"));

		FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_LUMASHARPEN)));
		FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
		FAILED_CHECK(m_pVIBuffer->Render());

		FAILED_CHECK(m_pGameInstance->End_MRT());  // Target_Chroma 에 저장 

		m_bLumaSharpen_Clear = false;
	}
	else
	{
		if (false == m_bLumaSharpen_Clear)
		{
			m_pGameInstance->Clear_MRT(TEXT("MRT_LumaSharpen"));

			m_bLumaSharpen_Clear = true;
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Final")));

	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::FINAL), m_pShader_Final, "g_FinalTarget")); /* 이전까지 그린 타겟 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_UI_Diffuse"), m_pShader_Final, "g_UI_Target")); /* ui그린거 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Debug"), m_pShader_Final, "g_DebugTarget")); /* DebugCom 그린거 */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_OutLine"), m_pShader_Final, "g_OutLine_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Independent"), m_pShader_Final, "g_Independent_Target")); /* Deferred에서 그린 Independent */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_RB_BlurActive"), m_pShader_Final, "g_RimBlur_Target")); /* Deferred에서 그린 RimBloom */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_OutLine_Blur"), m_pShader_Final, "g_OutLine_Blur_Target")); /* Deferred에서 그린 RimBloom */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_Final, "g_DeferredTarget")); /* Deferred에서 그린 RimBloom */
	
	/* 타겟만들기 아까워서 한가지만 사용가능한채로 여러가지 루트로 팜 */
	if (true == m_tScreenDEffect_Desc.bGrayScale_Active && false == m_tScreenDEffect_Desc.bSephia_Active)
	{
		FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::FINAL_SCREEN_GRAY)));
	}
	else if (false == m_tScreenDEffect_Desc.bGrayScale_Active && true == m_tScreenDEffect_Desc.bSephia_Active)
	{
		_float GreyPower = m_tScreenDEffect_Desc.GreyPower;
		_float SepiaPower = m_tScreenDEffect_Desc.SepiaPower;
	
		FAILED_CHECK(m_pShader_Final->Bind_RawValue("g_GreyPower", &GreyPower, sizeof(_float)));
		FAILED_CHECK(m_pShader_Final->Bind_RawValue("g_SepiaPower", &SepiaPower, sizeof(_float)));
		FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::FINAL_SCREEN_SEPHIA)));
	}
	else if (false == m_tScreenDEffect_Desc.bGrayScale_Active && false == m_tScreenDEffect_Desc.bSephia_Active)
	{
		FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::FINAL)));
	}

	// Test
	//FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::TEST)));

	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());  // Target_Chroma 에 저장 

	return S_OK;
}

HRESULT CRenderer::Render_ShaderResult()
{
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_Final->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::RESULT), m_pShader_Final, "g_ResultTarget")); /* 이전까지 그린 타겟 */

	FAILED_CHECK(m_pShader_Final->Begin(ECast(FINAL_SHADER::RESULT)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
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


HRESULT CRenderer::Deferred_Effect()
{
	/* Effect 추가해서 사용할거면 추가가능합니다~ */

	FAILED_CHECK(Render_Effect_Priority());	/* MRT_Effect_Priority : Target_Effect_Priority_Diffuse, Target_Effect_Priority_Solid, Target_Effect_Priority_RimBloom, Target_Effect_Priority_Distortion 타겟 지정 */

	FAILED_CHECK(Render_Effect());	/* MRT_Effect : Target_Effect_Diffuse, Target_Effect_Normal, Target_Effect_Depth, Target_Effect_RimBloom */
	
	FAILED_CHECK(Render_Effect_BloomBlur());	/* Target_Effect_RR_Blur , Target_Effect_Priority_RR_Blur 에 저장됨 */

	FAILED_CHECK(Render_Effect_Distortion());	/* 기존 Distortion 효과에 Deferred를 합치기 -> Target_Distortion에 저장 */
	//FAILED_CHECK(Render_Effect_Priority_Distortion());	/* 기존 Distortion 효과에 Deferred를 합치기 -> Target_Priority_Distortion에 저장 */

	FAILED_CHECK(Render_Effect_Final()); /* Target_Effect_Combine에 디스토션 넣기  병합 */

	//FAILED_CHECK(Render_Effect_Distortion_Blur());

	//FAILED_CHECK(Render_Effect_Combine()); /* Target_Effect_Combine 에 Distortion제외 모두 병합 */

	m_ePrevTarget = POST_TYPE::DEFERRED;

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Priority()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect_Priority")));

	/* CEffect_Void - CAlphaObject 자식클래스 확인 */
	m_RenderObjects[RENDER_EFFECT_PRIORITY].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return ((CAlphaObject*)pSour)->Get_CamDistance() > ((CAlphaObject*)pDest)->Get_CamDistance();
		});

	for (auto& pGameObject : m_RenderObjects[RENDER_EFFECT_PRIORITY])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_EFFECT_PRIORITY].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Final()
{
	//FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect_Final")));
	//
	//FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	//FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	//FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	//
	///* 기본 Effect */
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Diffuse"), m_pShader_PostProcess, "g_Effect_Target"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Solid"), m_pShader_PostProcess, "g_Effect_Solid"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_RR_Blur"), m_pShader_PostProcess, "g_EffectBlur_Target"));
	//
	///* Priority Effect */
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_Diffuse"), m_pShader_PostProcess, "g_Effect_Priority_Diffuse"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_RR_Blur"), m_pShader_PostProcess, "g_EffectBlur_Priority_Target"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_Solid"), m_pShader_PostProcess, "g_Effect_Priority_Solid"));
	//
	///* Distortion */
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Distortion"), m_pShader_PostProcess, "g_Distortion_Target"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Priority_Distortion"), m_pShader_PostProcess, "g_Priority_Distortion_Target"));
	//
	///* 디퍼드 */
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_PostProcess, "g_Deferred_Target"));
	//
	//FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_EFFECT_MIX)));
	//FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	//FAILED_CHECK(m_pVIBuffer->Render());
	//
	//FAILED_CHECK(m_pGameInstance->End_MRT());
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect_Final")));

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	/* 기본 Effect */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Diffuse"), m_pShader_PostProcess, "g_Effect_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Solid"), m_pShader_PostProcess, "g_Effect_Solid"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_RR_Blur"), m_pShader_PostProcess, "g_EffectBlur_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Distortion"), m_pShader_PostProcess, "g_Distortion_Target"));

	/* Priority Effect */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_Diffuse"), m_pShader_PostProcess, "g_Effect_Priority_Diffuse"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_RR_Blur"), m_pShader_PostProcess, "g_EffectBlur_Priority_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_Solid"), m_pShader_PostProcess, "g_Effect_Priority_Solid"));

	/* Distortion */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_PostProcess, "g_Deferred_Target"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_RB_BlurActive"), m_pShader_Final, "g_RimBlur_Target")); /* Deferred에서 그린 RimBloom */

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_EFFECT_MIX)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Distortion()
{
	//FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion")));
	//
	//FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	//FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	//FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	//
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_PostProcess, "g_Deferred_Target"));
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Distortion"), m_pShader_PostProcess, "g_Effect_DistortionTarget"));
	//
	//FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_EFFECT_DISTORTION)));
	//FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	//FAILED_CHECK(m_pVIBuffer->Render());
	//
	//FAILED_CHECK(m_pGameInstance->End_MRT());
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion")));

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_PostProcess, "g_Deferred_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Distortion"), m_pShader_PostProcess, "g_Effect_DistortionTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Priority_Distortion"), m_pShader_PostProcess, "g_Effect_Priority_DistortionTarget"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_EFFECT_DISTORTION)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Priority_Distortion()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Priority_Distortion")));

	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Deferred"), m_pShader_PostProcess, "g_Deferred_Target"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Effect_Distortion"), m_pShader_PostProcess, "g_Effect_Priority_DistortionTarget"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_EFFECT_DISTORTION)));
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
	for (auto& pGameObject : m_RenderObjects[RENDER_UI_WORLD])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI_WORLD].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_BACK])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI_BACK].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_FRONT])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI_FRONT].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_FIRST])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_UI_FIRST].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_SECOND])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_UI_SECOND].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_THIRD])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_UI_THIRD].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_FOURTH])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_UI_FOURTH].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_UI_POPUP])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_UI_POPUP].clear();

	for (auto& pGameObject : m_RenderObjects[RENDER_CURSOR])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_CURSOR].clear();

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

HRESULT CRenderer::Render_OutLine()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_OutLine")));

	for (auto& pGameObject : m_RenderObjects[RENDER_OUTLINE])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render_OutLine();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_OUTLINE].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT());

	return S_OK;
}

HRESULT CRenderer::Render_OutLine_Blur()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_OutLine_B")));

	for (auto& pGameObject : m_RenderObjects[RENDER_OUTLINE_BLUR])
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Render_OutLine_Blur();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_OUTLINE_BLUR].clear();

	FAILED_CHECK(m_pGameInstance->End_MRT()); /* Target_OutLine_B 에 저장 */

	/* 해당 아웃라인에 블러 먹이기 */
	Render_Blur(TEXT("Target_OutLine_B"), TEXT("MRT_OutLine_Blur"),
		ECast(BLUR_SHADER::BLUR_HORIZON_LOW),
		ECast(BLUR_SHADER::BLUR_VERTICAL_LOW),
		ECast(BLUR_SHADER::BLUR_UP_ADD), true);

	/* Target_OutLine_Blur 에 저장되어나옴 */

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

HRESULT CRenderer::Render_MotionBlur()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur")));

	/* Value */
	_float		CamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_fCamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_PreCamViewMatrix", &m_matPreCameraView));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	/* Target */
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_MotionBlur", &m_tMotionBlur_Desc, sizeof(MOTIONBLUR_DESC)));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_PostProcess, "g_NormalTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_PostProcess, "g_DepthTarget")); 
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::MOTIONBLUR), m_pShader_PostProcess, "g_ProcessingTarget"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_MOTIONBLUR)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());  // Target_Chroma 에 저장 

	return S_OK;
}

HRESULT CRenderer::Render_Godray()
{
	FAILED_CHECK(m_pGameInstance->Begin_MRT(TEXT("MRT_Godray")));

	/* Value */
	_float		CamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_fCamFar", &CamFar, sizeof(_float)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_PreCamViewMatrix", &m_matPreCameraView));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader_PostProcess->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4Inverse(CPipeLine::D3DTS_PROJ)));

	/* Target */
	FAILED_CHECK(m_pSunTextureCom->Bind_ShaderResource(m_pShader_PostProcess, "SunTexture"));
	FAILED_CHECK(m_pShader_PostProcess->Bind_RawValue("g_MotionBlur", &m_tLightShaft_Desc, sizeof(LIGHTSHAFT_DESC)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader_PostProcess, "g_NormalTarget"));
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader_PostProcess, "g_DepthTarget"));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(Current_Target(POST_TYPE::GODRAY), m_pShader_PostProcess, "g_ProcessingTarget"));

	FAILED_CHECK(m_pShader_PostProcess->Begin(ECast(POST_SHADER::POST_GODRAY)));
	FAILED_CHECK(m_pVIBuffer->Bind_VIBuffers());
	FAILED_CHECK(m_pVIBuffer->Render());

	FAILED_CHECK(m_pGameInstance->End_MRT());  // Target_Godray 에 저장 

	return S_OK;
}

HRESULT CRenderer::Render_ShadowBlur()
{
	FAILED_CHECK(Render_Blur(TEXT("Target_ShadowDepth"), TEXT("MRT_Shadow_Blur"),
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

HRESULT CRenderer::Ready_CascadeShadow()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	for (_uint i = 0; i < 3; ++i)
	{
		ID3D11Texture2D* pDepthStencilTexture = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = 1280;
		TextureDesc.Height = 720;
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

wstring CRenderer::Current_Target(POST_TYPE eCurrType)
{
	/* Target_Deferred -> Target_Effect_Final  /  Target_Distortion */
	/* 순서 :DOF, HDF, Radial Blur, FXAA, HSV , Vignette, Chroma, MotionBlur*/

	if (eCurrType == POST_TYPE::FOG)
	{
		strCurrentTarget = TEXT("Target_Effect_Final");
		m_ePrevTarget = POST_TYPE::FOG;
	}
	else 
	{
		switch (m_ePrevTarget)
		{
		case POST_TYPE::FOG:
			strCurrentTarget = TEXT("Target_Fog");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::DEFERRED:
			strCurrentTarget = TEXT("Target_Effect_Final");
			m_ePrevTarget = eCurrType;
			break;
		
		case POST_TYPE::SSR:
			strCurrentTarget = TEXT("Target_SSR");
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

		case POST_TYPE::VIGNETTE:
			strCurrentTarget = TEXT("Target_Vignette");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::CHROMA:
			strCurrentTarget = TEXT("Target_Chroma");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::MOTIONBLUR:
			strCurrentTarget = TEXT("Target_MotionBlur");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::LUMASHARPEN:
			strCurrentTarget = TEXT("Target_LumaSharpen");
			m_ePrevTarget = eCurrType;
			break;

		case POST_TYPE::FINAL:
			strCurrentTarget = TEXT("Target_Final");
			m_ePrevTarget = eCurrType;
			break;

		}
	}
	return strCurrentTarget;
}

HRESULT CRenderer::Check_RenderEffect()
{
	/* OPEN LEVEL 로 인해 레벨바뀌니까 체크 - 그림자용 Light구분용으로 사용  */
	m_iCurrentLevel = m_pGameInstance->Get_NextLevel();

	/* 플레이어 죽었는지 확인 */
	if (true == m_bPlayerDead)
	{
		m_tHSV_Option.bScreen_Active = true;
		m_tHSV_Option.fFinal_Saturation <= 0.f ? m_bPlayerDead = false : m_tHSV_Option.fFinal_Saturation -= 0.01f;

		if (m_iCurrentLevel == 9)
		{
			m_tScreenDEffect_Desc.bGrayScale_Active = false;
			m_tScreenDEffect_Desc.bSephia_Active = false;
			m_bDeadOnce = true;
		}
	}
	else
	{
		if (true == m_bDeadOnce)
		{
			m_tScreenDEffect_Desc.bGrayScale_Active = false;
			m_tScreenDEffect_Desc.bSephia_Active = true;
			m_bDeadOnce = false;
		}


		return S_OK;
	}

	//cout << "Satureation : " <<  m_tHSV_Option.fFinal_Saturation << endl;
	return S_OK;
}

HRESULT CRenderer::Off_Shader()
{
	/* OFF 셰이더 리스트 */
	PBR_DESC			m_tOff_A;
	DEFERRED_DESC		m_tOff_B;
	HBAO_PLUS_DESC		m_tOff_C;
	FOG_DESC			m_tOff_D;
	RADIAL_DESC			m_tOff_E;
	DOF_DESC			m_tOff_F;
	HDR_DESC			m_tOff_G;
	ANTI_DESC			m_tOff_H;
	HSV_DESC			m_tOff_I;
	VIGNETTE_DESC		m_tOff_J;
	SSR_DESC			m_tOff_K;
	CHROMA_DESC			m_tOff_L;
	SCREENEFFECT_DESC	m_tOff_M;
	MOTIONBLUR_DESC		m_tOff_N;
	LUMASHARPEN_DESC	m_tOff_O;

	/* 옵션조절 */
	Set_PBR_Option(m_tOff_A);
	Set_Deferred_Option(m_tOff_B);
	Set_HBAO_Option(m_tOff_C);
	Set_Fog_Option(m_tOff_D);
	Set_RadialBlur_Option(m_tOff_E);
	Set_DOF_Option(m_tOff_F);
	Set_HDR_Option(m_tOff_G);
	Set_FXAA_Option(m_tOff_H);
	Set_HSV_Option(m_tOff_I);
	Set_Vignette_Option(m_tOff_J);
	Set_SSR_Option(m_tOff_K);
	Set_Chroma_Option(m_tOff_L);
	Set_ScreenEffect_Option(m_tOff_M);
	Set_MotionBlur_Option(m_tOff_N);
	Set_LumaSharpen_Option(m_tOff_O);

	return S_OK;
}

HRESULT CRenderer::Clear_RenderTarget(const wstring& RenderTag)
{
	if (RenderTag == TEXT("SHADOW"))
	{
		m_pGameInstance->Clear_MRT(TEXT("MRT_Blur_ViewShadow"));
	}

	return S_OK;
}


void CRenderer::Set_PlayerDeadState(_bool bOption)
{
	m_bPlayerDead = bOption;

	/* 살아나면 원래대로 돌아가야하므로 초기값 저장 */
	m_fBackupData = m_tHSV_Option.fFinal_Saturation;
}

void CRenderer::Set_PlayerRebirthState(_bool bOption)
{
	m_bPlayerDead = !bOption;

	/* 저장했던 원래값으로 되돌아가기*/
	m_tHSV_Option.fFinal_Saturation = m_fBackupData;
}

ID3DBlob* CRenderer::CompileShader(const std::wstring& filename, const string& entrypoint, const string& target)
{
	UINT compileFlags = 0;
#ifdef _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

#endif // _DEBUG

	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors;

	hr = D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(),
		compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	if (FAILED(hr))
		throw exception(); // 예외가 발생하면 프로그램의 흐름을 중단하고 예외처리 루틴으로 제어를 이동시킴

	// hr변수가 실패했을경우 예외발생 -> 예외처리 루틴으로 이동

	return byteCode;
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
HRESULT CRenderer::Add_CascadeObject(_uint iIndex, CGameObject* pObject)
{
	m_CascadeObjects[iIndex].push_back(pObject);
	Safe_AddRef(pObject);

	return S_OK;
}
#pragma endregion

#pragma region Renderer_Initialize

HRESULT CRenderer::Create_Buffer()
{
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

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

HRESULT CRenderer::Create_Texture()
{
	/* PBR  0 :  GamePlay, 1 : IntroBoss, 2 : LEVEL_SNOWMOUNTAIN, 3 : LEVEL_SNOWMOUNTAINBOSS */ 
	m_pIrradianceTextureCom[0] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Intro_Irradiance.dds")); // GamePlay
	m_pIrradianceTextureCom[1] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/IntroBoss_Irradiance.dds")); // IntroBoss
	m_pIrradianceTextureCom[2] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Snow_Irradiance.dds")); // SnowMountain
	m_pIrradianceTextureCom[3] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/SnowBoss_Irradiance.dds")); // SnowMountainBoss
	m_pIrradianceTextureCom[4] = m_pIrradianceTextureCom[3];
	//m_pIrradianceTextureCom[4] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/PlayGround_Irradiance.dds")); // Intro
	NULL_CHECK_RETURN(m_pIrradianceTextureCom, E_FAIL);

	m_pPreFilteredTextureCom[0] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Intro_PreFilteredTexture.dds")); // GamePlay
	m_pPreFilteredTextureCom[1] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/IntroBoss_PreFilteredTexture.dds")); // IntroBoss
	m_pPreFilteredTextureCom[2] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Snow_PreFilteredTexture.dds")); // SnowMountain
	m_pPreFilteredTextureCom[3] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/SnowBoss_PreFilteredTexture.dds")); // SnowMountainBoss
	m_pPreFilteredTextureCom[4] = m_pPreFilteredTextureCom[3];
	//m_pPreFilteredTextureCom[4] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/PlayGround_PreFilteredTexture.dds")); // Intro
	NULL_CHECK_RETURN(m_pPreFilteredTextureCom, E_FAIL);

	m_pBRDFTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/BRDF/BRDFTexture.png"));
	NULL_CHECK_RETURN(m_pBRDFTextureCom, E_FAIL);

	/* Fog */
	m_pVolumetrix_Voxel = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/VolumetricFog/Voxel.png"));
	NULL_CHECK_RETURN(m_pVolumetrix_Voxel, E_FAIL);
	m_pPerlinNoiseTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/T_Perlin_Noise_M.dds"));
	NULL_CHECK_RETURN(m_pPerlinNoiseTextureCom, E_FAIL);

	/* Tool */
	m_pTool_IrradianceTextureCom[0] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Intro_Irradiance.dds")); // GamePlay
	m_pTool_IrradianceTextureCom[1] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/IntroBoss_Irradiance.dds")); // IntroBoss
	m_pTool_IrradianceTextureCom[2] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Snow_Irradiance.dds")); // SnowMountain
	m_pTool_IrradianceTextureCom[3] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/SnowBoss_Irradiance.dds")); // SnowMountainBoss
	m_pTool_IrradianceTextureCom[4] = m_pTool_IrradianceTextureCom[3];
	//m_pTool_IrradianceTextureCom[4] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/PlayGround_Irradiance.dds")); // Intro
	m_pTool_IrradianceTextureCom[5] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Sky_9_Irradiance.dds"));
	m_pTool_IrradianceTextureCom[6] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Sky_10_Irradiance.dds")); 
	m_pTool_IrradianceTextureCom[7] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Sky_11_Irradiance.dds")); 
	m_pTool_IrradianceTextureCom[8] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Sky_12_Irradiance.dds")); 
	m_pTool_IrradianceTextureCom[9] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/Skybox/Sky_13_Irradiance.dds")); 
	
	m_pTool_PreFilteredTextureCom[0] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Intro_PreFilteredTexture.dds")); // GamePlay
	m_pTool_PreFilteredTextureCom[1] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/IntroBoss_PreFilteredTexture.dds")); // IntroBoss
	m_pTool_PreFilteredTextureCom[2] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Snow_PreFilteredTexture.dds")); // SnowMountain
	m_pTool_PreFilteredTextureCom[3] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/SnowBoss_PreFilteredTexture.dds")); // SnowMountainBoss
	m_pTool_PreFilteredTextureCom[4] = m_pTool_PreFilteredTextureCom[3];
	//m_pTool_PreFilteredTextureCom[4] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/PlayGround_PreFilteredTexture.dds")); // Intro
	m_pTool_PreFilteredTextureCom[5] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Sky_9_PreFilteredTexture.dds"));
	m_pTool_PreFilteredTextureCom[6] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Sky_10_PreFilteredTexture.dds"));
	m_pTool_PreFilteredTextureCom[7] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Sky_11_PreFilteredTexture.dds"));
	m_pTool_PreFilteredTextureCom[8] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Sky_12_PreFilteredTexture.dds"));
	m_pTool_PreFilteredTextureCom[9] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/PBR/SkyBox/Sky_13_PreFilteredTexture.dds"));
	
	/* GodRay */
	m_pSunTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Shader/sun.png"));
	NULL_CHECK_RETURN(m_pSunTextureCom, E_FAIL);

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
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RimBloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); 
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); 
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Independent"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); 
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))); /* 카메라에서 본 깊이버퍼 */
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ORM")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_RimBloom"))); 
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))); 
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Independent"))); 

	/* MRT_Bloom_Blur*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RB_BlurActive"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_RB_Blur"), TEXT("Target_RB_BlurActive")));

	/* MRT_LightAcc -> Directional , Spotlight, 등 받아온것 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Ambient"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Ambient")));
	
	/* MRT_Deferred_Shadow - 보류*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Blur"), TEXT("Target_Shadow_Blur")));
	//FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow_Deferred"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f)));
	//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Deferred"), TEXT("Target_Shadow_Deferred")));

	/* MRT_Shadow -> Render_Shadow 로 받아온값 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_ShadowDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_ShadowDepth")));

	/* MRT_Cascade */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade1"),(_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade1"), TEXT("Target_Cascade1")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade2"),(_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade2"), TEXT("Target_Cascade2")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Cascade3"),(_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade3"), TEXT("Target_Cascade3")));

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
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Priority_Distortion"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Priority_Distortion"), TEXT("Target_Priority_Distortion")));
	
	/* ViewShadow + Blur */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_ViewShadow"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_ViewShadow"), TEXT("Target_ViewShadow")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_ViewShadow"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_ViewShadow"), TEXT("Target_Blur_ViewShadow")));

	/* MRT_Distortion_Blur*/
	/* MRT_Distortion_Blur*/
	//FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_DistortionBlur"), TEXT("Target_Distortion_Blur")));

	/* MRT_HBAO+ */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HBAO"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f)))

	/* MRT_Deferred */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Deferred"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Deferred")));
	
	/* MRT_Godray */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Godray"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Godray"), TEXT("Target_Godray")));
	
	/* MRT_Fog */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Fog"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog")));

	/* MRT_HDR */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_HDR"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR")));

	/* MRT_RadialBlur */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_RadialBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_RaidalBlur"), TEXT("Target_RadialBlur")));
	
	/* MRT_MotionBlur */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("Target_MotionBlur")));
	
	/* MRT_LumaSharpen */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_LumaSharpen"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_LumaSharpen"), TEXT("Target_LumaSharpen")));

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
	
	/* MRT_HSV */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Vignette"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Vignette"), TEXT("Target_Vignette")));

	/* MRT_SSR*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSR"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_SSR"), TEXT("Target_SSR")));

	/* MRT_Chroma */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Chroma"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Chroma"), TEXT("Target_Chroma")));
	
	/* MRT_OutLine */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_OutLine"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_OutLine"), TEXT("Target_OutLine")));

	/* MRT_OutLine_Blur*/
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_OutLine_B"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_OutLine_B"), TEXT("Target_OutLine_B")));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_OutLine_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_OutLine_Blur"), TEXT("Target_OutLine_Blur")));

	/* MRT_Final */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final")));
	
	/* MRT_Debug */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Debug"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Debug"), TEXT("Target_Debug")));

	/* MRT_Blend - 보류 */
	//FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blend"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)))
	//FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Blend"), TEXT("Target_Blend")));

	/* MRT_Effect - clear color가 111의 흰색일경우 이펙트에 묻어나와서 무조건 000 으로 클리어해야함  */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Solid"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));	/* 해골 안투명하게 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))); /* 깊이버퍼 그 깊이 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_RimBloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))); /* 넣은거 알아서 블러되도록 처리함 */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Distortion"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); 
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Solid")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Normal")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Depth")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_RimBloom")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Distortion")));

	/* MRT_Effect_Priority  */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Priority_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Priority_Solid"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Priority_RimBloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Priority_Distortion"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))); 
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Priority"), TEXT("Target_Effect_Priority_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Priority"), TEXT("Target_Effect_Priority_Solid")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Priority"), TEXT("Target_Effect_Priority_RimBloom")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Priority"), TEXT("Target_Effect_Priority_Distortion")));
	
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_RR_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Blur"), TEXT("Target_Effect_RR_Blur")));

	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Priority_RR_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Priority_Blur"), TEXT("Target_Effect_Priority_RR_Blur")));

	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Combine"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Combine"), TEXT("Target_Effect_Combine")));

	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Final"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Final"), TEXT("Target_Effect_Final")));

	/* MRT_UI */
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Solid"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_UI"), TEXT("Target_UI_Diffuse")));
	FAILED_CHECK(m_pGameInstance->Add_MRT(TEXT("MRT_UI"), TEXT("Target_UI_Solid")));
	
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

HRESULT CRenderer::GraphicDebug_Shader()
{
	//m_psByteCode = CompileShader(L"../Bin/ShaderFiles/Shader_PostProcessing.hlsl", "PS_MAIN_SSR", "ps_5_0");
	m_psByteCode = CompileShader(L"../Bin/ShaderFiles/Shader_Deferred.hlsl", "PS_MAIN_NEW_PBR", "ps_5_0");

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Change_DebugRenderTarget(TARGET_TYPE type)
{
	_float fSizeX = 150.f;
	_float fSizeY = 100.f;
	_float fBigX = 300.f;
	_float fBigY = 200.f;

	switch (type)
	{
	case Engine::CRenderer::TARGET_TYPE::NONE:
		break;
	case Engine::CRenderer::TARGET_TYPE::ALL:
		{
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ORM"),				(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RimBloom"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Emissive"),			(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 13.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RB_BlurActive"),	(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));
			
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"),			(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Ambient"),			(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));

			/* Render_Shadow */
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ShadowDepth"),		((fSizeX / 2.f * 3)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY)); // Render_Shadow 결과 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Blur_ViewShadow"),	((fSizeX / 2.f * 3)), (fSizeY / 2.f * 7.f), fSizeX, fSizeY)); // 를 디퍼드에서 계산하던거 함 

			/* outline */
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_OutLine"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_OutLine_Blur"),		((fSizeX / 2.f * 3)), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));
			
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Deferred"),			((fSizeX / 2.f * 5)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Final"),		((fSizeX / 2.f * 5)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));

			/* 후처리 Shadow */
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Fog"),				((fSizeX / 2.f * 7)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_DOF"),				((fSizeX / 2.f * 7)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_HDR"),				((fSizeX / 2.f * 9)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RadialBlur"),		((fSizeX / 2.f * 9)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_FXAA"),				((fSizeX / 2.f * 11)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_HSV"),				((fSizeX / 2.f * 11)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Vignette"),			((fSizeX / 2.f * 13)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Chroma"),			((fSizeX / 2.f * 13)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_LumaSharpen"),		((fSizeX / 2.f * 13)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY)); // 

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Priority"),			(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Debug"),			(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Final"),			(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
		}
		break;
	case Engine::CRenderer::TARGET_TYPE::DEFERRED:
		{
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"),			((fSizeX / 2.f * 1.f)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"),			((fSizeX / 2.f * 1.f)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"),			((fSizeX / 2.f * 1.f)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ORM"),				((fSizeX / 2.f * 1.f)), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RimBloom"),			((fSizeX / 2.f * 1.f)), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Emissive"),			((fSizeX / 2.f * 1.f)), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Priority"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Final"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 
		}
		break;
	case Engine::CRenderer::TARGET_TYPE::SHADOW:
		FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ShadowDepth"),			((fBigX / 2.f * 1)), (fBigY / 2.f * 1.f), fBigX, fBigY)); // Render_Shadow 결과 
		FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Blur_ViewShadow"),		((fBigX / 2.f * 1)), (fBigY / 2.f * 3.f), fBigX, fBigY)); // 를 디퍼드에서 계산하던거 함 

		break;
	case Engine::CRenderer::TARGET_TYPE::POSTPROCESSING:
		{
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Fog"),				((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_DOF"),				((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_HDR"),				((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 5.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RadialBlur"),		((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 7.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_FXAA"),				((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 9.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_HSV"),				((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 11.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Vignette"),			((fSizeX / 2.f * 1 )), (fSizeY / 2.f * 13.f), fSizeX, fSizeY)); // 

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Chroma"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_LumaSharpen"),		((fSizeX / 2.f * 3)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 

			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Priority"),			((fSizeX / 2.f * 5 )), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Debug"),			((fSizeX / 2.f * 5 )), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 
			
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Final"),			((fSizeX / 2.f * 7 )), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
			//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Priority"),		(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Debug"),			(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Final"),			(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));

		}
		break;
	case Engine::CRenderer::TARGET_TYPE::EFFECT:
		{
			/* 일반 Effect */
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Diffuse"),					((fSizeX / 2.f * 1)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Solid"),						((fSizeX / 2.f * 1)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Normal"),					((fSizeX / 2.f * 1)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Depth"),						((fSizeX / 2.f * 1)), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_RimBloom"),					((fSizeX / 2.f * 1)), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
																													  
			/* Effect Priority */																					  
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Priority_Diffuse"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Priority_Solid"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Priority_RimBloom"),			((fSizeX / 2.f * 3)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Priority_Distortion"),		((fSizeX / 2.f * 3)), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
																													  
			/* Blur 처리 + Distortion */																				
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_RR_Blur"),					((fSizeX / 2.f * 7)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Priority_RR_Blur"),			((fSizeX / 2.f * 7)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Distortion"),				((fSizeX / 2.f * 7)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
																													  
			/* 최종결과 화면 */																						 
			FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Final"),						((fSizeX / 2.f * 9)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
		}
		break;
	case Engine::CRenderer::TARGET_TYPE::TYPE_END:
		break;
	}

	return S_OK;
}

HRESULT CRenderer::Ready_DebugRender()
{
	/*===============================================*/
	// 화면 좌측 혹은 우측에서 디버그용으로 볼 렌더타겟 추가하는방법  
	// 1. Render_DebugTarget() 함수에서 렌더타겟이 들어간 MRT 그룹 활성화 시키기 
	// 2. 여기 함수에서 해당타겟을 넣고 위치 조정하기 
	// 
	// Tip. fSizeX, Y : 150*100 으로 생각보다 작은 초기 렌더타겟
	// fBigX, Y : 300 * 200 으로 보는 면적자체는 4배로 커진 빅버전
	// 
	// 위치 잡는법 : N행 M열 = (fSizeX / 2.f * M.f), (fSizeY / 2.f * N.f) 로 위치잡을 수 있다. 
	// 1행 1열 위치 = 왼쪽 최상단 
	// 반사이즈 기준이기때문에 1, 3, 5, 7, 9... 이렇게 홀수번으로 숫자가 증가해야한다. 
	// 타겟의 위치별 (M , N) 의 번호 
	// ┌────────────────────────────────┐
	// │(1, 1) (3, 1) (5, 1) ...        │ <--- 모니터
	// │(1, 3) (3, 3) (5, 3) ...        │
	// │(1, 5) (3, 5) (5, 5) ...		│
	// └────────────────────────────────┘
	/*===============================================*/

	_float fSizeX = 150.f;
	_float fSizeY = 100.f;
	_float fBigX = 300.f;
	_float fBigY = 200.f;

	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"),				(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"),				(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Depth"),				(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ORM"),					(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 7.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RimBloom"),				(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Emissive"),				(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 13.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RB_BlurActive"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Independent"),		(fSizeX / 2.f * 1.f), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));

	/*Light*/
	//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade"),				(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"),				(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Ambient"),				(fSizeX / 2.f * 3.f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));

	/* Render_Shadow */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_ShadowDepth"), ((fSizeX / 2.f * 3)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY)); // Render_Shadow 결과 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Blur_ViewShadow"), ((fSizeX / 2.f * 3)), (fSizeY / 2.f * 7.f), fSizeX, fSizeY)); // 를 디퍼드에서 계산하던거 함 

	/* outline */
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_OutLine"), ((fSizeX / 2.f * 3)), (fSizeY / 2.f * 9.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_OutLine_Blur"), ((fSizeX / 2.f * 3)), (fSizeY / 2.f * 11.f), fSizeX, fSizeY));

	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Deferred"), ((fSizeX / 2.f * 5)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Final"), ((fSizeX / 2.f * 5)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));

	
	/* 후처리 Shadow */ 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Fog"),					((fSizeX / 2.f * 7)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_DOF"),					((fSizeX / 2.f * 7)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 

	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_HDR"),					((fSizeX / 2.f * 9)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_RadialBlur"),			((fSizeX / 2.f * 9)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 

	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_FXAA"),					((fSizeX / 2.f * 11)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_HSV"),					((fSizeX / 2.f * 11)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 
	
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Vignette"),				((fSizeX / 2.f * 13)), (fSizeY / 2.f * 1.f), fSizeX, fSizeY)); // 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Chroma"),				((fSizeX / 2.f * 13)), (fSizeY / 2.f * 3.f), fSizeX, fSizeY)); // 
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_LumaSharpen"),			((fSizeX / 2.f * 13)), (fSizeY / 2.f * 5.f), fSizeX, fSizeY)); // 

	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Priority"),				(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Debug"),				(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 3.f), fSizeX, fSizeY));
	FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Final"),				(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 5.f), fSizeX, fSizeY));

	/* !유정 : Effect관련 RenderTarget */
	{	
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Diffuse"),			((fBigX / 2.f * 1)), (fBigY / 2.f * 3.f), fBigX, fBigY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Solid"),			((fBigX / 2.f * 1)), (fBigY / 2.f * 3.f), fBigX, fBigY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Normal"),			((fBigX / 2.f * 1)), (fBigY / 2.f * 5.f), fBigX, fBigY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Depth"),			((fBigX / 2.f * 1)), (fBigY / 2.f * 5.f), fBigX, fBigY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_RR_Blur"),			((fBigX / 2.f * 3)), (fBigY / 2.f * 1.f), fBigX, fBigY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_RimBloom"),		((fBigX / 2.f * 3)), (fBigY / 2.f * 3.f), fBigX, fBigY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Effect_Distortion"),		((fBigX / 2.f * 3)), (fBigY / 2.f * 5.f), fBigX, fBigY));

	}

	/* !성희 : UI관련 RenderTarget */
	{
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_UI_Diffuse"),			(g_iWinsizeX - fSizeX * 0.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
		//FAILED_CHECK(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_UI_Tool"),			(fSizeX / 2.f * 24.5f), (fSizeY / 2.f * 1.f), fSizeX, fSizeY));
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
	/*===============================================*/
	// 아래의 목록에서 자기가 렌더타겟으로 볼 타겟이 
	// Add되어있는 MRT만 활성화 시키기 
	// 디버그에서 프레임관리하기위함임
	// 찾는법 : Create_RenderTarget함수에서 찾을 타겟 검색하면 어느 MRT에 속한타겟인지 알 수 있음. 
	/*===============================================*/
	m_pShader_Deferred->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader_Deferred->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Debug"),				m_pShader_Deferred, m_pVIBuffer);

	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Priority"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_GameObjects"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_RB_Blur"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_LightAcc"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Deferred"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Shadow_Blur"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Blur_ViewShadow"),		m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_OutLine"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_OutLine_B"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_OutLine_Blur"),		m_pShader_Deferred, m_pVIBuffer);

	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Cascade1"),			m_pShader_Deferred, m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Cascade2"),			m_pShader_Deferred, m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Cascade3"),			m_pShader_Deferred, m_pVIBuffer);

	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_SSR"),				m_pShader_Deferred, m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_LightShaft"),		m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Fog"),					m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_HDR"),					m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_RaidalBlur"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_DOF"),					m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_DOFBlur"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_FXAA"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_HSV"),					m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Vignette"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Chroma"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_LumaSharpen"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Final"),				m_pShader_Deferred, m_pVIBuffer);
	
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect"),				m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect_Blur"),			m_pShader_Deferred, m_pVIBuffer);

	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect_Priority"),		m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect_Priority_Blur"),m_pShader_Deferred, m_pVIBuffer);
	 
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Distortion"),			m_pShader_Deferred, m_pVIBuffer);
	m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_Effect_Final"),		m_pShader_Deferred, m_pVIBuffer);

	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_UI"),				m_pShader_Deferred, m_pVIBuffer);
	//m_pGameInstance->Render_Debug_RTVs(TEXT("MRT_UI_Tool"),			m_pShader_Deferred, m_pVIBuffer);
	
		
	return S_OK;
}

HRESULT CRenderer::Control_HotKey()
{
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

	Safe_Release(m_pBRDFTextureCom);
	Safe_Release(m_pVolumetrix_Voxel);
	Safe_Release(m_pSunTextureCom);

	for (auto& pPrefILTERED : m_pPreFilteredTextureCom)
		Safe_Release(pPrefILTERED);

	for (auto& pIrrad : m_pIrradianceTextureCom)
		Safe_Release(pIrrad);

	Safe_Release(m_psByteCode);
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
