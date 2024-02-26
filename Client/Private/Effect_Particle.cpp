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
	m_tParticleDesc = *(PARTICLE_DESC*)pArg;

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
	CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC* pDesc = m_pVIBufferCom->Get_Desc();

	if (m_tParticleDesc.bActive_Tool)
	{
		m_fSequenceTime = m_fLifeTime + m_fRemainTime;

		//pDesc->bActive_Tool = TRUE;
		pDesc->vMinMaxLifeTime.x = m_fWaitingTime;
		pDesc->vMinMaxLifeTime.y = m_fLifeTime;

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
			m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_fLifeTime);


			/* ======================= 라이프 타임 동작 시작 ======================= */



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
	else
	{
		m_tParticleDesc.bActive_Tool = FALSE;
	}
}

void CEffect_Particle::Late_Tick(_float fTimeDelta)
{
	if (m_tParticleDesc.bActive_Tool)
	{
		if (m_tParticleDesc.bRender)
		{
			Compute_CamDistance();

			// CRenderer::RENDER_BLEND
			if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tParticleDesc.iRenderGroup), this)))
				return;
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

	Out_Json["strProtoTag"] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strProtoTag);
	Out_Json["strPartTag"] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strPartTag);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		Out_Json["strTextureTag"][i] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]);
	}
		
	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		Out_Json["iTextureIndex"][i] = m_tParticleDesc.iTextureIndex[i];
	}
	
	Out_Json["iRenderGroup"] = m_tParticleDesc.iRenderGroup;
	Out_Json["strShaderTag"] = m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag);
	Out_Json["iShaderPassIndex"] = m_tParticleDesc.iShaderPassIndex;

	Out_Json["bBillBoard"] = m_tParticleDesc.bBillBoard;
	Out_Json["bDissolve"] = m_tParticleDesc.bDissolve;

	CJson_Utility::Write_Float2(Out_Json["vUV_Offset"], m_tParticleDesc.vUV_Offset);
	CJson_Utility::Write_Float2(Out_Json["vUV_Scale"], m_tParticleDesc.vUV_Scale);

	Out_Json["fUV_RotDegree"] = m_tParticleDesc.fUV_RotDegree;

	Out_Json["bUV_Wave"] = m_tParticleDesc.bUV_Wave;
	Out_Json["fUV_WaveSpeed"] = m_tParticleDesc.fUV_WaveSpeed;

	CJson_Utility::Write_Float4(Out_Json["vColor_Offset"], m_tParticleDesc.vColor_Offset);
	CJson_Utility::Write_Float4(Out_Json["vColor_Clip"], m_tParticleDesc.vColor_Clip);
	CJson_Utility::Write_Float4(Out_Json["vColor_Mul"], m_tParticleDesc.vColor_Mul);

	Out_Json["fBloom"] = m_tParticleDesc.fBloom;
	Out_Json["fRadial"] = m_tParticleDesc.fRadial;
	Out_Json["fDissolveAmount"] = m_tParticleDesc.fDissolveAmount;
	Out_Json["padding"] = m_tParticleDesc.padding;
	
	Out_Json["bActive_Tool"] = m_tParticleDesc.bActive_Tool;
	Out_Json["bPlay"] = m_tParticleDesc.bPlay;
	Out_Json["bLoop"] = m_tParticleDesc.bLoop;
	Out_Json["bReverse"] = m_tParticleDesc.bReverse;
	Out_Json["bRender"] = m_tParticleDesc.bRender;

	Out_Json["eType_Easing"] = m_tParticleDesc.eType_Easing;
	Out_Json["bParentPivot"] = m_tParticleDesc.bParentPivot;

	for (_int i = 0; i < 4; ++i)
		CJson_Utility::Write_Float4(Out_Json["matPivot"][i], XMLoadFloat4x4(&m_tParticleDesc.matPivot).r[i]);

	for (_int i = 0; i < 4; ++i)
		CJson_Utility::Write_Float4(Out_Json["matOffset"][i], XMLoadFloat4x4(&m_tParticleDesc.matOffset).r[i]);
		
	CJson_Utility::Write_Float3(Out_Json["vPosition_Start"], m_tParticleDesc.vPosition_Start);
	CJson_Utility::Write_Float3(Out_Json["vPosition_End"], m_tParticleDesc.vPosition_End);
	Out_Json["bPosition_Lerp"] = m_tParticleDesc.bPosition_Lerp;

	CJson_Utility::Write_Float3(Out_Json["vRotation_Start"], m_tParticleDesc.vRotation_Start);
	CJson_Utility::Write_Float3(Out_Json["vRotation_End"], m_tParticleDesc.vRotation_End);
	Out_Json["bRotation_Lerp"] = m_tParticleDesc.bRotation_Lerp;

	CJson_Utility::Write_Float3(Out_Json["vScaling_Start"], m_tParticleDesc.vScaling_Start);
	CJson_Utility::Write_Float3(Out_Json["vScaling_End"], m_tParticleDesc.vScaling_End);
	Out_Json["bScaling_Lerp"] = m_tParticleDesc.bScaling_Lerp;

	CJson_Utility::Write_Float3(Out_Json["vVelocity_Start"], m_tParticleDesc.vVelocity_Start);
	CJson_Utility::Write_Float3(Out_Json["vVelocity_End"], m_tParticleDesc.vVelocity_End);
	CJson_Utility::Write_Float3(Out_Json["vVelocity_Cur"], m_tParticleDesc.vVelocity_Cur);
	Out_Json["bVelocity_Lerp"] = m_tParticleDesc.bVelocity_Lerp;


	CJson_Utility::Write_Float4(Out_Json["vColor_Start"], m_tParticleDesc.vColor_Start);
	CJson_Utility::Write_Float4(Out_Json["vColor_End"], m_tParticleDesc.vColor_End);
	CJson_Utility::Write_Float4(Out_Json["vColor_Cur"], m_tParticleDesc.vColor_Cur);
	Out_Json["bColor_Lerp"] = m_tParticleDesc.bColor_Lerp;


	/* Particle */


	return true;
}

