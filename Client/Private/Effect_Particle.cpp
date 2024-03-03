#include "stdafx.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"

#include "Effect.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{

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

	*static_cast<EFFECTVOID_DESC*>(&m_tParticleDesc) = *static_cast<EFFECTVOID_DESC*>(pArg);
	

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
	//CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC* pDesc = m_pVIBufferCom->Get_Desc();
	CVIBuffer_Particle::PARTICLE_BUFFER_DESC* pDesc = m_pVIBufferCom->Get_Desc();

	if (m_tParticleDesc.bActive_Tool)
	{
		m_fSequenceTime = m_fLifeTime + m_fRemainTime;

		//pDesc->vMinMaxLifeTime.x = m_fWaitingTime;
		//pDesc->vMinMaxLifeTime.y = m_fLifeTime;

		if (m_tParticleDesc.bPlay)
		{
			m_fSequenceAcc += fTimeDelta;

			// 시작지연 누적시간이 지나면 렌더 시작(파티클 시작)
			if (m_fWaitingAcc <= m_fWaitingTime)
			{
				m_fWaitingAcc += fTimeDelta;

				if (m_fWaitingAcc >= m_fWaitingTime)
				{
					m_tParticleDesc.bRender = TRUE;
					//pDesc->bPlay	= TRUE;
				}
				else
					return;
			}

			// 시간 누적 시작
			m_fTimeAcc += fTimeDelta;
			m_fSpriteTimeAcc += fTimeDelta;
			m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_fLifeTime);


			/* ======================= 라이프 타임 동작 시작 ======================= */
			if (m_tParticleDesc.bUseSpriteAnim)
			{
				if (!m_tSpriteDesc.bSpriteFinish)
				{
					if (m_fSpriteTimeAcc > m_tSpriteDesc.fSequenceTerm)
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
						m_fSpriteTimeAcc = 0.f;
					}
				}
	
				if (m_tSpriteDesc.bSpriteFinish)
					m_tParticleDesc.bRender = FALSE;
			}

			/* ======================= 라이프 타임 동작 끝  ======================= */

			if (m_fTimeAcc >= m_fLifeTime)
			{
				// 삭제 대기시간 누적 시작
				m_fRemainAcc += fTimeDelta;

				// 디졸브 시작
				m_tParticleDesc.bDissolve = TRUE;
				if (m_tParticleDesc.bDissolve)
				{
					m_tParticleDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_fRemainAcc, m_fRemainTime, m_tParticleDesc.eType_Easing);
				}

				if (m_fRemainAcc >= m_fRemainTime)
				{
					m_tParticleDesc.fDissolveAmount = 1.f;
					m_tParticleDesc.bRender = TRUE;
					return;
				}
			}

			if (m_tParticleDesc.bRender)
			{
				m_pVIBufferCom->Update(fTimeDelta);
			}
		}
	}
	//else
	//{
	//	m_tParticleDesc.bActive_Tool = FALSE;
	//}
}

void CEffect_Particle::Late_Tick(_float fTimeDelta)
{
	if (m_tParticleDesc.bActive_Tool)
	{
		if (m_tParticleDesc.bRender)
		{
			if (nullptr != m_pOwner)
			{
				if (m_tParticleDesc.bParentPivot)
				{			
					m_tParticleDesc.matPivot = m_pOwner->Get_Transform()->Get_WorldFloat4x4();
					XMStoreFloat4x4(&m_tParticleDesc.matOffset, m_pTransformCom->Get_WorldMatrix() * m_tParticleDesc.matPivot);
				}
			}
			
			if (m_bSortZ)
			{
				//m_pVIBufferCom->Sort_Z(m_pVIBufferCom->Get_NumInstance());
			}
			//Compute_CamDistance();

			// CRenderer::RENDER_BLEND
			//if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tParticleDesc.iRenderGroup), this)))
			//	return;
			FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT, this));
		}
	}
}

