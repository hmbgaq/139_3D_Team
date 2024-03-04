#include "stdafx.h"
#include "..\Public\Effect_Trail.h"

#include "GameInstance.h"


CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{

}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& rhs)
	: CEffect_Void(rhs)
{
}

HRESULT CEffect_Trail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Trail::Initialize(void* pArg)
{
	m_tTrailDesc = *(TRAIL_DESC*)pArg;

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

	if (FALSE == m_tTrailDesc.bTrailOn)
	{
		m_pVIBufferCom->Reset_Points(m_tTrailDesc.matPivot);
		return;
	}

	m_pVIBufferCom->Update(fTimeDelta, m_tTrailDesc.matPivot);

};

void CEffect_Trail::Late_Tick(_float fTimeDelta)
{

	if (nullptr != m_pOwner)
	{
		if (m_tTrailDesc.bParentPivot)
		{
			m_tTrailDesc.matPivot = m_pOwner->Get_Transform()->Get_WorldFloat4x4();
			XMStoreFloat4x4(&m_tTrailDesc.matOffset, m_pTransformCom->Get_WorldMatrix() * m_tTrailDesc.matPivot);
		}
	}

	Compute_CamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT, this)))
		return;
}

HRESULT CEffect_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 쉐이더에 n번째 패스로 그릴거야. */
	m_pShaderCom->Begin(0);

	/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

void* CEffect_Trail::Get_BufferDesc()
{
	CVIBuffer_Trail::TRAIL_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.bTrailOn = m_tTrailDesc.bTrailOn;

	tBufferDesc.vPos_0	= m_tTrailDesc.vPos_0;
	tBufferDesc.vPos_1	= m_tTrailDesc.vPos_1;
	tBufferDesc.iPass	= m_tTrailDesc.iPass;

	tBufferDesc.iNumVertices = m_tTrailDesc.iNumVertices;
	tBufferDesc.iMaxCnt		= m_tTrailDesc.iMaxCnt;
	tBufferDesc.iVtxCnt		= m_tTrailDesc.iVtxCnt;

	tBufferDesc.vLocalSwordLow	= m_tTrailDesc.vLocalSwordLow;
	tBufferDesc.vLocalSwordHigh = m_tTrailDesc.vLocalSwordHigh;

	tBufferDesc.iLerpPointNum		= m_tTrailDesc.iLerpPointNum;
	tBufferDesc.iCatMullRomIndex[4] = m_tTrailDesc.iCatMullRomIndex[4];

	return &tBufferDesc;
}

HRESULT CEffect_Trail::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_EffectTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));


	/* For.Com_VIBuffer */
	CVIBuffer_Trail::TRAIL_BUFFER_DESC tBufferInfo = *static_cast<CVIBuffer_Trail::TRAIL_BUFFER_DESC*>(Get_BufferDesc());
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo));


	/* For.Com_Texture */
	{
		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;

		if (TEXT("") != m_tTrailDesc.strTextureTag[TEXTURE_MASK])
		{
			/* For.Com_Mask */
			if (FAILED(__super::Add_Component(iNextLevel, m_tTrailDesc.strTextureTag[TEXTURE_MASK],
				TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
				return E_FAIL;
		}

		if (TEXT("") != m_tTrailDesc.strTextureTag[TEXTURE_NOISE])
		{
			/* For.Com_Noise */
			if (FAILED(__super::Add_Component(iNextLevel, m_tTrailDesc.strTextureTag[TEXTURE_NOISE],
				TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
				return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
{

	if (m_tTrailDesc.bParentPivot)
	{
		FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_tTrailDesc.matOffset));
	}
	else
	{
		FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	}


	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));


	FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tTrailDesc.iTextureIndex[TEXTURE_DIFFUSE]));

	if (nullptr != m_pTextureCom[TEXTURE_MASK])
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tTrailDesc.iTextureIndex[TEXTURE_MASK]));
	}
	if (nullptr != m_pTextureCom[TEXTURE_NOISE])
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tTrailDesc.iTextureIndex[TEXTURE_NOISE]));
	}

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));

	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"));

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

