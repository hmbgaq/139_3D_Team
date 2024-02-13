#pragma once

#include "Client_Defines.h"
#include "Effect_Object.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Edit final : public CEffect_Object
{
public:	
	
private:
	CParticle_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Edit(const CParticle_Edit& rhs);
	virtual ~CParticle_Edit() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Point*		m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CParticle_Edit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END