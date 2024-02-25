#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEffect_Trail final : public CEffect_Void
{
public:
	typedef struct tagTextureEffectDesc : public CEffect_Void::EFFECTVOID_DESC
	{


	}TRAIL_DESC;


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
	TRAIL_DESC*		 Get_Desc() { return &m_tTrailDesc; }
	DISTORTION_DESC* Get_Noise_Desc() { return &m_tDistortionDesc; }


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
	TRAIL_DESC				m_tTrailDesc = {};
	DISTORTION_DESC			m_tDistortionDesc = {};

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