#include "stdafx.h"
#include "..\Public\Particle_Edit.h"

#include "GameInstance.h"

CParticle_Edit::CParticle_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Object(pDevice, pContext)
{

}

CParticle_Edit::CParticle_Edit(const CParticle_Edit & rhs)
	: CEffect_Object(rhs)
{
}

HRESULT CParticle_Edit::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CParticle_Edit::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Edit::Priority_Tick(_float fTimeDelta)
{
	

	
}

void CParticle_Edit::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pVIBufferCom->Update(fTimeDelta);
	}
}

void CParticle_Edit::Late_Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
			return;
	}
}

HRESULT CParticle_Edit::Render()
{
	if (m_bActive)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		/* 이 쉐이더에 0번째 패스로 그릴거야. */
		m_pShaderCom->Begin(0);

		/* 내가 그리려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
		m_pVIBufferCom->Bind_VIBuffers();

		/* 바인딩된 정점, 인덱스를 그려. */
		m_pVIBufferCom->Render();
	}


	return S_OK;
}

HRESULT CParticle_Edit::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC		ParticleDesc = {};
	ParticleDesc.vCenter	= _float3(0.f, 0.f, 0.f);
	ParticleDesc.fRange		= 3.f;
	ParticleDesc.vSpeed = _float2(0.1f, 5.0f);

	ParticleDesc.vScale		= _float2(0.2f, 0.5f);
	ParticleDesc.vRotX		= _float2(0.0f, 360.f);
	ParticleDesc.vRotY		= _float2(0.0f, 360.f);
	ParticleDesc.vRotZ		= _float2(0.0f, 360.f);

	ParticleDesc.vColor		= _float4(1.f, 1.f, 1.f, 1.f);

	ParticleDesc.vLifeTime	= _float2(0.5f, 3.0f);
	ParticleDesc.fAcceleration = { 2.f };
	ParticleDesc.fAccPosition = { 0.1f };

	m_tParticleDesc = ParticleDesc;
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Edit::Bind_ShaderResources()
{	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CParticle_Edit * CParticle_Edit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Edit*		pInstance = new CParticle_Edit(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Edit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticle_Edit::Clone(void* pArg)
{
	CParticle_Edit*		pInstance = new CParticle_Edit(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Edit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle_Edit::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