void CEffect_Particle::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strProtoTag) = In_Json["strProtoTag"];
	m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strPartTag) = In_Json["strPartTag"];

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strTextureTag[i]) = In_Json["strTextureTag"][i];
		m_tParticleDesc.iTextureIndex[i] = In_Json["iTextureIndex"][i];
	}

	m_tParticleDesc.iRenderGroup = In_Json["iRenderGroup"];
	m_pGameInstance->Convert_WString_To_String(m_tParticleDesc.strShaderTag) = In_Json["strShaderTag"];
	m_tParticleDesc.iShaderPassIndex = In_Json["iShaderPassIndex"];

	m_tParticleDesc.bBillBoard = In_Json["bBillBoard"];
	m_tParticleDesc.bDissolve = In_Json["bDissolve"];


	CJson_Utility::Load_Float2(In_Json["vUV_Offset"], m_tParticleDesc.vUV_Offset);
	CJson_Utility::Load_Float2(In_Json["vUV_Scale"], m_tParticleDesc.vUV_Scale);


	m_tParticleDesc.fUV_RotDegree = In_Json["fUV_RotDegree"];

	m_tParticleDesc.bUV_Wave = In_Json["bUV_Wave"];
	m_tParticleDesc.fUV_WaveSpeed = In_Json["fUV_WaveSpeed"];


	CJson_Utility::Load_Float4(In_Json["vColor_Offset"], m_tParticleDesc.vColor_Offset);
	CJson_Utility::Load_Float4(In_Json["vColor_Clip"], m_tParticleDesc.vColor_Clip);
	CJson_Utility::Load_Float4(In_Json["vColor_Mul"], m_tParticleDesc.vColor_Mul);

	m_tParticleDesc.fBloom = In_Json["fBloom"];
	m_tParticleDesc.fRadial = In_Json["fRadial"];
	m_tParticleDesc.fDissolveAmount = In_Json["fDissolveAmount"];
	m_tParticleDesc.padding = In_Json["padding"];


	m_tParticleDesc.bActive_Tool = In_Json["bActive_Tool"];
	m_tParticleDesc.bPlay = In_Json["bPlay"];
	m_tParticleDesc.bLoop = In_Json["bLoop"];
	m_tParticleDesc.bReverse = In_Json["bReverse"];
	m_tParticleDesc.bRender = In_Json["bRender"];

	m_tParticleDesc.eType_Easing = In_Json["eType_Easing"];

	m_tParticleDesc.bParentPivot = In_Json["bParentPivot"];

	_float4x4 matPivot;
	ZeroMemory(&matPivot, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["matPivot"], matPivot);

	_float4x4 matOffset;
	ZeroMemory(&matOffset, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["matOffset"], matOffset);


	CJson_Utility::Load_Float3(In_Json["vPosition_Start"], m_tParticleDesc.vPosition_Start);
	CJson_Utility::Load_Float3(In_Json["vPosition_End"], m_tParticleDesc.vPosition_End);

	m_tParticleDesc.bPosition_Lerp = In_Json["bPosition_Lerp"];

	CJson_Utility::Load_Float3(In_Json["vRotation_Start"], m_tParticleDesc.vRotation_Start);
	CJson_Utility::Load_Float3(In_Json["vRotation_End"], m_tParticleDesc.vRotation_End);
	m_tParticleDesc.bRotation_Lerp = In_Json["bRotation_Lerp"];

	CJson_Utility::Load_Float3(In_Json["vScaling_Start"], m_tParticleDesc.vScaling_Start);
	CJson_Utility::Load_Float3(In_Json["vScaling_End"], m_tParticleDesc.vScaling_End);
	m_tParticleDesc.bScaling_Lerp = In_Json["bScaling_Lerp"];

	CJson_Utility::Load_Float3(In_Json["vVelocity_Start"], m_tParticleDesc.vVelocity_Start);
	CJson_Utility::Load_Float3(In_Json["vVelocity_End"], m_tParticleDesc.vVelocity_End);
	CJson_Utility::Load_Float3(In_Json["vVelocity_Cur"], m_tParticleDesc.vVelocity_Cur);
	m_tParticleDesc.bVelocity_Lerp = In_Json["bVelocity_Lerp"];


	CJson_Utility::Load_Float4(In_Json["vColor_Start"], m_tParticleDesc.vColor_Start);
	CJson_Utility::Load_Float4(In_Json["vColor_End"], m_tParticleDesc.vColor_End);
	CJson_Utility::Load_Float4(In_Json["vColor_Cur"], m_tParticleDesc.vColor_Cur);
	 m_tParticleDesc.bColor_Lerp = In_Json["bColor_Lerp"];

}

