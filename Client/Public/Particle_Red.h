#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Red final : public CGameObject
{
public:	
	CParticle_Red(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Red(const CParticle_Red& rhs);
	virtual ~CParticle_Red() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CShader>						m_pShaderCom = { nullptr };
	shared_ptr<CTexture>					m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Particle_Point>	m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CParticle_Red> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END