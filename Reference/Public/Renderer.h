#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP
	{   /* Priority */
		RENDER_PRIORITY, RENDER_NONLIGHT, RENDER_OUTLINE,
		/* Post Processing  */
		RENDER_NONBLEND, RENDER_SHADOW, RENDER_DECAL,
		/* EFFECT */
		RENDER_EFFECT, RENDER_EFFECT_PRIORITY, RENDER_EFFECT_MESH,
		/* UI */
		RENDER_UI_BACK, RENDER_UI, RENDER_UI_FRONT, RENDER_UI_FIRST, RENDER_UI_SECOND, RENDER_UI_THIRD, RENDER_UI_FOURTH, RENDER_UI_POPUP, RENDER_CURSOR, RENDER_UI_WORLD,
		/* RenderGroup*/
		RENDER_BLEND, RENDER_CASCADE, RENDER_OUTLINE_BLUR, RENDER_ROLLING, RENDER_END
	};

	enum class POST_TYPE { DEFERRED, FOG, GODRAY, SSR, DOF, HDR, RADIAL_BLUR, FXAA, HSV, VIGNETTE, CHROMA, 
						   MOTIONBLUR, LUMASHARPEN, FINAL, TYPE_END};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Create_Buffer();
	HRESULT	Create_Shader();
	HRESULT	Create_Texture();
	HRESULT Create_RenderTarget();
	HRESULT Create_DepthStencil();
	HRESULT GraphicDebug_Shader();

	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, class CGameObject* pGameObject);
	HRESULT Add_DebugRender(class CComponent* pDebugCom);
	HRESULT Add_CascadeObject(_uint iIndex, CGameObject* pObject);
	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Test();

	HRESULT Render_Priority();
	HRESULT Render_NonLight();
	HRESULT Render_Fog();

	HRESULT Render_NonBlend();
	HRESULT Render_ShadowDepth();
	HRESULT Render_Cascade();
	HRESULT Render_LightAcc();
	HRESULT Bake_ViewShadow();
	HRESULT Render_HBAO_PLUS();
	HRESULT Render_RimBloom();
	HRESULT Render_Deferred();
	HRESULT Render_MyPBR();
	HRESULT Render_PBR();
	HRESULT Render_SSR();
	HRESULT Render_Chroma();			
	HRESULT Render_LumaSharpen();

	HRESULT Deferred_Effect();
	HRESULT Render_Effect_Priority();
	HRESULT Render_Effect_BloomBlur();
	HRESULT Render_Effect_Distortion();
	HRESULT Render_Effect_Priority_Distortion();
	HRESULT Render_Effect_Distortion_Blur();
	HRESULT Render_Effect_Final();

	HRESULT Render_OutLine();
	HRESULT Render_OutLine_Blur();
	HRESULT Render_RadialBlur(); 
	HRESULT Render_HDR();
	HRESULT Render_DOF(); // 일반적으로 HDR적용이후에 적용됨 
	HRESULT Render_FXAA();
	HRESULT Render_HSV();
	HRESULT Render_Vignette(); 
	HRESULT Render_Final();
	HRESULT Render_Blend();
	HRESULT Render_MotionBlur();
	HRESULT Render_Godray();

	HRESULT Deferred_UI();

	/* Effect */
	HRESULT Render_Effect();

	/* UI */
	HRESULT Render_UI();
	HRESULT Render_UI_Tool();

	/* ETC */
	HRESULT Render_Blur(const wstring& strStartTargetTag, const wstring& strFinalMRTTag, _int eHorizontalPass, _int eVerticalPass, _int eBlendType, _bool bClear);
	HRESULT	Render_Blur_DownSample(const wstring& strStartTargetTag);
	HRESULT	Render_Blur_Horizontal(_int eHorizontalPass);
	HRESULT	Render_Blur_Vertical(_int eVerticalPass);
	HRESULT	Render_Blur_UpSample(const wstring& strFinalMrtTag, _bool bClear, _int eBlendType);

	HRESULT Render_ShadowBlur();
	HRESULT Render_Alphablend(const wstring& Begin_MRT_Tag, const wstring& Blend_Target_Tag);
	HRESULT Ready_CascadeShadow();
	wstring Current_Target(POST_TYPE eType);
	
	HRESULT Check_RenderEffect();