HRESULT CEffect_Particle::Render()
{
	if (m_tParticleDesc.bActive_Tool)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		/* 이 쉐이더에 0번째 패스로 그릴거야. */
		m_pShaderCom->Begin(m_tParticleDesc.iShaderPassIndex);

		/* 내가 그리려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
		m_pVIBufferCom->Bind_VIBuffers();

		/* 바인딩된 정점, 인덱스를 그려. */
		m_pVIBufferCom->Render();
	}


	return S_OK;
}

void CEffect_Particle::ReSet_Effect()
{
	__super::ReSet_Effect();

	m_tParticleDesc.fDissolveAmount = 0.f;
	m_tParticleDesc.bDissolve = FALSE;
	m_tParticleDesc.bRender = FALSE;

	if (m_tParticleDesc.bUseSpriteAnim)
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

	if (m_tParticleDesc.bLoop)
	{
		ReSet_Effect();
	}

}

_bool CEffect_Particle::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	Write_VoidDesc(Out_Json, &m_tParticleDesc);


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

	*&m_tParticleDesc = *static_cast<PARTICLE_DESC*>(Load_VoidDesc(In_Json));


	 /* Sprite */
	m_tSpriteDesc.fSequenceTerm = In_Json["fSequenceTerm"];

	CJson_Utility::Load_Float2(In_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Load_Float2(In_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Load_Float2(In_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Load_Float2(In_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);

}


void* CEffect_Particle::Get_BufferDesc()
{
	// 초기화용

	CVIBuffer_Particle::PARTICLE_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.iCurNumInstance = m_tParticleDesc.iCurNumInstance;

	/* LifeTime */
	tBufferDesc.vMinMaxLifeTime = m_tParticleDesc.vMinMaxLifeTime;

	/* RigidBody */
	tBufferDesc.bUseRigidBody = m_tParticleDesc.bUseRigidBody;
	tBufferDesc.bKinetic = m_tParticleDesc.bKinetic;
	tBufferDesc.bUseGravity = m_tParticleDesc.bUseGravity;
	tBufferDesc.eForce_Mode = m_tParticleDesc.eForce_Mode;

	tBufferDesc.fGravity = m_tParticleDesc.fGravity;
	tBufferDesc.fFriction = m_tParticleDesc.fFriction;
	tBufferDesc.fSleepThreshold = m_tParticleDesc.fSleepThreshold;
	tBufferDesc.byFreezeAxis = m_tParticleDesc.byFreezeAxis;

	tBufferDesc.vMinMaxPower = m_tParticleDesc.vMinMaxPower;
	tBufferDesc.vMinMaxMass = m_tParticleDesc.vMinMaxMass;

	/* For.Position */
	tBufferDesc.vCenterPosition = m_tParticleDesc.vCenterPosition;
	tBufferDesc.vMinMaxRange = m_tParticleDesc.vMinMaxRange;

	/* For.Rotation */
	tBufferDesc.vMinMaxRotationOffsetX = m_tParticleDesc.vMinMaxRotationOffsetX;
	tBufferDesc.vMinMaxRotationOffsetY = m_tParticleDesc.vMinMaxRotationOffsetY;
	tBufferDesc.vMinMaxRotationOffsetZ = m_tParticleDesc.vMinMaxRotationOffsetZ;


	/* For.Color */
	tBufferDesc.eType_ColorLerp = m_tParticleDesc.eType_ColorLerp;
	tBufferDesc.bDynamic_Color = m_tParticleDesc.bDynamic_Color;
	tBufferDesc.vMinMaxRed = m_tParticleDesc.vMinMaxRed;
	tBufferDesc.vMinMaxGreen = m_tParticleDesc.vMinMaxGreen;
	tBufferDesc.vMinMaxBlue = m_tParticleDesc.vMinMaxBlue;
	tBufferDesc.vMinMaxAlpha = m_tParticleDesc.vMinMaxAlpha;


	return &tBufferDesc;
}


HRESULT CEffect_Particle::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	///* For.Com_VIBuffer */
	//{
	//	CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC tBufferInfo = *static_cast<CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC*>(Get_BufferDesc());
	//	//CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC tBufferInfo = {};
	//	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle_Point"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo)))
	//		return E_FAIL;
	//}

	/* For.Com_VIBuffer */
	{
		CVIBuffer_Particle::PARTICLE_BUFFER_DESC tBufferInfo = *static_cast<CVIBuffer_Particle::PARTICLE_BUFFER_DESC*>(Get_BufferDesc());
		//CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC tBufferInfo = {};
		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo)))
			return E_FAIL;
	}


	/* For.Com_Texture */
	FAILED_CHECK(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_DIFFUSE], TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));
	FAILED_CHECK(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_SPRITE], TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));


	if (TEXT("") != m_tParticleDesc.strTextureTag[TEXTURE_MASK])
	{
		/* For.Com_Mask */
		if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_MASK],
			TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
			return E_FAIL;
	}

	if (TEXT("") != m_tParticleDesc.strTextureTag[TEXTURE_NOISE])
	{
		/* For.Com_Noise */
		if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_NOISE],
			TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CEffect_Particle::Bind_ShaderResources()
{
	//if (FAILED(__super::Bind_ShaderResources()))
	//	return E_FAIL;

	/* Matrix ============================================================================================ */
	/* =================================================================================================== */
	if (m_tParticleDesc.bParentPivot)
	{
		FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_tParticleDesc.matOffset));
	}
	else
	{
		FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	}

	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));


	/* Texture ============================================================================================ */
	/* ==================================================================================================== */
	if (m_tParticleDesc.bUseSpriteAnim)
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_SPRITE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_SPRITE]));
	}
	else
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_DIFFUSE]));
	}


	if (nullptr != m_pTextureCom[TEXTURE_MASK])
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tParticleDesc.iTextureIndex[TEXTURE_MASK]));
	}
	if (nullptr != m_pTextureCom[TEXTURE_NOISE])
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_NOISE]));
	}

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bBillBoard", &m_tParticleDesc.bBillBoard, sizeof(_bool)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tParticleDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = float3(m_tParticleDesc.vColor_Clip.x, m_tParticleDesc.vColor_Clip.y, m_tParticleDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));

	/* UV ============================================================================================ */
	/* =============================================================================================== */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tParticleDesc.fUV_RotDegree, sizeof(_float)));


	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bSprite", &m_tParticleDesc.bUseSpriteAnim, sizeof(_bool)));
	if (m_tParticleDesc.bUseSpriteAnim)
	{
		m_tParticleDesc.vUV_Offset = { (_float)(m_tSpriteDesc.vUV_CurTileIndex.x * m_tSpriteDesc.vTileSize.x) / m_tSpriteDesc.vTextureSize.x
									, (_float)(m_tSpriteDesc.vUV_CurTileIndex.y * m_tSpriteDesc.vTileSize.y) / m_tSpriteDesc.vTextureSize.y };

		m_tParticleDesc.vUV_Scale = { (_float)m_tSpriteDesc.vTileSize.x / m_tSpriteDesc.vTextureSize.x
									, (_float)m_tSpriteDesc.vTileSize.y / m_tSpriteDesc.vTextureSize.y };


		FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tParticleDesc.vUV_Offset, sizeof(_float2)));
		FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tParticleDesc.vUV_Scale, sizeof(_float2)));
	}


	/* Camera ============================================================================================ */
	/* =================================================================================================== */
	_vector vCamDirection = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
	vCamDirection = XMVector4Normalize(vCamDirection);
	_float4 vCamDirectionFloat4 = {};
	XMStoreFloat4(&vCamDirectionFloat4, vCamDirection);

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat4, sizeof(_float4)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	/* ETC ============================================================================================ */
	/* ================================================================================================ */
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

