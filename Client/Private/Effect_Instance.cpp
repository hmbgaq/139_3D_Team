#include "stdafx.h"
#include "Effect_Instance.h"

#include "GameInstance.h"
#include "VIBuffer_Effect_Model_Instance.h"

#include "Easing_Utillity.h"
#include "SMath.h"

CEffect_Instance::CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = TRUE;
}

CEffect_Instance::CEffect_Instance(const CEffect_Instance & rhs)
	: CEffect_Void(rhs)
{
}

HRESULT CEffect_Instance::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CEffect_Instance::Initialize(void* pArg)
{	
	//*static_cast<EFFECTVOID_DESC*>(&m_tInstanceDesc) = *static_cast<EFFECTVOID_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CEffect_Instance::Priority_Tick(_float fTimeDelta)
{

}

void CEffect_Instance::Tick(_float fTimeDelta)
{

	if (m_tVoidDesc.bActive_Tool)
	{
		m_tVoidDesc.fSequenceTime = m_tVoidDesc.fLifeTime + m_tVoidDesc.fRemainTime;

		if (m_tVoidDesc.bPlay)
		{
			m_tVoidDesc.fSequenceAcc += fTimeDelta;

			// 시작지연 누적시간이 지나면 렌더 시작(이 이펙트 시작)
			if (m_tVoidDesc.fWaitingAcc <= m_tVoidDesc.fWaitingTime)
			{
				m_tVoidDesc.fWaitingAcc += fTimeDelta;

				if (m_tVoidDesc.fWaitingAcc >= m_tVoidDesc.fWaitingTime)
				{
					m_tVoidDesc.bRender = TRUE;
				}
				else
					return;
			}

			// 라이프타임 누적 시작
			m_tVoidDesc.fTimeAcc += fTimeDelta;
			m_tVoidDesc.fLifeTimeRatio = min(1.0f, m_tVoidDesc.fTimeAcc / m_tVoidDesc.fLifeTime);

			/* ======================= 라이프 타임 동작 시작 ======================= */




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

				// 이 이펙트의 타임라인 종료
				if (m_tVoidDesc.fRemainAcc >= m_tVoidDesc.fRemainTime)
				{
					m_tVoidDesc.fDissolveAmount = 1.f;
					m_tVoidDesc.bRender = TRUE;
					return;
				}
			}

			if (m_tVoidDesc.bRender)
			{
				if (CVIBuffer_Effect_Model_Instance::MODE_PARTICLE == m_pVIBufferCom->Get_Desc()->eType_Mode)
				{
					m_pVIBufferCom->Update_Particle(fTimeDelta);
				}
				else
				{
					m_pVIBufferCom->Update(fTimeDelta);
				}
				
			}
		}
	}


}

void CEffect_Instance::Late_Tick(_float fTimeDelta)
{
	if (m_tVoidDesc.bActive_Tool)
	{
		if (m_tVoidDesc.bRender)
		{
			__super::Update_PivotMat();

			Compute_CamDistance();

			//FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT, this));
			FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tVoidDesc.iRenderGroup, this), );
		}
	}
}

HRESULT CEffect_Instance::Render()
{
	FAILED_CHECK(Bind_ShaderResources());


	if (m_pVIBufferCom->Get_Desc()->bMorph)	// 모프가 true이면 (박쥐 모델)
	{
		_uint	iCurModelNum = m_pVIBufferCom->Get_Desc()->eCurModelNum;

		if (FALSE == m_tInstanceDesc.bUseCustomTex)
		{
			m_pModelCom[iCurModelNum]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)0, aiTextureType_DIFFUSE);
		}
	
		m_pShaderCom->Begin(m_tVoidDesc.iShaderPassIndex);
		m_pVIBufferCom->Render((_uint)0);

		return S_OK;
	}
	else
	{
		//_uint	iNumMeshes = m_pModelCom[0]->Get_NumMeshes();

		//for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FALSE == m_tInstanceDesc.bUseCustomTex)
			{
				m_pModelCom[0]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)0, aiTextureType_DIFFUSE);
				//m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
			}

			m_pShaderCom->Begin(m_tVoidDesc.iShaderPassIndex);
			m_pVIBufferCom->Render((_uint)0);
		}
	}



	return S_OK;
}

void CEffect_Instance::ReSet_Effect()
{
	__super::ReSet_Effect();	// 시간 초기화

	m_tVoidDesc.fDissolveAmount = 0.f;
	m_tVoidDesc.bDissolve = FALSE;

	if (!m_pVIBufferCom->Get_Desc()->bRecycle)
	{
		m_pVIBufferCom->ReSet();
	}

}

void CEffect_Instance::End_Effect()
{
	__super::End_Effect();

	if (m_tVoidDesc.bLoop)
	{
		ReSet_Effect();
	}
}

