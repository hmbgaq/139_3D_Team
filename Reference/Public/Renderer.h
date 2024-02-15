#pragma once

#include "Base.h"

/* 화면에 그려져야할 객체들을 그려야할 순서대로 모아놓는다. */
/* 모아놓은 순서대로 객체들의 렌더함수를 호출한다.(렌더콜) */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONLIGHT, 
					   
						/* Shader */
					   RENDER_GODRAY, RENDER_OUTLINE,
					   
					   RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };

	enum SHADER_TYPE { SHADER_DEFERRED, SHADER_POSTPROCESSING, SHADER_BLUR, SHADER_FINAL, SHADER_END };
	
	struct QuadVertex // ssao 
	{
		_float3 pos;
		_float3 ToFarPlaneIndex;
		_float2 tex;
	};
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, class CGameObject* pGameObject);
	HRESULT Add_DebugRender(class CComponent* pDebugCom);
	HRESULT Draw_RenderGroup();
	HRESULT Draw_RenderName();
	
	/* Ready */
	HRESULT Create_Buffer();
	HRESULT	Create_Shader();
	HRESULT Create_RenderTarget();
	HRESULT Create_DepthStencil();
	HRESULT Ready_DebugRender();
	HRESULT Ready_SSAO();

	/* SSAO */
	HRESULT Initialize_ScreenQuad();
	void	BuildFrustumFarCorners();
	void	BuildOffsetVectors();

	HRESULT RenderScreenQuad();

#ifdef _DEBUG
public:
	void Set_RenderDebug(_bool _bRenderDebug) {	m_bRenderDebug = _bRenderDebug;	}
#endif

private:
	class CShader*				m_pShader[SHADER_TYPE::SHADER_END] = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11DepthStencilView*		m_pLightDepthDSV = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RENDER_END];

#ifdef _DEBUG
	list<class CComponent*>		m_DebugComponent;
	_bool						m_bRenderDebug = { false };
#endif

private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix, m_ProjMatrix;
	_float4						m_vLineColor = { 1.f, 1.f, 1.f, 1.f };

private: // SSAO
	class CTexture* m_pRandomVectorTexture = { nullptr };
	ID3D11Buffer*	m_pQuadVertexBuffer = { nullptr };
	ID3D11Buffer*	m_pQuadIndexBuffer = { nullptr };

	_float4			m_vFrustumFarCorner[4];
	_float4			m_vOffsets[26];
	_int			m_iQuadVerCount;
	_int			m_iQuadIndexCount;

	HRESULT			Render_Blur_DownSample();
	HRESULT			Render_Blur_Horizontal();
	HRESULT			Render_Blur_Vertical();
	HRESULT			Render_Blur_UpSample();

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonLight();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();

	HRESULT Render_OutLine();
	HRESULT Render_SSAO();
	HRESULT Render_SSAO_Blur();
	HRESULT Render_GodRay();


#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END