void* CEffect_Particle::Get_BufferDesc()
{
	CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.eType_Action = m_tParticleDesc.eType_Action;
	tBufferDesc.eType_Fade = m_tParticleDesc.eType_Fade;
	tBufferDesc.eType_ColorLerp = m_tParticleDesc.eType_ColorLerp;

	tBufferDesc.bLoop = m_tParticleDesc.bLoop;
	tBufferDesc.bReverse = m_tParticleDesc.bReverse;
	tBufferDesc.bSpriteAnim = m_tParticleDesc.bSpriteAnim;

	tBufferDesc.iCurNumInstance = m_tParticleDesc.iCurNumInstance;
	
	tBufferDesc.vMinMaxLifeTime = m_tParticleDesc.vMinMaxLifeTime;

	tBufferDesc.vMinMaxRange = m_tParticleDesc.vMinMaxRange;
	tBufferDesc.vMinMaxRangeLength = m_tParticleDesc.vMinMaxRangeLength;
	tBufferDesc.vCenterPosition = m_tParticleDesc.vCenterPosition;

	tBufferDesc.vMinMaxSpeed = m_tParticleDesc.vMinMaxSpeed;

	tBufferDesc.fSpeedAcc = m_tParticleDesc.fSpeedAcc;
	tBufferDesc.fAccPosition = m_tParticleDesc.fAccPosition;


	tBufferDesc.bUseGravity = m_tParticleDesc.bUseGravity;
	tBufferDesc.fGravityAcc = m_tParticleDesc.fGravityAcc;
	tBufferDesc.fUseGravityPosition = m_tParticleDesc.fUseGravityPosition;


	tBufferDesc.vMinMaxRotationOffsetX = m_tParticleDesc.vMinMaxRotationOffsetX;
	tBufferDesc.vMinMaxRotationOffsetY = m_tParticleDesc.vMinMaxRotationOffsetY;
	tBufferDesc.vMinMaxRotationOffsetZ = m_tParticleDesc.vMinMaxRotationOffsetZ;
	tBufferDesc.vRotationOffset = m_tParticleDesc.vRotationOffset;


	tBufferDesc.vCurrentRotation = m_tParticleDesc.vCurrentRotation;
	tBufferDesc.vMinMaxRotationForce = m_tParticleDesc.vMinMaxRotationForce;


	tBufferDesc.vMinMaxScale = m_tParticleDesc.vMinMaxScale;
	tBufferDesc.vAddScale = m_tParticleDesc.vAddScale;
	tBufferDesc.vCurrentScale = m_tParticleDesc.vCurrentScale;


	tBufferDesc.vMinMaxRed = m_tParticleDesc.vMinMaxRed;
	tBufferDesc.vMinMaxGreen = m_tParticleDesc.vMinMaxGreen;
	tBufferDesc.vMinMaxBlue = m_tParticleDesc.vMinMaxBlue;
	tBufferDesc.vMinMaxAlpha = m_tParticleDesc.vMinMaxAlpha;

	tBufferDesc.vCurrentColor = m_tParticleDesc.vCurrentColor;


	/* SpriteDesc */
	tBufferDesc.fSequenceTerm = m_tSpriteDesc.fSequenceTerm;

	tBufferDesc.vTextureSize = m_tSpriteDesc.vTextureSize;
	tBufferDesc.vTileSize = m_tSpriteDesc.vTileSize;

	tBufferDesc.vUV_CurTileIndex = m_tSpriteDesc.vUV_CurTileIndex;
	tBufferDesc.vUV_MinTileCount = m_tSpriteDesc.vUV_MinTileCount;
	tBufferDesc.vUV_MaxTileCount = m_tSpriteDesc.vUV_MaxTileCount;


	return &tBufferDesc;
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
		CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC tBufferInfo = *static_cast<CVIBuffer_Particle_Point::PARTICLE_BUFFER_DESC*>(Get_BufferDesc());
		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Particle_Point"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo)))
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

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_splineMesh_tornado"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	//CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC Desc;
	//Desc.pModel = m_pModelCom;
	//Desc.iNumInstance = 1;

	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"),
	//	TEXT("Com_VIBuffer_Model"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Model), &Desc)))
	//	return E_FAIL;


	return S_OK;
}


