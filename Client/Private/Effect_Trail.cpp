#include "stdafx.h"
#include "..\Public\Effect_Trail.h"

#include "GameInstance.h"


CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect(pDevice, pContext, strPrototypeTag)
{

}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Trail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Trail::Initialize(void* pArg)
{
	m_tEffect = *(Effect_Trail_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CEffect_Trail::Priority_Tick(_float fTimeDelta)
{



}

void CEffect_Trail::Tick(_float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_tNoiseDesc.fIntervalTime)
	{
		m_fTimeAcc = 0.f;
	}

};

void CEffect_Trail::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 쉐이더에 0번째 패스로 그릴거야. */
	m_pShaderCom->Begin(2);

	/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Trail::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, m_tEffect.strShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	{
		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;

		if (TEXT("") != m_tEffect.strTextureTag[TEXTURE_MASK])
		{
			/* For.Com_Mask */
			if (FAILED(__super::Add_Component(iNextLevel, m_tEffect.strTextureTag[TEXTURE_MASK],
				TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
				return E_FAIL;
		}

		if (TEXT("") != m_tEffect.strTextureTag[TEXTURE_NOISE])
		{
			/* For.Com_Noise */
			if (FAILED(__super::Add_Component(iNextLevel, m_tEffect.strTextureTag[TEXTURE_NOISE],
				TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
				return E_FAIL;
		}
	}



	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tEffect.iTextureIndex[TEXTURE_DIFFUSE])))
		return E_FAIL;

	if (nullptr != m_pTextureCom[TEXTURE_MASK])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tEffect.iTextureIndex[TEXTURE_MASK])))
			return E_FAIL;
	}
	if (nullptr != m_pTextureCom[TEXTURE_NOISE])
	{
		if (FAILED(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tEffect.iTextureIndex[TEXTURE_NOISE])))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;


	_vector vCamDirection = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
	vCamDirection = XMVector4Normalize(vCamDirection);
	_float4 vCamDirectionFloat4 = {};
	XMStoreFloat4(&vCamDirectionFloat4, vCamDirection);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat4, sizeof(_float4))))
		return E_FAIL;

	//if (SINGLE == m_tEffect.eType)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrameTime", &m_fTimeAcc, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vScrollSpeeds", &m_tNoiseDesc.vScrollSpeeds, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vScales", &m_tNoiseDesc.vScales, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortion1", &m_tNoiseDesc.vDistortion1, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortion2", &m_tNoiseDesc.vDistortion2, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortion3", &m_tNoiseDesc.vDistortion3, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionScale", &m_tNoiseDesc.fDistortionScale, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionBias", &m_tNoiseDesc.fDistortionBias, sizeof(_float))))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

CEffect_Trail* CEffect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEffect_Trail* pInstance = new CEffect_Trail(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Trail::Clone(void* pArg)
{
	CEffect_Trail* pInstance = new CEffect_Trail(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Trail::Pool()
{
	return new CEffect_Trail(*this);
}

void CEffect_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
}

