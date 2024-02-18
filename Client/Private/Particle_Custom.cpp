#include "stdafx.h"
#include "..\Public\Particle_Custom.h"

#include "GameInstance.h"

CParticle_Custom::CParticle_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAlphaObject(pDevice, pContext)
{

}

CParticle_Custom::CParticle_Custom(const CParticle_Custom& rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CParticle_Custom::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CParticle_Custom::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PARTICLE_CUSTOM_DESC* pDesc = (PARTICLE_CUSTOM_DESC*)pArg;

	for (_int i = 0; i < (_int)TYPE_END; i++)
	{
		m_tParticleDesc.strTextureTag[i] = pDesc->strTextureTag[i];
		m_tParticleDesc.iTextureIndex[i] = pDesc->iTextureIndex[i];
	}

	m_tParticleDesc.strShaderTag = pDesc->strShaderTag;
	m_tParticleDesc.iShaderPassIndex = pDesc->iShaderPassIndex;

	m_tParticleDesc.iRenderGroup = pDesc->iRenderGroup;
	m_tParticleDesc.iNumInstance = pDesc->iNumInstance;
	m_tParticleDesc.iMaxNumInstance = pDesc->iMaxNumInstance;

	m_tParticleDesc.fRotateUvDegree = pDesc->fRotateUvDegree;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Custom::Priority_Tick(_float fTimeDelta)
{



}

void CParticle_Custom::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		//if (GetKeyState(VK_LEFT) & 0x8000)
		//{
		//	//m_pTransformCom->Go_Left(fTimeDelta);
		//	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * -1.f);
		//}

		//_float4 vParticlePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		m_pVIBufferCom->Update(fTimeDelta);
		//m_pVIBufferCom->Update_Particle(fTimeDelta, FALSE);
		//Update_ParticlePosition(0, fTimeDelta);
	}
}

void CParticle_Custom::Late_Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		// CRenderer::RENDER_BLEND
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tParticleDesc.iRenderGroup), this)))
			return;
	}
}

HRESULT CParticle_Custom::Render()
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

_bool CParticle_Custom::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	for (_int i = 0; i < (_int)TYPE_END; i++)
		Out_Json["strTextureTag"][i] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]);

	Out_Json["strShaderTag"] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag);

	return true;
}

void CParticle_Custom::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	for (_int i = 0; i < (_int)TYPE_END; i++)
		m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]) = In_Json["strTextureTag"][i];

	m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag) = In_Json["strShaderTag"];

}


HRESULT CParticle_Custom::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, m_tParticleDesc.strShaderTag,
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

		tVIBufferDesc.fGravityAcc = { 0.f };
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

		if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &tVIBufferDesc)))
			return E_FAIL;
	}


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, m_tParticleDesc.strTextureTag[TYPE_DIFFUSE],
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_DIFFUSE]))))
		return E_FAIL;

	if (nullptr != m_pTextureCom[TYPE_MASK])
	{
		/* For.Com_Mask */
		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_tParticleDesc.strTextureTag[TYPE_MASK],
			TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK]))))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TYPE_NOISE])
	{
		/* For.Com_Noise */
		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_tParticleDesc.strTextureTag[TYPE_NOISE],
			TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_NOISE]))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CParticle_Custom::Bind_ShaderResources()
{
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

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tParticleDesc.iTextureIndex[TYPE_DIFFUSE])))
		return E_FAIL;

	if (nullptr != m_pTextureCom[TYPE_MASK])
	{
		if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tParticleDesc.iTextureIndex[TYPE_MASK])))
			return E_FAIL;
	}
	if (nullptr != m_pTextureCom[TYPE_NOISE])
	{
		if (FAILED(m_pTextureCom[TYPE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tParticleDesc.iTextureIndex[TYPE_NOISE])))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//_vector vCamDir = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamLook", &vCamDir, sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}

CParticle_Custom* CParticle_Custom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Custom* pInstance = new CParticle_Custom(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Custom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParticle_Custom::Clone(void* pArg)
{
	CParticle_Custom* pInstance = new CParticle_Custom(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Custom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle_Custom::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TYPE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
}

