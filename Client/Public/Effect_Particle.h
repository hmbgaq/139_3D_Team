#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle;
END

BEGIN(Client)

class CEffect_Particle final : public CEffect_Void
{
public:
	//typedef struct tagParticleDesc
	//{



	//}PARTICLE_DESC;


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
	virtual void	Init_ReSet_Effect() override;
	virtual void	End_Effect()	override;

public:
	virtual _bool Write_Json(json& Out_Json)		 override;
	virtual void  Load_FromJson(const json& In_Json) override;

	void			Set_SortZ(_bool bSortz) { m_bSortZ = bSortz; }
	_bool			Get_SortZ() { return m_bSortZ; }

	/* For.Desc */
public:
	//PARTICLE_DESC* Get_Desc() { return &m_tParticleDesc; }
	UVSPRITE_DESC* Get_Sprite_Desc() { return &m_tSpriteDesc; }
	DISTORTION_DESC* Get_Distortion_Desc() { return &m_tDistortionDesc; }


// 툴 용
public:
	HRESULT			Change_TextureCom(wstring strProtoTextureTag);
	HRESULT			Remove_TextureCom(TEXTURE eTexture);


public:
	CVIBuffer_Particle* Get_VIBufferCom() { return m_pVIBufferCom; }
	CTexture* Get_TextureCom(TEXTURE eTexture) { return m_pTextureCom[eTexture]; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	//PARTICLE_DESC				m_tParticleDesc = {};
	UVSPRITE_DESC				m_tSpriteDesc		= {};
	DISTORTION_DESC				m_tDistortionDesc	= {};


private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom[TEXTURE_END] = { nullptr };
	CVIBuffer_Particle*			m_pVIBufferCom = { nullptr };

	_bool						m_bSortZ = { FALSE };

	_bool						m_bFirst = { TRUE };

public:
	/* 원형객체를 생성한다. */
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END