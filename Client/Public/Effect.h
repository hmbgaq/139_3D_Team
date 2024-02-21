#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Client)

class CEffect : public CAlphaObject
{
public:
	enum TEXTURE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_NOISE, TEXTURE_END };

	typedef struct tagEffectDesc
	{
		_float		fDiscardValue = { 0.8f };

		wstring		strTextureTag[TEXTURE_END];
		_int		iTextureIndex[TEXTURE_END] = { 0 };

		wstring		strShaderTag = { TEXT("") };
		_uint		iShaderPassIndex = { 0 };

		_int		iRenderGroup = { 7 };	//! 밖에서 렌더러의 렌더그룹을 인트로 형변환해서 던져주자 현재 작성기준 CRENDERER::RENDERGROUP::RENDER_END가 8임

		_float3	vPosition_Start = _float3(0.f, 0.f, 0.f);
		_float3	vPosition_End = _float3(0.f, 0.f, 0.f);
		_bool	bPosition_Lerp = true;

		_float3	vRotation_Start = _float3(0.f, 0.f, 0.f);
		_float3	vRotation_End = _float3(0.f, 0.f, 0.f);
		_bool	bRotation_Lerp = true;

		_float3	vScaling_Start = _float3(1.f, 1.f, 1.f);
		_float3	vScaling_End = _float3(1.f, 1.f, 1.f);
		_bool	bScaling_Lerp = true;

		_float3	vVelocity_Start = _float3(0.f, 0.f, 0.f);
		_float3	vVelocity_End = _float3(0.f, 0.f, 0.f);
		_bool	bVelocity_Lerp = true;

		_float4	vColor_Start = _float4(0.f, 0.f, 0.f, 0.f);
		_float4	vColor_End = _float4(1.f, 1.f, 1.f, 1.f);
		_bool	bColor_Lerp = true;

		_float	fLifeTime = 3.f;
		_float	fWaitingTime = 0.0f;
		_float	fRemainTime = 0.0f;
		_bool	bParentPivot = false;

		_float2	vUV_Start = _float2(0.f, 0.f);
		_float2	vUV_Speed = _float2(0.f, 0.f);

		_bool	IsSequence = false;
		_bool	IsLoop = false;
		_float	fSequenceTerm = 0.05f;
		_float	fDissolveStart = 1.f;

		// Variables
		_float2	vUV_Offset = _float2(0.f, 0.f);
		_int	iUV_Wave = false;
		_float	fUV_WaveSpeed = 1.f;
		_float2	vUV_TileCount = _float2(1.f, 1.f);
		_float2	vUV_TileIndex = _float2(0.f, 0.f);
		_float4	vColor_Offset = _float4(1.f, 1.f, 1.f, 1.f);
		_float4	vColor_Clip = _float4(0.f, 0.f, 0.f, 0.f);
		_float4	vColor_Mul = _float4(1.f, 1.f, 1.f, 1.f);

		// Intensity
		_float	fBloom = 0.f;
		_float	fRadial = 0.f;
		_float	fDissolveAmount = 0.f;

		// Billboard
		_bool	bBillboard = { true };

		/* Particle */
		_float3	vEmitPosition = _float3(0.f, 0.f, 0.f);
		_float3	vEmitDirection = _float3(0.f, 0.f, 0.f);
		_float3	vRandomMul = _float3(1.f, 1.f, 1.f);
		_float	fSpreadSpeed = 0.f;
		_float	fEmitTerm = 0.f;
		_float	fParticleLifeTime = 0.f;

	}EFFECT_DESC;

	struct tagFX_Variables
	{
		_float2	vUV_Offset		= _float2(0.f, 0.f);
		_int	iUV_Wave		= false;
		_float	fUV_WaveSpeed	= 1.f;
		_float2	vUV_TileCount	= _float2(1.f, 1.f);
		_float2	vUV_TileIndex	= _float2(0.f, 0.f);
		_float4	vColor_Offset	= _float4(1.f, 1.f, 1.f, 1.f);
		_float4	vColor_Clip		= _float4(0.01f, 0.01f, 0.01f, 0.01f);
		_float4	vColor_Mul		= _float4(1.f, 1.f, 1.f, 1.f);
	} m_tVariables;

	struct tagFX_Intensity
	{
		_float	fBloom = 0.f;
		_float	fRadial = 0.f;
		_float	fDissolveAmount = 0.f;
		_float	padding;
	} m_tIntensity;


protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

protected:
	virtual	HRESULT Bind_ShaderResources();


public:
	void	Set_Color_Offset(_float fRed, _float fGreen, _float fBlue, _float fAlpha) { m_tVariables.vColor_Offset = _float4(fRed, fGreen, fBlue, fAlpha); }


protected:
	//EFFECT_DESC m_tEffect = {};

protected:
	_float4x4	m_matPivot		= {}; /*XMStoreFloat4x4(&m_matPivot, XMMatrixIdentity())*/
	_float4x4	m_matOffset		= {};
	_float4x4	m_matCombined	= {};

	_float	m_fTimeAcc			= { 0.f };
	_float	m_fWaitingAcc		= { 0.0f };
	_float	m_fLifeTimeRatio	= { 0.f };

	_float	m_fSequenceTimer	= { 0.0f };
	_float	m_fDissolveStart	= { 1.f };


public:
	/* 원형객체를 생성한다. */
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END