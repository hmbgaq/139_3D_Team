#include "stdafx.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{

}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Particle::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CEffect_Particle::Initialize(void* pArg)
{
	m_tParticleDesc = *(EFFECT_PARTICLE_DESC*)pArg;

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
	if (m_bActive)
	{
		//if (GetKeyState(VK_LEFT) & 0x8000)
		//{
		//	//m_pTransformCom->Go_Left(fTimeDelta);
		//	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * -1.f);
		//}

		//_float4 vParticlePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		__super::Tick(fTimeDelta);


		if (SPRITE == m_tParticleDesc.eType)
		{
			m_tSpriteDesc.fTimeAcc += fTimeDelta;

			if (m_tSpriteDesc.fTimeAcc > m_tSpriteDesc.fAddTime)
			{
				m_tSpriteDesc.iCurrentHor++;

				if (m_tSpriteDesc.iCurrentHor == m_tSpriteDesc.iMaxHor)
				{
					m_tSpriteDesc.iCurrentVer++;
					m_tSpriteDesc.iCurrentHor = m_tSpriteDesc.iMinVer;

					if (m_tSpriteDesc.iCurrentVer == m_tSpriteDesc.iMaxVer)
					{
						m_tSpriteDesc.iCurrentVer = m_tSpriteDesc.iMinHor;
					}
				}

				m_tSpriteDesc.fTimeAcc = 0.f;
			}
		}


		m_pVIBufferCom->Set_Color(m_tVariables.vColor_Offset.x, m_tVariables.vColor_Offset.y, m_tVariables.vColor_Offset.z);
		m_pVIBufferCom->Update(fTimeDelta);

	}
}

void CEffect_Particle::Late_Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		// CRenderer::RENDER_BLEND
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tParticleDesc.iRenderGroup), this)))
			return;
	}
}

HRESULT CEffect_Particle::Render()
{
	if (m_bActive)
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

_bool CEffect_Particle::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Out_Json["strTextureTag"][i] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]);

	Out_Json["strShaderTag"] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag);

	return true;
}

void CEffect_Particle::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]) = In_Json["strTextureTag"][i];

	m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag) = In_Json["strShaderTag"];

}


HRESULT CEffect_Particle::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	{
		CVIBuffer_Particle_Point::PARTICLE_POINT_DESC		tVIBufferDesc = {};
		tVIBufferDesc.eType_Action = { CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE };
		tVIBufferDesc.eType_Fade = { CVIBuffer_Particle_Point::TYPE_FADE::FADE_OUT };

		tVIBufferDesc.bActive		= { TRUE };
		tVIBufferDesc.bBillBoard	= { TRUE };
		tVIBufferDesc.bIsPlay		= { TRUE };
		tVIBufferDesc.bReverse		= { FALSE };
		tVIBufferDesc.bLoop			= { TRUE };

		tVIBufferDesc.vMinMaxLifeTime = _float2(0.5f, 3.0f);
		tVIBufferDesc.vMinMaxSpawnTime = { 0.f, 0.f };
		tVIBufferDesc.iCurNumInstance = { m_tParticleDesc.iNumInstance };

		tVIBufferDesc.vMinMaxRange = { 0.1f, 3.f };
		tVIBufferDesc.vCenterPosition = _float3(0.f, 0.f, 0.f);
		tVIBufferDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);

		tVIBufferDesc.vMinMaxSpeed = _float2(0.1f, 5.0f);
		tVIBufferDesc.fAcceleration = { 2.f };
		tVIBufferDesc.fAccPosition = { 0.1f };

		tVIBufferDesc.fGravityAcc = { -9.8 };
		tVIBufferDesc.vCurrentGravity = { 0.f, 0.f, 0.f };

		tVIBufferDesc.vMinMaxRotationOffsetX = { 0.0f, 360.f };
		tVIBufferDesc.vMinMaxRotationOffsetY = { 0.0f, 360.f };
		tVIBufferDesc.vMinMaxRotationOffsetZ = { 0.0f, 360.f };

		tVIBufferDesc.vCurrentRotation = { 0.f, 0.f, 0.f };
		tVIBufferDesc.vMinMaxRotationForce = { 0.f, 0.f, 0.f };

		tVIBufferDesc.vMinMaxScale = _float2(0.2f, 0.5f);
		tVIBufferDesc.vAddScale = { 0.f, 0.f };
		tVIBufferDesc.vMinMaxScaleForce = { 1.f, 1.f };

		tVIBufferDesc.vCurrentColor = _float4(1.f, 1.f, 1.f, 1.f);
		tVIBufferDesc.vColorSpeed = { 0.f, 0.f, 0.f, 0.f };
		tVIBufferDesc.vColorForce = { 0.f, 0.f, 0.f, 0.f };

		tVIBufferDesc.fMinMaxAlpha = { 1.f, 1.f };
		tVIBufferDesc.fAlphaForce = { 0.f };

		tVIBufferDesc.vSpriteUV = { 0.f, 0.f };
		tVIBufferDesc.vSpriteUVForce = { 0.f, 0.f };
		tVIBufferDesc.iSpriteFrameIndex = { 1 };

		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &tVIBufferDesc)))
			return E_FAIL;
	}


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iNextLevel, m_tParticleDesc.strTextureTag[TEXTURE_DIFFUSE],
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
		return E_FAIL;

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
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tParticleDesc.fRotateUvDegree, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_DIFFUSE])))
		return E_FAIL;

	if (nullptr != m_pTextureCom[TEXTURE_MASK])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tParticleDesc.iTextureIndex[TEXTURE_MASK])))
			return E_FAIL;
	}
	if (nullptr != m_pTextureCom[TEXTURE_NOISE])
	{
		if (FAILED(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tParticleDesc.iTextureIndex[TEXTURE_NOISE])))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (SPRITE == m_tParticleDesc.eType)
	{
		_vector vCamDirection = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
		vCamDirection = XMVector4Normalize(vCamDirection);
		_float4 vCamDirectionFloat4 = {};
		XMStoreFloat4(&vCamDirectionFloat4, vCamDirection);

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat4, sizeof(_float4))))
			return E_FAIL;


		_float2 uvOffset = { (_float)(m_tSpriteDesc.iCurrentHor * m_tSpriteDesc.fAnimationSizeX) / m_tSpriteDesc.fSpriteSizeX, (_float)(m_tSpriteDesc.iCurrentVer * m_tSpriteDesc.fAnimationSizeY) / m_tSpriteDesc.fSpriteSizeY };
		_float2 uvScale = { (_float)m_tSpriteDesc.fAnimationSizeX / m_tSpriteDesc.fSpriteSizeX, (_float)m_tSpriteDesc.fAnimationSizeY / m_tSpriteDesc.fSpriteSizeY };

		if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &uvOffset, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &uvScale, sizeof(_float2))))
			return E_FAIL;


		if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
			return E_FAIL;
	}




	//if (FAILED(m_pShaderCom->Bind_RawValue("FX_Variables", &m_tVariables, sizeof(tagFX_Variables))))
	//	return E_FAIL;


	return S_OK;
}

CEffect_Particle* CEffect_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Particle* pInstance = new CEffect_Particle(pDevice, pContext);

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

void CEffect_Particle::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
}

