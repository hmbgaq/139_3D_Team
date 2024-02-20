#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Texture final : public CEffect
{
public:
	enum TYPE { SINGLE, SPRITE, TYPE_END };

	typedef struct tagTextureEffectDesc : public CEffect::EFFECT_DESC
	{
		TYPE		eType = { SINGLE };

	}EFFECT_TEXTURE_DESC;

	typedef struct tagSpriteUV
	{
		_bool	bPlay = { true };

		_float	fIntervalTime = { 0.05f };

		_float	fAnimationSizeX = { 256.f };
		_float	fAnimationSizeY = { 256.f };

		_float	fSpriteSizeX = { 1792.f };
		_float	fSpriteSizeY = { 1792.f };

		_int	iCurrentVer = { 0 };
		_int	iCurrentHor = { 0 };

		_int	iMinVer = { 0 };
		_int	iMinHor = { 0 };

		_int	iMaxVer = { 7 };
		_int	iMaxHor = { 7 };

	}SPRITEUV_DESC;

	typedef struct tagNoiseUV
	{
		_float	fIntervalTime	= { 0.05f };

		_float3 vScrollSpeeds	= { 0.f, 0.f, 0.f };
		_float3 vScales			= { 0.f, 0.f, 0.f };

		_float2 vDistortion1	= { 0.f, 0.f};
		_float2 vDistortion2	= { 0.f, 0.f };
		_float2 vDistortion3	= { 0.f, 0.f };

		_float fDistortionScale = { 0.f };
		_float fDistortionBias = { 0.f };

	}NOISE_DESC;

private:
	CEffect_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Texture(const CEffect_Texture& rhs);
	virtual ~CEffect_Texture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	CTexture*				Get_TextureCom(TEXTURE eTexture) { return m_pTextureCom[eTexture]; }

public:
	EFFECT_TEXTURE_DESC*	Get_Desc() { return &m_tEffect; }
	SPRITEUV_DESC*			Get_Sprite_Desc() { return &m_tSpriteDesc; }
	NOISE_DESC*				Get_Noise_Desc() { return &m_tNoiseDesc; }

	void					Set_Play(_bool bPlay) { m_tSpriteDesc.bPlay = bPlay; }
	void					Set_IntervalTime(_float fIntervalTime) { m_tSpriteDesc.fIntervalTime = fIntervalTime; }
	void					ReSet_Sprite();

	void					Set_TextureIndex(CEffect::TEXTURE eTexture, _int iIndex) { m_tEffect.iTextureIndex[eTexture] = iIndex; }

	void					Set_AnimationSize(_float fX, _float fY) { m_tSpriteDesc.fAnimationSizeX = fX; m_tSpriteDesc.fAnimationSizeY = fY; }
	void					Set_SpriteSize(_float fX, _float fY) { m_tSpriteDesc.fSpriteSizeX = fX; m_tSpriteDesc.fSpriteSizeY = fY; }
	void					Set_MaxVerHor(_int iX, _int iY) { m_tSpriteDesc.iMaxVer = iX; m_tSpriteDesc.iMaxHor = iY; }

	void					Set_ShaderPassIndex(_int iShaderPassIndex) { m_tEffect.iShaderPassIndex = iShaderPassIndex; }

/* For.Noise Desc*/
public:
	void Set_NoiseTimeInterval(_float fValue) { m_tNoiseDesc.fIntervalTime = fValue; }

	void Set_ScrollSpeeds(_float fX, _float fY, _float fZ) { m_tNoiseDesc.vScrollSpeeds = _float3(fX, fY, fZ); }
	void Set_Scales(_float fX, _float fY, _float fZ) { m_tNoiseDesc.vScales = _float3(fX, fY, fZ); }

	void Set_Distortion1(_float fX, _float fY) { m_tNoiseDesc.vDistortion1 = _float2(fX, fY); }
	void Set_Distortion2(_float fX, _float fY) { m_tNoiseDesc.vDistortion2 = _float2(fX, fY); }
	void Set_Distortion3(_float fX, _float fY) { m_tNoiseDesc.vDistortion3 = _float2(fX, fY); }

	void Set_DistortionScale(_float fValue) { m_tNoiseDesc.fDistortionScale = fValue; }
	void Set_DistortionBias(_float fValue) { m_tNoiseDesc.fDistortionBias = fValue; }


private:
	CShader*			m_pShaderCom				= { nullptr };
	CTexture*			m_pTextureCom[TEXTURE_END]	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom				= { nullptr };

private:
	EFFECT_TEXTURE_DESC	m_tEffect = {};
	SPRITEUV_DESC		m_tSpriteDesc = {};
	NOISE_DESC			m_tNoiseDesc = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END