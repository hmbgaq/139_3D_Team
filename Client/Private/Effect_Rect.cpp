#include "stdafx.h"
#include "..\Public\Effect_Rect.h"

#include "GameInstance.h"


CEffect_Rect::CEffect_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = FALSE;
}

CEffect_Rect::CEffect_Rect(const CEffect_Rect & rhs)
	: CEffect_Void(rhs)
{
}

HRESULT CEffect_Rect::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CEffect_Rect::Initialize(void* pArg)
{	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	// void* pArg : 복제하여 객체를 생성할때 원형에서 채우지 못한 데이터를 추가적으로 더 초기화해주기위해서. 
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_Scaling(0.5f, 1.f, 1.f);


	return S_OK;
}

void CEffect_Rect::Priority_Tick(_float fTimeDelta)
{

	
}

void CEffect_Rect::Tick(_float fTimeDelta)
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

			if (m_tVoidDesc.bUseSpriteAnim)
			{
				if (!m_tSpriteDesc.bSpriteFinish)
				{
					if (m_tVoidDesc.fSpriteTimeAcc > m_tSpriteDesc.fSequenceTerm)
					{
						(m_tSpriteDesc.vUV_CurTileIndex.x)++;	// 가로 인덱스 증가

						if (m_tSpriteDesc.vUV_CurTileIndex.x == m_tSpriteDesc.vUV_MaxTileCount.x)
						{
							(m_tSpriteDesc.vUV_CurTileIndex.y)++;											// 세로 인덱스 증가
							m_tSpriteDesc.vUV_CurTileIndex.x = m_tSpriteDesc.vUV_MinTileCount.x;			// 가로 인덱스 초기화

							if (m_tSpriteDesc.vUV_CurTileIndex.y == m_tSpriteDesc.vUV_MaxTileCount.y)		// 세로 인덱스도 끝까지 왔다면
							{
								m_tSpriteDesc.bSpriteFinish = TRUE;											// 스프라이트 애님 끝	
							}
						}
						m_tVoidDesc.fSpriteTimeAcc = 0.f;	// 시간 초기화
					}
				}

				if (m_tSpriteDesc.bSpriteFinish)
				{
					// 스프라이트 재생이 끝났고,
					if (m_tSpriteDesc.bLoop)	// 스프라이트의 루프가 true이면
					{
						// 스프라이트 초기화
						m_tSpriteDesc.Reset_Sprite();
					}
					else
					{
						// 아니면 렌더 끄기
						m_tVoidDesc.bRender = FALSE;
						//m_tSpriteDesc.Reset_Sprite(); // 초기화
					}
				}
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
				// 디졸브 끝


				// 이 이펙트의 타임라인 종료
				if (m_tVoidDesc.fRemainAcc >= m_tVoidDesc.fRemainTime)
				{
					m_tVoidDesc.fDissolveAmount = 1.f;
					m_tVoidDesc.bRender = TRUE;
					return;
				}
			}
		}
	}

};

void CEffect_Rect::Late_Tick(_float fTimeDelta)
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

HRESULT CEffect_Rect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 쉐이더에 0번째 패스로 그릴거야. */
	m_pShaderCom->Begin(m_tVoidDesc.iShaderPassIndex);

	/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

_bool CEffect_Rect::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);


	/* Sprite Desc */
	Out_Json["bLoop"] = m_tSpriteDesc.bLoop;
	Out_Json["fSequenceTerm"] = m_tSpriteDesc.fSequenceTerm;

	CJson_Utility::Write_Float2(Out_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Write_Float2(Out_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Write_Float2(Out_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Write_Float2(Out_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);


	/* Distortion */
	Out_Json["fSequenceTerm"] = m_tDistortionDesc.fSequenceTerm;

	CJson_Utility::Write_Float3(Out_Json["vScrollSpeeds"], m_tDistortionDesc.vScrollSpeeds);
	CJson_Utility::Write_Float3(Out_Json["vScales"], m_tDistortionDesc.vScales);
	CJson_Utility::Write_Float2(Out_Json["vDistortion1"], m_tDistortionDesc.vDistortion1);
	CJson_Utility::Write_Float2(Out_Json["vDistortion2"], m_tDistortionDesc.vDistortion2);
	CJson_Utility::Write_Float2(Out_Json["vDistortion3"], m_tDistortionDesc.vDistortion3);

	Out_Json["fDistortionScale"] = m_tDistortionDesc.fDistortionScale;
	Out_Json["fDistortionBias"] = m_tDistortionDesc.fDistortionBias;

	return true;
}

void CEffect_Rect::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);


	/* Sprite Desc */
	m_tSpriteDesc.bLoop = In_Json["bLoop"];
	m_tSpriteDesc.fSequenceTerm = In_Json["fSequenceTerm"];

	CJson_Utility::Load_Float2(In_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Load_Float2(In_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Load_Float2(In_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Load_Float2(In_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);


	/* Distortion */
	m_tDistortionDesc.fSequenceTerm = In_Json["fSequenceTerm"];

	CJson_Utility::Load_Float3(In_Json["vScrollSpeeds"], m_tDistortionDesc.vScrollSpeeds);
	CJson_Utility::Load_Float3(In_Json["vScales"], m_tDistortionDesc.vScales);
	CJson_Utility::Load_Float2(In_Json["vDistortion1"], m_tDistortionDesc.vDistortion1);
	CJson_Utility::Load_Float2(In_Json["vDistortion2"], m_tDistortionDesc.vDistortion2);
	CJson_Utility::Load_Float2(In_Json["vDistortion3"], m_tDistortionDesc.vDistortion3);

	m_tDistortionDesc.fDistortionScale = In_Json["fDistortionScale"];
	m_tDistortionDesc.fDistortionBias = In_Json["fDistortionBias"];

}

void CEffect_Rect::ReSet_Effect()
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

}

void CEffect_Rect::End_Effect()
{
	__super::End_Effect();

	if (m_tVoidDesc.bLoop)
	{
		ReSet_Effect();
	}
}

HRESULT CEffect_Rect::Change_TextureCom(wstring strProtoTextureTag)
{
	_uint iCurLevel = m_pGameInstance->Get_CurrentLevel();

	wstring strDiffuse = TEXT("Diffuse");
	wstring strNormal = TEXT("Normal");
	wstring strMask = TEXT("Mask");
	wstring strNoise = TEXT("Noise");
	wstring strSprite = TEXT("Sprite");


	if (strProtoTextureTag.find(strDiffuse) != string::npos)	// 문자열 찾음
	{
		// 디퓨즈 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 디퓨즈폴더 -> 피 디퓨즈폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
		{
			Remove_Component(TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]));
			FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));
		}
	}
	else if (strProtoTextureTag.find(strNormal) != string::npos)
	{
		// 노말 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 노말폴더 -> 피 노말폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_NORAML])
		{
			Remove_Component(TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML]));
			FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML])));
		}
	}
	else if (strProtoTextureTag.find(strMask) != string::npos)
	{
		// 마스크 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 마스크폴더 -> 연기 마스크폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_MASK])
		{
			Remove_Component(TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]));
			FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK])));
		}
	}
	else if (strProtoTextureTag.find(strNoise) != string::npos)
	{
		// 노이즈 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 노이즈폴더 -> 불 노이즈폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_NOISE])
		{
			Remove_Component(TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]));
			FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE])));
		}
	}
	else if (strProtoTextureTag.find(strSprite) != string::npos)
	{
		// 스프라이트 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 스프라이트폴더 -> 연기 스프라이트폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_SPRITE])
		{
			Remove_Component(TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE]));
			FAILED_CHECK(__super::Add_Component(iCurLevel, strProtoTextureTag, TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));
		}
	}


	return S_OK;
}


