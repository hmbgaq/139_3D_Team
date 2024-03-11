#include "stdafx.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"

#include "Effect.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = FALSE;
}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: CEffect_Void(rhs)
{
}

HRESULT CEffect_Particle::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CEffect_Particle::Initialize(void* pArg)
{
	//m_tVoidDesc = *static_cast<EFFECTVOID_DESC*>(pArg);
	//*static_cast<EFFECTVOID_DESC*>(&m_tParticleDesc) = *static_cast<EFFECTVOID_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Particle::Priority_Tick(_float fTimeDelta)
{


}

void CEffect_Particle::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG
			m_tVoidDesc.fSequenceTime = m_tVoidDesc.fLifeTime + m_tVoidDesc.fRemainTime;

			if (m_tVoidDesc.bPlay)
			{
				m_tVoidDesc.fSequenceAcc += fTimeDelta;

				// 시작지연 누적시간이 지나면 렌더 시작(파티클 시작)
				if (m_tVoidDesc.fWaitingAcc <= m_tVoidDesc.fWaitingTime)
				{
					m_tVoidDesc.fWaitingAcc += fTimeDelta;

					if (m_tVoidDesc.fWaitingAcc >= m_tVoidDesc.fWaitingTime)
					{
						m_tVoidDesc.bRender = TRUE;
						//pDesc->bPlay	= TRUE;
					}
					else
						return;
				}

				// 시간 누적 시작
				m_tVoidDesc.fTimeAcc += fTimeDelta;
				m_tVoidDesc.fSpriteTimeAcc += fTimeDelta;
				m_tVoidDesc.fLifeTimeRatio = min(1.0f, m_tVoidDesc.fTimeAcc / m_tVoidDesc.fLifeTime);


				/* ======================= 라이프 타임 동작 시작 ======================= */
				if (m_tVoidDesc.bUseSpriteAnim)
				{
					if (!m_tSpriteDesc.bSpriteFinish)
					{
						if (m_tVoidDesc.fSpriteTimeAcc > m_tSpriteDesc.fSequenceTerm)
						{
							(m_tSpriteDesc.vUV_CurTileIndex.x)++;	// 가로 인덱스 증가

							if (m_tSpriteDesc.vUV_CurTileIndex.x == m_tSpriteDesc.vUV_MaxTileCount.x)
							{
								(m_tSpriteDesc.vUV_CurTileIndex.y)++;									// 세로 인덱스 증가
								m_tSpriteDesc.vUV_CurTileIndex.x = m_tSpriteDesc.vUV_MinTileCount.x;	// 가로 인덱스 초기화

								if (m_tSpriteDesc.vUV_CurTileIndex.y == m_tSpriteDesc.vUV_MaxTileCount.y)	// 세로 인덱스도 끝까지 왔다면
								{
									m_tSpriteDesc.bSpriteFinish = TRUE;										// 스프라이트 애님 끝	
								}
							}
							m_tVoidDesc.fSpriteTimeAcc = 0.f;
						}
					}

					if (m_tSpriteDesc.bSpriteFinish)
						m_tVoidDesc.bRender = FALSE;
				}

				/* ======================= 라이프 타임 동작 끝  ======================= */

				if (m_tVoidDesc.fTimeAcc >= m_tVoidDesc.fLifeTime)
				{
					// 삭제 대기시간 누적 시작
					m_tVoidDesc.fRemainAcc += fTimeDelta;

					// 디졸브 시작
					m_tVoidDesc.bDissolve = TRUE;
					if (m_tVoidDesc.bDissolve)
					{
						m_tVoidDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_tVoidDesc.fRemainAcc, m_tVoidDesc.fRemainTime, m_tVoidDesc.eType_Easing);
					}

					if (m_tVoidDesc.fRemainAcc >= m_tVoidDesc.fRemainTime)
					{
						m_tVoidDesc.fDissolveAmount = 1.f;
						m_tVoidDesc.bRender = TRUE;
						return;
					}
				}

				if (m_tVoidDesc.bRender)
				{
					m_pVIBufferCom->Update(fTimeDelta);
				}
			}
#ifdef _DEBUG
		}
	}
#endif // _DEBUG

}

void CEffect_Particle::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG
			if (m_tVoidDesc.bRender)
			{
				__super::Update_PivotMat();

				if (m_bSortZ)
				{
					//m_pVIBufferCom->Sort_Z(m_pVIBufferCom->Get_NumInstance());
				}
				Compute_CamDistance();

				// CRenderer::RENDER_BLEND
				FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tVoidDesc.iRenderGroup, this));
			}
#ifdef _DEBUG
		}
	}
#endif // _DEBUG
}

HRESULT CEffect_Particle::Render()
{
#ifdef _DEBUG
	//if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			/* 이 쉐이더에 0번째 패스로 그릴거야. */
			m_pShaderCom->Begin(m_tVoidDesc.iShaderPassIndex);

			/* 내가 그리려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
			m_pVIBufferCom->Bind_VIBuffers();

			/* 바인딩된 정점, 인덱스를 그려. */
			m_pVIBufferCom->Render();
#ifdef _DEBUG
		}
	}