public:
	/* 렌더옵션 초기화 */
	HRESULT Off_Shader(); /* 모든 셰이더옵션 다 끔 */
	HRESULT Clear_RenderTarget(const wstring& RenderTag);

	/* 활성화 */
	void Set_PBR_Active(_bool _Pbr_active) { m_tPBR_Option.bPBR_ACTIVE = _Pbr_active; }
	void Set_BloomBlur_Active(_bool _bloom_active) { m_tDeferred_Option.bRimBloom_Blur_Active = _bloom_active; }
	_bool Get_BloomBlur_Active() { return m_tDeferred_Option.bRimBloom_Blur_Active; }
	void Set_Shadow_Active(_bool _Shadow_Active) { m_tDeferred_Option.bShadow_Active = _Shadow_Active; }
	_bool Get_Shadow_Active() { return m_tDeferred_Option.bShadow_Active; }
	void Set_HBAO_Active(_bool _HBAO) { m_tHBAO_Option.bHBAO_Active = _HBAO; }
	_bool Get_HBAO_Active() { return m_tHBAO_Option.bHBAO_Active; }
	void Set_Fog_Active(_bool _Fog) { m_tFog_Option.bFog_Active = _Fog; }
	void Set_SSR_Active(_bool _SSR) { m_tSSR_Option.bSSR_Active = _SSR; }
	void Set_MotionBlur_Active(_bool _SSR) { m_tMotionBlur_Desc.bMotionBLur_Active = _SSR; }

	void Set_Radial_Blur_Active(_bool _Radial) { m_tRadial_Option.bRadial_Active = _Radial; }
	void Set_Chroma_Active(_bool _Chroma) { m_tChroma_Option.bChroma_Active = _Chroma; }
	void Set_DOF_Active(_bool _DOF) { m_tDOF_Option.bDOF_Active = _DOF; }
	_bool Get_DOF_Active() { return m_tDOF_Option.bDOF_Active; }
	void Set_HDR_Active(_bool _HDR_active) { m_tHDR_Option.bHDR_Active = _HDR_active; }
	_bool Get_HDR_Active() { return m_tHDR_Option.bHDR_Active; }
	void Set_FXAA_Active(_bool _FXAA_active) { m_tAnti_Option.bFXAA_Active = _FXAA_active; }
	_bool Get_FXAA_Active() { return m_tAnti_Option.bFXAA_Active; }
	void Set_HSV_Active(_bool _HSV_active) { m_tHSV_Option.bScreen_Active = _HSV_active; }
	void Set_Vignette_Active(_bool _Vignette_active) { m_tVignette_Option.bVignette_Active = _Vignette_active; }
	void Set_Gray_Active(_bool _Gray_active) { m_tScreenDEffect_Desc.bGrayScale_Active = _Gray_active; }
	void Set_Sephia_Active(_bool _Sephia_active) { m_tScreenDEffect_Desc.bSephia_Active = _Sephia_active; }
	void Set_Luma_Active(_bool _luma_active) { m_tLumaSharpen_Desc.bLumaSharpen_Active = _luma_active; }

	//Temp
	_float Get_MotionBlur_float() { return m_tMotionBlur_Desc.fMotionBlur_Intensity; }
	void   Set_MotionBLur_float(_float fValue) {m_tMotionBlur_Desc.fMotionBlur_Intensity = fValue; }

	/* 옵션조절 */
	void Set_PBR_Option(PBR_DESC desc) { m_tPBR_Option = desc; }
	void Set_Deferred_Option(DEFERRED_DESC desc) { m_tDeferred_Option = desc; }
	void Set_HBAO_Option(HBAO_PLUS_DESC desc) { m_tHBAO_Option = desc; }
	void Set_Fog_Option(FOG_DESC desc) { m_tFog_Option = desc; }
	void Set_SSR_Option(SSR_DESC desc) { m_tSSR_Option = desc; }
	void Set_Chroma_Option(CHROMA_DESC desc) { m_tChroma_Option = desc; }

	void Set_RadialBlur_Option(RADIAL_DESC desc) { m_tRadial_Option = desc; }
	void Set_DOF_Option(DOF_DESC desc) { m_tDOF_Option = desc; }
	void Set_HDR_Option(HDR_DESC desc) { m_tHDR_Option = desc; }
	void Set_FXAA_Option(ANTI_DESC desc) { m_tAnti_Option = desc; }
	void Set_HSV_Option(HSV_DESC desc) { m_tHSV_Option = desc; }
	void Set_Vignette_Option(VIGNETTE_DESC desc) { m_tVignette_Option = desc; }
	void Set_ScreenEffect_Option(SCREENEFFECT_DESC desc) { m_tScreenDEffect_Desc = desc; }
	void Set_MotionBlur_Option(MOTIONBLUR_DESC desc) { m_tMotionBlur_Desc = desc; }
	void Set_LumaSharpen_Option(LUMASHARPEN_DESC desc) { m_tLumaSharpen_Desc = desc; }

public:
	void Set_PlayerDeadState(_bool bOption);
	void Set_PlayerRebirthState(_bool bOption);
//	void Set_PlayerDead(_bool bOption);

private: // Debug
	ID3DBlob* CompileShader(const std::wstring& filename, const string& entrypoint, const string& target);