HRESULT CEffect_Rect::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_EffectTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;



	/* For.Com_Texture */
	{
		// Diffuse
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

HRESULT CEffect_Rect::Bind_ShaderResources()
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
		// 스프라이트 사용이고, 
		if (nullptr != m_pTextureCom[TEXTURE_SPRITE])	// 스프라이트 텍스처 있으면 바인드
			FAILED_CHECK(m_pTextureCom[TEXTURE_SPRITE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_SPRITE]));
	}
	else
	{
		// 기본은 디퓨즈만 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_DIFFUSE]));
	}

	if (nullptr != m_pTextureCom[TEXTURE_NORAML])	// 노말 텍스처 있으면 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_NORAML]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NORAML]));

	if (nullptr != m_pTextureCom[TEXTURE_MASK])		// 마스크 텍스처 있으면 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tVoidDesc.iTextureIndex[TEXTURE_MASK]));

	if (nullptr != m_pTextureCom[TEXTURE_NOISE])	// 노이즈 텍스처 있으면 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NOISE]));



	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bBillBoard", &m_tVoidDesc.bBillBoard, sizeof(_bool)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tVoidDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = _float3(m_tVoidDesc.vColor_Clip.x, m_tVoidDesc.vColor_Clip.y, m_tVoidDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vColor_Mul", &m_tVoidDesc.vColor_Mul, sizeof(_float4)));


	/* Camera ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	_float3 vCamDirectionFloat3 = m_pGameInstance->Get_CamDirection();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat3, sizeof(_float3)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));



	/* Sprite =========================================================================================*/
	if (m_tVoidDesc.bUseSpriteAnim)
	{
		m_tVoidDesc.vUV_Offset = { (_float)(m_tSpriteDesc.vUV_CurTileIndex.x * m_tSpriteDesc.vTileSize.x) / m_tSpriteDesc.vTextureSize.x
									, (_float)(m_tSpriteDesc.vUV_CurTileIndex.y * m_tSpriteDesc.vTileSize.y) / m_tSpriteDesc.vTextureSize.y };

		m_tVoidDesc.vUV_Scale = { (_float)m_tSpriteDesc.vTileSize.x / m_tSpriteDesc.vTextureSize.x
								, (_float)m_tSpriteDesc.vTileSize.y / m_tSpriteDesc.vTextureSize.y };
	}


	/* UV ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tVoidDesc.vUV_Offset, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tVoidDesc.vUV_Scale, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tVoidDesc.fUV_RotDegree, sizeof(_float)));

	/* Distortion */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fFrameTime", &m_tVoidDesc.fTimeAcc, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vScrollSpeeds", &m_tDistortionDesc.vScrollSpeeds, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vScales", &m_tDistortionDesc.vScales, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion1", &m_tDistortionDesc.vDistortion1, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion2", &m_tDistortionDesc.vDistortion2, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion3", &m_tDistortionDesc.vDistortion3, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDistortionScale", &m_tDistortionDesc.fDistortionScale, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDistortionBias", &m_tDistortionDesc.fDistortionBias, sizeof(_float)));


	/* Rim & Bloom ====================================================================================== */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_tVoidDesc.vBloomPower, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_tVoidDesc.vRimColor, sizeof(_float4)));


	/* ETC ============================================================================================ */
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"));

	return S_OK;
}

CEffect_Rect * CEffect_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEffect_Rect*		pInstance = new CEffect_Rect(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Rect::Clone(void* pArg)
{
	CEffect_Rect*		pInstance = new CEffect_Rect(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Rect::Pool()
{
	return new CEffect_Rect(*this);
}

void CEffect_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pShaderCom);
}