HRESULT CEffect_Instance::Change_TextureCom(wstring strProtoTextureTag)
{
	_uint iCurLevel = m_pGameInstance->Get_CurrentLevel();

	wstring strDiffuse	= TEXT("Diffuse");
	wstring strNormal	= TEXT("Normal");
	wstring strMask		= TEXT("Mask");
	wstring strNoise	= TEXT("Noise");
	wstring strSprite	= TEXT("Sprite");

	CEffect_Void::TEXTURE eTexture = TEXTURE_END;

	if (strProtoTextureTag.find(strDiffuse) != string::npos)	// 문자열 찾음
	{
		// 디퓨즈 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 디퓨즈폴더 -> 피 디퓨즈폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
		{
			Remove_Component(TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]));
		}

		eTexture = TEXTURE_DIFFUSE;
		FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));
	}
	else if (strProtoTextureTag.find(strNormal) != string::npos)
	{
		// 노말 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 노말폴더 -> 피 노말폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_NORAML])
		{
			Remove_Component(TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML]));
		}
		eTexture = TEXTURE_NORAML;
		FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML])));
	}
	else if (strProtoTextureTag.find(strMask) != string::npos)
	{
		// 마스크 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 마스크폴더 -> 연기 마스크폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_MASK])
		{
			Remove_Component(TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]));
		}
		eTexture = TEXTURE_MASK;
		FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK])));
	}
	else if (strProtoTextureTag.find(strNoise) != string::npos)
	{
		// 노이즈 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 노이즈폴더 -> 불 노이즈폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_NOISE])
		{
			Remove_Component(TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]));
		}

		eTexture = TEXTURE_NOISE;
		FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE])));
	}
	else if (strProtoTextureTag.find(strSprite) != string::npos)
	{
		// 스프라이트 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 스프라이트폴더 -> 연기 스프라이트폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_SPRITE])
		{
			Remove_Component(TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE]));
		}
		eTexture = TEXTURE_SPRITE;
		FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));
	}

	m_tVoidDesc.strTextureTag[eTexture] = strProtoTextureTag;
	m_tVoidDesc.iTextureIndex[eTexture] = 0;

	return S_OK;
}


HRESULT CEffect_Instance::Remove_TextureCom(TEXTURE eTexture)
{
	wstring strTexureComTag = TEXT("");

	if(TEXTURE_DIFFUSE == eTexture)
		strTexureComTag = TEXT("Com_Diffuse");
	else if(TEXTURE_NORAML)
		strTexureComTag = TEXT("Com_Normal");
	else if (TEXTURE_MASK)
		strTexureComTag = TEXT("Com_Mask");
	else if (TEXTURE_NOISE)
		strTexureComTag = TEXT("Com_Noise");
	else if (TEXTURE_SPRITE)
		strTexureComTag = TEXT("Com_Sprite");


	m_tVoidDesc.strTextureTag[eTexture] = TEXT("");
	m_tVoidDesc.iTextureIndex[eTexture] = 0;

	return Remove_Component(strTexureComTag, reinterpret_cast<CComponent**>(&m_pTextureCom[eTexture]));
}

_bool CEffect_Instance::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);


	/* Mesh */
	Out_Json["bUseCustomTex"] = m_tInstanceDesc.bUseCustomTex;


	/* Distortion */
	Out_Json["eType_Scroll"] = m_tDistortionDesc.eType_Scroll;

	CJson_Utility::Write_Float3(Out_Json["vScrollSpeeds"], m_tDistortionDesc.vScrollSpeeds);
	CJson_Utility::Write_Float3(Out_Json["vScales"], m_tDistortionDesc.vScales);
	CJson_Utility::Write_Float2(Out_Json["vDistortion1"], m_tDistortionDesc.vDistortion1);
	CJson_Utility::Write_Float2(Out_Json["vDistortion2"], m_tDistortionDesc.vDistortion2);
	CJson_Utility::Write_Float2(Out_Json["vDistortion3"], m_tDistortionDesc.vDistortion3);

	Out_Json["fDistortionScale"] = m_tDistortionDesc.fDistortionScale;
	Out_Json["fDistortionBias"] = m_tDistortionDesc.fDistortionBias;


	return true;
}

void CEffect_Instance::Load_FromJson(const json& In_Json)
{

	__super::Load_FromJson(In_Json);

	/* Mesh */
	m_tInstanceDesc.bUseCustomTex	= In_Json["bUseCustomTex"];


	/* Distortion */
	m_tDistortionDesc.eType_Scroll = In_Json["eType_Scroll"];

	CJson_Utility::Load_Float3(In_Json["vScrollSpeeds"], m_tDistortionDesc.vScrollSpeeds);
	CJson_Utility::Load_Float3(In_Json["vScales"], m_tDistortionDesc.vScales);
	CJson_Utility::Load_Float2(In_Json["vDistortion1"], m_tDistortionDesc.vDistortion1);
	CJson_Utility::Load_Float2(In_Json["vDistortion2"], m_tDistortionDesc.vDistortion2);
	CJson_Utility::Load_Float2(In_Json["vDistortion3"], m_tDistortionDesc.vDistortion3);

	m_tDistortionDesc.fDistortionScale = In_Json["fDistortionScale"];
	m_tDistortionDesc.fDistortionBias = In_Json["fDistortionBias"];

}



