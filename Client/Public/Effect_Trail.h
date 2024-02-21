#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Trail final : public CEffect
{
public:
	typedef struct tagTextureEffectDesc : public CEffect::EFFECT_DESC
	{
		

	}Effect_Trail_DESC;


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
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

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
	Effect_Trail_DESC*		Get_Desc() { return &m_tEffect; }
	NOISE_DESC*				Get_Noise_Desc() { return &m_tNoiseDesc; }

	void					Set_TextureIndex(CEffect::TEXTURE eTexture, _int iIndex) { m_tEffect.iTextureIndex[eTexture] = iIndex; }

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
	Effect_Trail_DESC	m_tEffect = {};
	NOISE_DESC			m_tNoiseDesc = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END