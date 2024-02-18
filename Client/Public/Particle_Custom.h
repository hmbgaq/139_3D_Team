#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Custom final : public CAlphaObject
{
public:
	enum TYPE { SINGLE, SPRITE, TEXTURE_TYPE_END };
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_NOISE, TYPE_END };

	typedef struct tagParticleCustomDesc : public CGameObject::GAMEOBJECT_DESC
	{
		TYPE		eType = { SINGLE };

		wstring		strTextureTag[TYPE_END];
		_int		iTextureIndex[TYPE_END] = { 0 };

		wstring		strShaderTag = {TEXT("")};
		_uint		iShaderPassIndex = { 0 };

		_int		iRenderGroup = { 7 };	//! 밖에서 렌더러의 렌더그룹을 인트로 형변환해서 던져주자 현재 작성기준 CRENDERER::RENDERGROUP::RENDER_END가 8임

		_uint		iNumInstance = { 0 };
		_uint		iMaxNumInstance;

		_float		fRotateUvDegree;

	}PARTICLE_CUSTOM_DESC;

	typedef struct tagSpriteUV
	{
		_float	fTimeAcc = { 0.f };
		_float	fAddTime = { 0.05f };

		_float	fAnimationSizeX = { 292.5f };
		_float	fAnimationSizeY = { 292.5f };

		_float	fSpriteSizeX = { 2048.f };
		_float	fSpriteSizeY = { 2048.f };

		_int	iCurrentVer = { 0 };
		_int	iCurrentHor = { 0 };

		_int	iMinVer = { 0 };
		_int	iMinHor = { 0 };

		_int	iMaxVer = { 7 };
		_int	iMaxHor = { 7 };

	}SPRITEUV_DESC;

private:
	CParticle_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Custom(const CParticle_Custom& rhs);
	virtual ~CParticle_Custom() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual _bool Write_Json(json& Out_Json)		 override;
	virtual void  Load_FromJson(const json& In_Json) override;

	/* For.Desc */
public:
	PARTICLE_CUSTOM_DESC*	Get_Desc() { return &m_tParticleDesc; }
	SPRITEUV_DESC*			Get_Sprite_Desc() { return &m_tSpriteDesc; }

	void		Set_Active(_bool bActive) { m_bActive = bActive; }

	void		Set_TextureIndex(TEXTURE eTexture, _int iIndex) { m_tParticleDesc.iTextureIndex[eTexture] = iIndex; }

	void		Set_RotateUvDegree(_float fDegree) { m_tParticleDesc.fRotateUvDegree = fDegree; }

public:
	CVIBuffer_Particle_Point* Get_VIBufferCom() { return m_pVIBufferCom; }


private:
	PARTICLE_CUSTOM_DESC		m_tParticleDesc = {};
	SPRITEUV_DESC				m_tSpriteDesc = {};

private:
	_bool			m_bActive = { TRUE };


private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Particle_Point*	m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CParticle_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END