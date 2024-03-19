#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP
	{	/* Priority */
		RENDER_PRIORITY, RENDER_NONLIGHT,
		/* Post Processing  */
		RENDER_NONBLEND, RENDER_SHADOW, RENDER_DECAL, RENDER_OUTLINE,
		/* EFFECT */
		RENDER_EFFECT, RENDER_EFFECT_PARTICLE, RENDER_EFFECT_MESH,
		/* UI */
		RENDER_UI, RENDER_CURSOR,
		/* RenderGroup*/
		RENDER_BLEND, RENDER_END
	};
	enum class POST_TYPE { DEFERRED, RADIAL_BLUR, HDR, DOF, FXAA, HSV, FINAL, TYPE_END};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Create_Buffer();
	HRESULT	Create_Shader();
	HRESULT Create_RenderTarget();
	HRESULT Create_DepthStencil();

	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, class CGameObject* pGameObject);
	HRESULT Add_DebugRender(class CComponent* pDebugCom);
	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonLight();

	HRESULT Render_NonBlend();
	HRESULT Render_Shadow();
	HRESULT Render_LightAcc();
	HRESULT Render_HBAO_PLUS();
	HRESULT Render_RimBloom();
	HRESULT Render_Deferred();

	HRESULT Deferred_Effect();
	HRESULT Render_Effect_BloomBlur();
	HRESULT Render_Effect_Distortion();
	HRESULT Render_Effect_Final();

	HRESULT Render_OutLine();
	HRESULT Render_RadialBlur(); 
	HRESULT Render_HDR();
	HRESULT Render_DOF(); // 일반적으로 HDR적용이후에 적용됨 
	HRESULT Render_FXAA();
	HRESULT Render_HSV();
	HRESULT Render_Final();
	HRESULT Render_Blend();

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

	HRESULT Deferred_Shadow();
	HRESULT Render_ShadowBlur();
	HRESULT Render_Alphablend(const wstring& Begin_MRT_Tag, const wstring& Blend_Target_Tag);

	wstring Current_Target(POST_TYPE eType);
	
public:
	/* 활성화 */
	void Set_HBAO_Active(_bool _HBAO) { m_tHBAO_Option.bHBAO_Active = _HBAO; }
	void Set_BloomBlur_Active(_bool _bloom_active) { m_tBloomRim_Option.bRimBloom_Blur_Active = _bloom_active; }
	void Set_Fog_Active(_bool _Fog) { m_tFog_Option.bFog_Active = _Fog; }

	void Set_Radial_Blur_Active(_bool _Radial) { m_tRadial_Option.bRadial_Active = _Radial; }
	void Set_DOF_Active(_bool _DOF) { m_tDOF_Option.bDOF_Active = _DOF; }
	void Set_HDR_Active(_bool _HDR_active) { m_tHDR_Option.bHDR_Active = _HDR_active; }
	void Set_FXAA_Active(_bool _FXAA_active) { m_tAnti_Option.bFXAA_Active = _FXAA_active; }
	void Set_HSV_Active(_bool _HSV_active) { m_tHSV_Option.bScreen_Active = _HSV_active; }

	/* 옵션조절 */
	void Set_HBAO_Option(HBAO_PLUS_DESC desc) { m_tHBAO_Option = desc; }
	void Set_BloomRim_Option(BLOOMRIM_DESC desc) { m_tBloomRim_Option = desc; }
	void Set_Fog_Option(FOG_DESC desc) { m_tFog_Option = desc; }

	void Set_RadialBlur_Option(RADIAL_DESC desc) { m_tRadial_Option = desc; }
	void Set_HDR_Option(HDR_DESC desc) { m_tHDR_Option = desc; }
	void Set_HSV_Option(HSV_DESC desc) { m_tHSV_Option = desc; }
	void Set_FXAA_Option(ANTI_DESC desc) { m_tAnti_Option = desc; }
	void Set_DOF_Option(DOF_DESC desc) { m_tDOF_Option = desc; }

private:
	_bool						m_bInit						= { true }; /* 없으면 터짐 건들지마세요 */

	HBAO_PLUS_DESC				m_tHBAO_Option				= {};
	BLOOMRIM_DESC				m_tBloomRim_Option			= {};
	FOG_DESC					m_tFog_Option				= {};

	RADIAL_DESC					m_tRadial_Option			= {};
	DOF_DESC					m_tDOF_Option				= {};
	HDR_DESC					m_tHDR_Option				= {};
	HSV_DESC					m_tHSV_Option				= {};
	ANTI_DESC					m_tAnti_Option				= {};

private:
	POST_TYPE					m_ePrevTarget				= POST_TYPE::FINAL;
	_bool						m_bBloomBlur_Clear			= false;
	_bool						m_bRimBlur_Clear			= false;
	_int						m_iCurrentLevel				= {};
	wstring						strCurrentTarget			= TEXT("Target_Effect_Final");

private:
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
	ID3D11DepthStencilView*		m_pLightDepthDSV			= { nullptr };
	_float4x4					m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	HRESULT						Control_HotKey();

public:
	_bool			m_bUI_MRT = false;
	void			Render_UI_MRT(_bool bMRT) { m_bUI_MRT = bMRT;}

#ifdef _DEBUG
public:
	void	Set_DebugRenderTarget(_bool _bDebug) { m_bDebugRenderTarget = _bDebug; }
	void	Set_DebugCom(_bool _bDebug) { m_bDebugCom = _bDebug; }

private:
	HRESULT Ready_DebugRender();
	HRESULT Render_DebugCom();	
	HRESULT Render_DebugTarget();
	_bool	m_bDebugRenderTarget	= { false };
	_bool	m_bDebugCom				= { true };
	list<class CComponent*>			m_DebugComponent;
#endif	

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END