HRESULT CEffect_Instance::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Effect_Model_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Model */
	{
		if (FAILED(__super::Add_Component(iNextLevel, m_tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_01],
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom[CVIBuffer_Effect_Model_Instance::MORPH_01]))))
			return E_FAIL;

		if (TEXT("") != m_tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_02])
		{
			if (FAILED(__super::Add_Component(iNextLevel, m_tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_02],
				TEXT("Com_Model_Morph"), reinterpret_cast<CComponent**>(&m_pModelCom[CVIBuffer_Effect_Model_Instance::MORPH_02]))))
				return E_FAIL;
		}
	}


	/* For.Com_VIBuffer */
	{
		CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC tBufferInfo = {};

		for (_int i = 0; i < ECast(CVIBuffer_Effect_Model_Instance::MODE_END); ++i)
		{
			if (nullptr != m_pModelCom[i])
			{
				tBufferInfo.pModel[i] = m_pModelCom[i];
			}		
		}

		if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo)))
			return E_FAIL;
	}


	/* For.Com_Texture */
	{
		// Diffuse
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_DIFFUSE])
			FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_DIFFUSE], TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));


		// Normal
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_NORAML])
			FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_NORAML], TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML])));

		// Mask
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_MASK])
			FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_MASK], TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK])));

		// Noise
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_NOISE])
			FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_NOISE], TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE])));

		// Sprite
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_SPRITE])
			FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_SPRITE], TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));
	}


	return S_OK;
}

HRESULT CEffect_Instance::Bind_ShaderResources()
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


	if (TRUE == m_tInstanceDesc.bUseCustomTex)	// 텍스처를 내가 정해줄거면 
	{
		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		//m_pGameInstance->Find_RenderTarget(TEXT("MRT_Deferred"))->Get_Texture2D()->GetDesc();
		//TextureDesc.Format

		// 기본은 디퓨즈만 바인드
		if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])	// 디퓨즈 텍스처 있으면 바인드
			FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_DIFFUSE]));

		if (nullptr != m_pTextureCom[TEXTURE_NORAML])	// 노말 텍스처 있으면 바인드
			FAILED_CHECK(m_pTextureCom[TEXTURE_NORAML]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NORAML]));

		if (nullptr != m_pTextureCom[TEXTURE_MASK])		// 마스크 텍스처 있으면 바인드
			FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tVoidDesc.iTextureIndex[TEXTURE_MASK]));

		if (nullptr != m_pTextureCom[TEXTURE_NOISE])	// 노이즈 텍스처 있으면 바인드
			FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NOISE]));

	}

	/* UV ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tVoidDesc.vUV_Offset, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tVoidDesc.vUV_Scale, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tVoidDesc.fUV_RotDegree, sizeof(_float)));


	/* Color & Discard ===============================================================================*/
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vColor_Mul", &m_tVoidDesc.vColor_Mul, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tVoidDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = _float3(m_tVoidDesc.vColor_Clip.x, m_tVoidDesc.vColor_Clip.y, m_tVoidDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));

	/* Camera ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	_float3 vCamDirectionFloat3 = m_pGameInstance->Get_CamDirection();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat3, sizeof(_float3)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));


	/* Dissolve */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_tVoidDesc.fDissolveAmount, sizeof(_float)));


	/* Distortion */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fFrameTime", &m_tVoidDesc.fTimeAcc, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_iScrollType", &m_tDistortionDesc.eType_Scroll, sizeof(_int)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vScrollSpeeds", &m_tDistortionDesc.vScrollSpeeds, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vScales", &m_tDistortionDesc.vScales, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion1", &m_tDistortionDesc.vDistortion1, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion2", &m_tDistortionDesc.vDistortion2, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion3", &m_tDistortionDesc.vDistortion3, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDistortionScale", &m_tDistortionDesc.fDistortionScale, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDistortionBias", &m_tDistortionDesc.fDistortionBias, sizeof(_float)));


	/* 소영 추가사항 - Bloom , Rim 용도 ====== */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_tVoidDesc.vBloomPower, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_tVoidDesc.vRimColor, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_tVoidDesc.fRimPower, sizeof(_float)));

	return S_OK;
}

CEffect_Instance * CEffect_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CEffect_Instance*		pInstance = new CEffect_Instance(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Instance::Clone(void* pArg)
{
	CEffect_Instance*		pInstance = new CEffect_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Instance::Pool()
{
	return new CEffect_Instance(*this);
}

void CEffect_Instance::Free()
{
	__super::Free();
	
	for (_int i = 0; i < ECast(CVIBuffer_Effect_Model_Instance::MORPH_END); i++)
	{
		if (nullptr != m_pModelCom[i])
		{
			Safe_Release(m_pModelCom[i]);
		}	
	}
		
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

}



