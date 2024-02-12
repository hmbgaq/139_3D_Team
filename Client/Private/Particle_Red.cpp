#include "stdafx.h"
#include "..\Public\Particle_Red.h"

#include "GameInstance.h"

_uint		iTexIndex = rand() % 3;

CParticle_Red::CParticle_Red(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CParticle_Red::CParticle_Red(const CParticle_Red & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle_Red::Initialize_Prototype()
{
	/* 원형객체의 초기화과정을 수행한다. */
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CParticle_Red::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 10.f, 0.f, 1.f));

	return S_OK;
}

void CParticle_Red::Priority_Tick(_float fTimeDelta)
{
	

	
}

void CParticle_Red::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Update(fTimeDelta);


}

void CParticle_Red::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_ptr<CParticle_Red>(this))))//
		return;
}

HRESULT CParticle_Red::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰ㅒ이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(0);

	/* 내가 그릴려고하는 정점, 인덷ㄱ스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Red::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */

	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC		ParticleDesc = {};
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.fRange = 3.f;
	ParticleDesc.vScale = _float2(0.2f, 0.5f);
	ParticleDesc.vSpeed = _float2(0.1f, 5.0f);
	ParticleDesc.vLifeTime = _float2(0.5f, 3.0f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom), &ParticleDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Red::Bind_ShaderResources()
{	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 50)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CParticle_Red> CParticle_Red::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	shared_ptr<CParticle_Red>		pInstance = make_shared<CParticle_Red>(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Red");
		Safe_Release(pInstance);
	}
	return pInstance;
}

shared_ptr<CGameObject> CParticle_Red::Clone(void* pArg)
{
	shared_ptr<CParticle_Red>		pInstance = make_shared<CParticle_Red>(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Red");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle_Red::Free()
{
	__super::Free();

	//Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pShaderCom);
}

