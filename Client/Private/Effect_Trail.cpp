#include "stdafx.h"
#include "..\Public\Effect_Trail.h"

#include "GameInstance.h"

#include "Effect.h"

#include "Model_Preview.h"
#include "Part_Preview.h"


CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = FALSE;
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
	XMStoreFloat4x4(&m_tTrailDesc.matSocketWorld, XMMatrixIdentity());

	//m_tTrailDesc = *(TRAIL_DESC*)pArg;

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
	//CVIBuffer_Trail::TRAIL_BUFFER_DESC* pDesc = m_pVIBufferCom->Get_Desc();
#ifdef _DEBUG
	if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG

			if (FALSE == m_tVoidDesc.bPlay)
			{
				m_tVoidDesc.bRender = FALSE;
				m_pVIBufferCom->Reset_Points(m_tVoidDesc.matCombined);
				return;
			}
			else
			{
				m_tVoidDesc.bRender = TRUE;
				m_pVIBufferCom->Update(fTimeDelta, m_tVoidDesc.matCombined);
			}		

#ifdef _DEBUG
		}
	}
#endif // _DEBUG
};

void CEffect_Trail::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		if (m_tVoidDesc.bActive_Tool)
		{
			if (OWNER_PREVIEW == m_tTrailDesc.eType_Owner)
			{
				CGameObject* pParentOwner = m_pOwner->Get_Object_Owner();
				m_tTrailDesc.matSocketWorld = dynamic_cast<CPart_Preview*>(pParentOwner)->Get_WorldMatrix_Socket();
			}
#endif // _DEBUG

			if (OWNER_WEAPON == m_tTrailDesc.eType_Owner)
			{

			}
			else if (OWNER_OBJECT == m_tTrailDesc.eType_Owner)
			{
				m_tTrailDesc.matSocketWorld = m_pOwner->Get_Object_Owner()->Get_Transform()->Get_WorldFloat4x4();
			}


			if (m_tVoidDesc.bRender)
			{
				CEffect_Trail::Update_PivotMat();

				if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tVoidDesc.iRenderGroup, this)))
					return;
			}
#ifdef _DEBUG
		}
	}
#endif // _DEBUG
}

HRESULT CEffect_Trail::Render()
{
#ifdef _DEBUG
	if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG

			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			/* 이 쉐이더에 n번째 패스로 그릴거야. */
			m_pShaderCom->Begin(m_tVoidDesc.iShaderPassIndex);

			/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
			m_pVIBufferCom->Bind_VIBuffers();

			/* 바인딩된 정점, 인덱스를 그려. */
			m_pVIBufferCom->Render();

#ifdef _DEBUG
		}
	}
#endif // _DEBUG

	return S_OK;
}


void CEffect_Trail::Update_PivotMat()
{
	if (nullptr != m_pOwner)	// 부모 이펙트가 있고
	{
		if (m_tVoidDesc.bParentPivot)		//부모의 매트릭스를 사용할거고
		{
			CGameObject* pParentOwner = m_pOwner->Get_Object_Owner();
			if (nullptr != pParentOwner)	// 부모의 오너가 있으면
			{
				m_tVoidDesc.matPivot = dynamic_cast<CEffect*>(m_pOwner)->Get_Desc()->matCombined * m_tTrailDesc.matSocketWorld;
				XMStoreFloat4x4(&m_tVoidDesc.matCombined, m_pTransformCom->Get_WorldMatrix() * m_tVoidDesc.matPivot);
			}
			else
			{
				// 부모의 오너가 없으면 부모의 매트릭스만 사용
				m_tVoidDesc.matPivot = m_pOwner->Get_Transform()->Get_WorldFloat4x4();
				XMStoreFloat4x4(&m_tVoidDesc.matCombined, m_pTransformCom->Get_WorldMatrix() * m_tVoidDesc.matPivot);
			}
		}

	}
}


HRESULT CEffect_Trail::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_EffectTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));


	/* For.Com_VIBuffer */
	CVIBuffer_Trail::TRAIL_BUFFER_DESC tBufferInfo = {};
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo));


	/* For.Com_Texture */
	{
		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Texture_Effect_Diffuse"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;

		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_MASK])
		{
			/* For.Com_Mask */
			if (FAILED(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_MASK],
				TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
				return E_FAIL;
		}

		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_NOISE])
		{
			/* For.Com_Noise */
			if (FAILED(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_NOISE],
				TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
				return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
{

	/* Matrix ============================================================================================ */
	if (m_tVoidDesc.bParentPivot)
	{
		FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_tVoidDesc.matCombined));
	}
	else
	{
		FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	}

	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));


	/* Texture ============================================================================================ */
	if (m_tVoidDesc.bUseSpriteAnim)
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_SPRITE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_SPRITE]));
	}
	else
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_DIFFUSE]));
	}

	if (nullptr != m_pTextureCom[TEXTURE_MASK])
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tVoidDesc.iTextureIndex[TEXTURE_MASK]));
	}
	if (nullptr != m_pTextureCom[TEXTURE_NOISE])
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NOISE]));
	}


	/* Discard ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tVoidDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = _float3(m_tVoidDesc.vColor_Clip.x, m_tVoidDesc.vColor_Clip.y, m_tVoidDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));

	/* UV ============================================================================================ */
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tVoidDesc.fUV_RotDegree, sizeof(_float)));



	/* Camera ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	//_float3 vCamDirectionFloat3 = m_pGameInstance->Get_CamDirection();
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat3, sizeof(_float3)));

	//_float fCamFar = m_pGameInstance->Get_CamFar();
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));


	/* ETC ============================================================================================ */
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

