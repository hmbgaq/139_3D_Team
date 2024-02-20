#pragma once

#include "Client_Defines.h"
#include "Effect_Object.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Custom final : public CEffect_Object
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_NOISE, TYPE_END };

	typedef struct tagParticleCustomDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring		strTextureTag;
		_int		iMaxNumInstance;

	}PARTICLE_CUSTOM_DESC;

private:
	CParticle_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CParticle_Custom(const CParticle_Custom& rhs);
	virtual ~CParticle_Custom() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;


	//public:
	//	void Update_ParticlePosition(const _uint& i, _float fTimeDelta);


public:
	void		Set_Active(_bool bActive) { m_bActive = bActive; }

	void		Set_TextureIndex(TEXTURE eTexture, _int iIndex) { m_iTextureIndex[eTexture] = iIndex; }
	_int		Get_TextureIndex(TEXTURE eTexture) { return m_iTextureIndex[eTexture]; }

	void		Set_RotateUvDegree(_float fDegree) { m_fRotateUvDegree = fDegree; }
	_float		Get_RotateUvDegree() { return m_fRotateUvDegree; }

public:
	CVIBuffer_Particle_Point* Get_VIBufferCom() { return m_pVIBufferCom; }
	CTransform* Get_TransformCom() { return m_pTransformCom; }


private:
	wstring			m_strTextureTag;

	_int			m_iTextureIndex[TYPE_END] = { 0 };

	_float			m_fRotateUvDegree = { 0.f };

private:
	_bool			m_bActive = { TRUE };
	_float			m_fTimer = { 0.0f };


private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Particle_Point* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CParticle_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END