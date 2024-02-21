#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Rect final : public CEffect_Void
{
public:
	enum TYPE { SINGLE, SPRITE, TYPE_END };

	typedef struct tagTextureEffectDesc : public CEffect_Void::EFFECTVOID_DESC
	{
		TYPE		eType = { SINGLE };

	}EFFECT_RECT_DESC;



private:
	CEffect_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Rect(const CEffect_Rect& rhs);
	virtual ~CEffect_Rect() = default;

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
	EFFECT_RECT_DESC*		Get_Desc() { return &m_tRectDesc; }
	UVSPRITE_DESC*			Get_Sprite_Desc() { return &m_tSpriteDesc; }
	DISTORTION_DESC*		Get_Noise_Desc() { return &m_tDistortionDesc; }

	void					Set_Play(_bool bPlay) { m_tRectDesc.bPlay = bPlay; }
	void					Set_TextureIndex(CEffect_Void::TEXTURE eTexture, _int iIndex) { m_tRectDesc.iTextureIndex[eTexture] = iIndex; }

/* For.UVSprite Desc */
	void					ReSet_Sprite();
	void					Set_SpriteTerm(_float fSequenceTerm) { m_tSpriteDesc.fSequenceTerm = fSequenceTerm; }

	void					Set_SpriteTileSize(_float fX, _float fY) { m_tSpriteDesc.vTileSize.x = fX; m_tSpriteDesc.vTileSize.y = fY; }
	void					Set_SpriteTexSize(_float fX, _float fY) { m_tSpriteDesc.vTextureSize.x = fX; m_tSpriteDesc.vTextureSize.y = fY; }
	void					Set_MaxTileCount(_int iX, _int iY) { m_tSpriteDesc.vUV_MaxTileCount.x = iX; m_tSpriteDesc.vUV_MaxTileCount.y = iY; }

	void					Set_ShaderPassIndex(_int iShaderPassIndex) { m_tRectDesc.iShaderPassIndex = iShaderPassIndex; }

	void					Set_DiscardValue(_float fDiscardValue) { m_tRectDesc.vColor_Clip.z = fDiscardValue; }

/* For.Distortion Desc*/
public:
	void Set_DistortionTerm(_float fValue) { m_tDistortionDesc.fSequenceTerm = fValue; }

	void Set_ScrollSpeeds(_float fX, _float fY, _float fZ) { m_tDistortionDesc.vScrollSpeeds = _float3(fX, fY, fZ); }
	void Set_Scales(_float fX, _float fY, _float fZ) { m_tDistortionDesc.vScales = _float3(fX, fY, fZ); }

	void Set_Distortion1(_float fX, _float fY) { m_tDistortionDesc.vDistortion1 = _float2(fX, fY); }
	void Set_Distortion2(_float fX, _float fY) { m_tDistortionDesc.vDistortion2 = _float2(fX, fY); }
	void Set_Distortion3(_float fX, _float fY) { m_tDistortionDesc.vDistortion3 = _float2(fX, fY); }

	void Set_DistortionScale(_float fValue) { m_tDistortionDesc.fDistortionScale = fValue; }
	void Set_DistortionBias(_float fValue) { m_tDistortionDesc.fDistortionBias = fValue; }


private:
	CShader*			m_pShaderCom				= { nullptr };
	CTexture*			m_pTextureCom[TEXTURE_END]	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom				= { nullptr };

private:
	EFFECT_RECT_DESC	m_tRectDesc = {};
	UVSPRITE_DESC		m_tSpriteDesc = {};
	DISTORTION_DESC		m_tDistortionDesc = {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END