#endif // _DEBUG


	return S_OK;
}

void CEffect_Particle::ReSet_Effect()
{
	__super::ReSet_Effect();

	m_tVoidDesc.fDissolveAmount = 0.f;
	m_tVoidDesc.bDissolve = FALSE;
	m_tVoidDesc.bRender = FALSE;

	if (m_tVoidDesc.bUseSpriteAnim)
	{
		m_tSpriteDesc.bSpriteFinish = FALSE;
		m_tSpriteDesc.vUV_CurTileIndex.y = m_tSpriteDesc.vUV_MinTileCount.y;
		m_tSpriteDesc.vUV_CurTileIndex.x = m_tSpriteDesc.vUV_MinTileCount.x;
	}

	m_pVIBufferCom->ReSet();

}

void CEffect_Particle::End_Effect()
{
	__super::End_Effect();

	if (m_tVoidDesc.bLoop)
	{
		ReSet_Effect();
	}

}

_bool CEffect_Particle::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	//Write_VoidDesc(Out_Json, &m_tParticleDesc);

	// m_tParticleDesc


	/* Sprite */
	Out_Json["fSequenceTerm"] = m_tSpriteDesc.fSequenceTerm;

	CJson_Utility::Write_Float2(Out_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Write_Float2(Out_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Write_Float2(Out_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Write_Float2(Out_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);


	return true;
}

void CEffect_Particle::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);


	//*&m_tParticleDesc = *static_cast<PARTICLE_DESC*>(Load_VoidDesc(In_Json));
	//*static_cast<EFFECTVOID_DESC*>(&m_tParticleDesc) = *static_cast<EFFECTVOID_DESC*>(Load_VoidDesc(In_Json));
	//static_cast<EFFECTVOID_DESC>(m_tParticleDesc) = Load_VoidDesc(In_Json); // 왜???????????????????????????


	// m_tParticleDesc


	/* Sprite */
	m_tSpriteDesc.fSequenceTerm = In_Json["fSequenceTerm"];

	CJson_Utility::Load_Float2(In_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Load_Float2(In_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Load_Float2(In_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Load_Float2(In_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);

}


HRESULT CEffect_Particle::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_VIBuffer */
	{
		CVIBuffer_Particle::PARTICLE_BUFFER_DESC tBufferDesc = {};
		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferDesc)))
			return E_FAIL;
	}


	/* For.Com_Texture */
	{
		FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_DIFFUSE], TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));
		FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_SPRITE], TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));


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


HRESULT CEffect_Particle::Bind_ShaderResources()
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

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bBillBoard", &m_tVoidDesc.bBillBoard, sizeof(_bool)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tVoidDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = _float3(m_tVoidDesc.vColor_Clip.x, m_tVoidDesc.vColor_Clip.y, m_tVoidDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));

	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vColor_Mul", &m_tVoidDesc.vColor_Mul, sizeof(_float4)));

	/* UV ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tVoidDesc.fUV_RotDegree, sizeof(_float)));

	// 이펙트 정보
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_EffectDesc", m_pVIBufferCom->Get_ParticleShaderInfoDescs().data(), sizeof(CVIBuffer_Particle::PARTICLE_SHADER_INFO_DESC) * m_pVIBufferCom->Get_ParticleShaderInfoDescs().size()));


	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bSprite", &m_tVoidDesc.bUseSpriteAnim, sizeof(_bool)));
	if (m_tVoidDesc.bUseSpriteAnim)
	{
		m_tVoidDesc.vUV_Offset = { (_float)(m_tSpriteDesc.vUV_CurTileIndex.x * m_tSpriteDesc.vTileSize.x) / m_tSpriteDesc.vTextureSize.x
									, (_float)(m_tSpriteDesc.vUV_CurTileIndex.y * m_tSpriteDesc.vTileSize.y) / m_tSpriteDesc.vTextureSize.y };

		m_tVoidDesc.vUV_Scale = { (_float)m_tSpriteDesc.vTileSize.x / m_tSpriteDesc.vTextureSize.x
								, (_float)m_tSpriteDesc.vTileSize.y / m_tSpriteDesc.vTextureSize.y };


		FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tVoidDesc.vUV_Offset, sizeof(_float2)));
		FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tVoidDesc.vUV_Scale, sizeof(_float2)));
	}


	/* Camera ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	_float3 vCamDirectionFloat3 = m_pGameInstance->Get_CamDirection();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat3, sizeof(_float3)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));


	/* ETC ============================================================================================ */
	FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"));


	return S_OK;
}

CEffect_Particle* CEffect_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEffect_Particle* pInstance = new CEffect_Particle(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Particle::Clone(void* pArg)
{
	CEffect_Particle* pInstance = new CEffect_Particle(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Particle::Pool()
{
	return new CEffect_Particle(*this);
}

void CEffect_Particle::Free()
{
	__super::Free();

	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pVIBufferCom_Model);

	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
}

