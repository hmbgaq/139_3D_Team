#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CEffect_Particle final : public CEffect_Void
{
public:
	enum TYPE { SINGLE, SPRITE, TEXTURE_TYPE_END };

	typedef struct tagParticleDesc : public CEffect_Void::EFFECTVOID_DESC
	{
		TYPE	eType			= { SINGLE };


	}PARTICLE_DESC;


private:
	CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual void	ReSet_Effect()	override;
	virtual void	End_Effect()	override;

public:
	virtual _bool Write_Json(json& Out_Json)		 override;
	virtual void  Load_FromJson(const json& In_Json) override;


	/* For.Desc */
public:
	PARTICLE_DESC*	Get_Desc() { return &m_tParticleDesc; }


public:
	CVIBuffer_Particle_Point* Get_VIBufferCom() { return m_pVIBufferCom; }


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	PARTICLE_DESC				m_tParticleDesc = {};
	//CVIBuffer_Particle_Point::PARTICLE_POINT_DESC 
	//UVSPRITE_DESC				m_tSpriteDesc = {};


private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom[TEXTURE_END] = { nullptr };
	CVIBuffer_Particle_Point*	m_pVIBufferCom = { nullptr };


public:
	/* 원형객체를 생성한다. */
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END