HRESULT CEffect_Particle::Bind_ShaderResources()
{
	//if (FAILED(__super::Bind_ShaderResources()))
	//	return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tParticleDesc.fUV_RotDegree, sizeof(_float))))
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

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_DiscardValue", &m_tParticleDesc.vColor_Clip.w, sizeof(_float)));


	//if (SPRITE == m_tParticleDesc.eType)
	//{
	//	_vector vCamDirection = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
	//	vCamDirection = XMVector4Normalize(vCamDirection);
	//	_float4 vCamDirectionFloat4 = {};
	//	XMStoreFloat4(&vCamDirectionFloat4, vCamDirection);

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat4, sizeof(_float4))))
	//		return E_FAIL;


	//	m_tParticleDesc.vUV_Offset = { (_float)(m_tSpriteDesc.vUV_CurTileIndex.x * m_tSpriteDesc.vTileSize.x) / m_tSpriteDesc.vTextureSize.x
	//					, (_float)(m_tSpriteDesc.vUV_CurTileIndex.y * m_tSpriteDesc.vTileSize.y) / m_tSpriteDesc.vTextureSize.y };

	//	m_tParticleDesc.vUV_Scale = { (_float)m_tSpriteDesc.vTileSize.x / m_tSpriteDesc.vTextureSize.x
	//							, (_float)m_tSpriteDesc.vTileSize.y / m_tSpriteDesc.vTextureSize.y };


	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tParticleDesc.vUV_Offset, sizeof(_float2))))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tParticleDesc.vUV_Scale, sizeof(_float2))))
	//		return E_FAIL;
	//}




	//if (FAILED(m_pShaderCom->Bind_RawValue("FX_Variables", &m_tVariables, sizeof(tagFX_Variables))))
	//	return E_FAIL;


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