private:
	_bool						m_bInit						= { true }; /* 없으면 터짐 건들지마세요 */
	_bool						bTest						= { true };
	_bool						m_bPlayerDead				= { false };
	_float4x4					m_matPreCameraView = {};

	PBR_DESC					m_tPBR_Option				= {};
	DEFERRED_DESC				m_tDeferred_Option			= {};
	HBAO_PLUS_DESC				m_tHBAO_Option				= {};
	FOG_DESC					m_tFog_Option				= {};
	RADIAL_DESC					m_tRadial_Option			= {};
	DOF_DESC					m_tDOF_Option				= {};
	HDR_DESC					m_tHDR_Option				= {};
	ANTI_DESC					m_tAnti_Option				= {};
	HSV_DESC					m_tHSV_Option				= {};
	VIGNETTE_DESC				m_tVignette_Option			= {};
	SSR_DESC					m_tSSR_Option				= {};
	CHROMA_DESC					m_tChroma_Option			= {};
	SCREENEFFECT_DESC			m_tScreenDEffect_Desc		= {};
	LUMASHARPEN_DESC			m_tLumaSharpen_Desc			= {};

	MOTIONBLUR_DESC				m_tMotionBlur_Desc			= {};
	LIGHTSHAFT_DESC				m_tLightShaft_Desc			= {};

private:
	POST_TYPE					m_ePrevTarget				= POST_TYPE::FINAL;
	_int						m_iCurrentLevel				= {};
	wstring						strCurrentTarget			= TEXT("Target_Effect_Final");
	_float						m_fBackupData				= {};

	_bool						m_bBloomBlur_Clear			= false;
	_bool						m_bRimBlur_Clear			= false;
	_bool						m_bShadow_Clear				= false;
	_bool 						m_bHBAO_Clear				= false;
	_bool 						m_bPBR_Clear				= false;
	_bool 						m_bFOG_Clear				= false;
	_bool 						m_bDOF_Clear				= false;
	_bool 						m_bHDR_Clear				= false;
	_bool 						m_bRadialBlur_Clear			= false;
	_bool 						m_bFxaa_Clear				= false;
	_bool						m_bHSV_Clear = false;
	_bool						m_bVignette_Clear = false;

	_bool 						m_Chroma_Clear				= false;
	_bool 						m_bLumaSharpen_Clear		= false;

private:
	ID3DBlob*					m_psByteCode				= { nullptr };
	ID3D11Device*				m_pDevice					= { nullptr };
	ID3D11DeviceContext*		m_pContext					= { nullptr };
	class CGameInstance*		m_pGameInstance				= { nullptr };
	list<class CGameObject*>	m_RenderObjects[RENDER_END];

private:
	class CShader*				m_pShader_Deferred			= { nullptr };
	class CShader*				m_pShader_PostProcess		= { nullptr };
	class CShader*				m_pShader_Blur				= { nullptr };
	class CShader*				m_pShader_Fxaa				= { nullptr };
	class CShader*				m_pShader_Final				= { nullptr };
	class CShader*				m_pShader_UI				= { nullptr };
	class CShader*				m_pShader_Effect			= { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer					= { nullptr };

	class CTexture*				m_pPerlinNoiseTextureCom	= { nullptr };
	class CTexture*				m_pIrradianceTextureCom[4]	= { nullptr };
	class CTexture*				m_pPreFilteredTextureCom[4]	= { nullptr };
	class CTexture*				m_pBRDFTextureCom			= { nullptr };
	class CTexture*				m_pVolumetrix_Voxel			= { nullptr };
	class CTexture*				m_pSunTextureCom = { nullptr };
	ID3D11DepthStencilView*		m_pLightDepthDSV			= { nullptr };
	_float4x4					m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	HRESULT						Control_HotKey();

	vector<class CGameObject*>	m_CascadeObjects[3]			= {};
	ID3D11DepthStencilView*		m_pCascadeShadowDSV[3]		= {};

	/* For. Tool */
	class CTexture* m_pTool_IrradianceTextureCom[10] = { nullptr };
	class CTexture* m_pTool_2_IrradianceTextureCom[10] = { nullptr };
	class CTexture* m_pTool_PreFilteredTextureCom[10] = { nullptr };
	class CTexture* m_pTool_2_PreFilteredTextureCom[10] = { nullptr };

public:
	_bool			m_bUI_MRT						= { false };	
	void			Render_UI_MRT(_bool bMRT) { m_bUI_MRT = bMRT;}
	void			Set_ToolPBRTexture_InsteadLevel(_int iPBRTexture) { m_iPBRTexture_InsteadLevel = iPBRTexture; }
	_bool			m_bToolLevel					= { false };
	_int			m_iPBRTexture_InsteadLevel		= { 0 };

#ifdef _DEBUG
public:
	void			Set_DebugRenderTarget(_bool _bDebug) { m_bDebugRenderTarget = _bDebug; }
	void			Set_DebugCom(_bool _bDebug) { m_bDebugCom = _bDebug; }

private:
	HRESULT			Ready_DebugRender();
	HRESULT			Render_DebugCom();	
	HRESULT			Render_DebugTarget();
	_bool			m_bDebugRenderTarget	= { false };
	_bool			m_bDebugCom				= { false };
	list<class CComponent*>			m_DebugComponent;
#endif	

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END