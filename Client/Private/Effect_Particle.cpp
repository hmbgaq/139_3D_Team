#include "stdafx.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"

#include "Effect.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = false;
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
						m_tVoidDesc.bRender = true;
						//pDesc->bPlay	= true;
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
									m_tSpriteDesc.bSpriteFinish = true;										// 스프라이트 애님 끝	
								}
							}
							m_tVoidDesc.fSpriteTimeAcc = 0.f;	// 시간 초기화
						}
					}

					if (m_tSpriteDesc.bSpriteFinish)
					{
						// 스프라이트 재생이 끝났고,
						if (m_tSpriteDesc.bSpriteLoop)	// 스프라이트의 루프가 true이면
						{	
							// 스프라이트 초기화
							m_tSpriteDesc.Reset_Sprite();
						}
						else
						{
							// 아니면 렌더 끄기
							m_tVoidDesc.bRender = false;
							//m_tSpriteDesc.Reset_Sprite(); // 초기화
						}				
					}
				}


				if (m_tVoidDesc.bRender) // 파티클 버퍼 업데이트
				{
					m_pVIBufferCom->Update(fTimeDelta);
				}

				/* ======================= 라이프 타임 동작 끝  ======================= */

				if (m_tVoidDesc.fTimeAcc >= m_tVoidDesc.fLifeTime)
				{
					// 삭제 대기시간 누적 시작
					m_tVoidDesc.fRemainAcc += fTimeDelta;

					// 디졸브 시작
					m_tVoidDesc.bDissolve = true;
					if (m_tVoidDesc.bDissolve)
					{
						m_tVoidDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_tVoidDesc.fRemainAcc, m_tVoidDesc.fRemainTime, m_tVoidDesc.eType_Easing);
					}

					if (m_tVoidDesc.fRemainAcc >= m_tVoidDesc.fRemainTime)
					{
						// 파티클 끝
						m_tVoidDesc.fDissolveAmount = 1.f;
						m_pVIBufferCom->Set_Finish(true);
						//m_tVoidDesc.bRender = true;
						return;
					}
				}


			}

#ifdef _DEBUG
	}
#endif // _DEBUG

}

void CEffect_Particle::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	if (m_tVoidDesc.bActive_Tool)
	{
#endif // _DEBUG
		if (m_tVoidDesc.bRender)
		{
			__super::Update_PivotMat();

			//if (m_bSortZ)
			//{
			//	//m_pVIBufferCom->Sort_Z(m_pVIBufferCom->Get_NumInstance());
			//}

			FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tVoidDesc.iRenderGroup, this), );
		}
#ifdef _DEBUG
	}
#endif // _DEBUG

}

HRESULT CEffect_Particle::Render()
{

#ifdef _DEBUG
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
#endif // _DEBUG


	return S_OK;
}

void CEffect_Particle::ReSet_Effect()
{
	__super::ReSet_Effect();

	m_tVoidDesc.fDissolveAmount = 0.f;
	m_tVoidDesc.bDissolve = false;
	

	if (m_tVoidDesc.bUseSpriteAnim)
	{
		m_tSpriteDesc.bSpriteFinish = false;
		m_tSpriteDesc.vUV_CurTileIndex.y = m_tSpriteDesc.vUV_MinTileCount.y;
		m_tSpriteDesc.vUV_CurTileIndex.x = m_tSpriteDesc.vUV_MinTileCount.x;
	}

	if (!m_pVIBufferCom->Get_Desc()->bRecycle)	// 파티클 버퍼가 재사용이 아닐때만 리셋
	{
		//m_tVoidDesc.bRender = false;
		m_pVIBufferCom->ReSet(); // 버퍼 리셋
	}

}

void CEffect_Particle::Init_ReSet_Effect()
{
	__super::ReSet_Effect();

	m_tVoidDesc.fDissolveAmount = 0.f;
	m_tVoidDesc.bDissolve = false;


	if (m_tVoidDesc.bUseSpriteAnim)
	{
		m_tSpriteDesc.bSpriteFinish = false;
		m_tSpriteDesc.vUV_CurTileIndex.y = m_tSpriteDesc.vUV_MinTileCount.y;
		m_tSpriteDesc.vUV_CurTileIndex.x = m_tSpriteDesc.vUV_MinTileCount.x;
	}


	m_tVoidDesc.bRender = false;
	m_pVIBufferCom->ReSet(); // 버퍼 리셋

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

	/* m_tParticleDesc */


	/* Sprite Desc */
	Out_Json["bSpriteLoop"] = m_tSpriteDesc.bSpriteLoop;
	Out_Json["fSequenceTerm"] = m_tSpriteDesc.fSequenceTerm;

	CJson_Utility::Write_Float2(Out_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Write_Float2(Out_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Write_Float2(Out_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Write_Float2(Out_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);


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

void CEffect_Particle::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);


	//*&m_tParticleDesc = *static_cast<PARTICLE_DESC*>(Load_VoidDesc(In_Json));
	//*static_cast<EFFECTVOID_DESC*>(&m_tParticleDesc) = *static_cast<EFFECTVOID_DESC*>(Load_VoidDesc(In_Json));
	//static_cast<EFFECTVOID_DESC>(m_tParticleDesc) = Load_VoidDesc(In_Json); // 왜???????????????????????????


	/* m_tParticleDesc */


	/* Sprite Desc */
	if (In_Json.contains("bSpriteLoop")) // 다시 저장 후 if문 삭제
	{
		m_tSpriteDesc.bSpriteLoop = In_Json["bSpriteLoop"];
	}
	else
	{
		m_tSpriteDesc.bSpriteLoop = In_Json["bLoop"];
	}
	m_tSpriteDesc.fSequenceTerm = In_Json["fSequenceTerm"];

	CJson_Utility::Load_Float2(In_Json["vTextureSize"], m_tSpriteDesc.vTextureSize);
	CJson_Utility::Load_Float2(In_Json["vTileSize"], m_tSpriteDesc.vTileSize);

	CJson_Utility::Load_Float2(In_Json["vUV_MinTileCount"], m_tSpriteDesc.vUV_MinTileCount);
	CJson_Utility::Load_Float2(In_Json["vUV_MaxTileCount"], m_tSpriteDesc.vUV_MaxTileCount);


	/* Distortion */
	if (In_Json.contains("eType_Scroll")) // 디스토션 정보가 있으면 읽기 (다시 저장 후 if문 삭제)
	{
		m_tDistortionDesc.eType_Scroll = In_Json["eType_Scroll"];

		CJson_Utility::Load_Float3(In_Json["vScrollSpeeds"], m_tDistortionDesc.vScrollSpeeds);
		CJson_Utility::Load_Float3(In_Json["vScales"], m_tDistortionDesc.vScales);
		CJson_Utility::Load_Float2(In_Json["vDistortion1"], m_tDistortionDesc.vDistortion1);
		CJson_Utility::Load_Float2(In_Json["vDistortion2"], m_tDistortionDesc.vDistortion2);
		CJson_Utility::Load_Float2(In_Json["vDistortion3"], m_tDistortionDesc.vDistortion3);

		m_tDistortionDesc.fDistortionScale = In_Json["fDistortionScale"];
		m_tDistortionDesc.fDistortionBias = In_Json["fDistortionBias"];
	}


}



HRESULT CEffect_Particle::Change_TextureCom(wstring strProtoTextureTag)	// 툴 용
{
	if (TEXT("") == strProtoTextureTag)
		return S_OK;

	//_uint iCurLevel = m_pGameInstance->Get_CurrentLevel();

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
			Remove_TextureCom(TEXTURE_DIFFUSE);
		}

		eTexture = TEXTURE_DIFFUSE;
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, strProtoTextureTag, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));
	}
	else if (strProtoTextureTag.find(strNormal) != string::npos)
	{
		// 노말 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 노말폴더 -> 피 노말폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_NORAML])
		{
			Remove_TextureCom(TEXTURE_NORAML);
		}
		eTexture = TEXTURE_NORAML;
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, strProtoTextureTag, TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML])));
	}
	else if (strProtoTextureTag.find(strMask) != string::npos)
	{
		// 마스크 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 마스크폴더 -> 연기 마스크폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_MASK])
		{
			Remove_TextureCom(TEXTURE_MASK);
		}
		eTexture = TEXTURE_MASK;
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, strProtoTextureTag, TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK])));
	}
	else if (strProtoTextureTag.find(strNoise) != string::npos)
	{
		// 노이즈 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 노이즈폴더 -> 불 노이즈폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_NOISE])
		{
			Remove_TextureCom(TEXTURE_NOISE);
		}

		eTexture = TEXTURE_NOISE;
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, strProtoTextureTag, TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE])));
	}
	else if (strProtoTextureTag.find(strSprite) != string::npos)
	{
		// 스프라이트 텍스처 컴포넌트 해제 후 새로운 텍스처로 다시 생성 (예시 : 일반 스프라이트폴더 -> 연기 스프라이트폴더로 변경하고싶을 떄)
		if (nullptr != m_pTextureCom[TEXTURE_SPRITE])
		{
			Remove_TextureCom(TEXTURE_SPRITE);
		}
		eTexture = TEXTURE_SPRITE;
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, strProtoTextureTag, TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));
	}

	m_tVoidDesc.strTextureTag[eTexture] = strProtoTextureTag;
	m_tVoidDesc.iTextureIndex[eTexture] = 0;

	return S_OK;
}

HRESULT CEffect_Particle::Remove_TextureCom(TEXTURE eTexture)
{
	wstring strTexureComTag = TEXT("");

	if (TEXTURE_DIFFUSE == eTexture)
		strTexureComTag = TEXT("Com_Diffuse");
	else if (TEXTURE_NORAML == eTexture)
		strTexureComTag = TEXT("Com_Normal");
	else if (TEXTURE_MASK == eTexture)
		strTexureComTag = TEXT("Com_Mask");
	else if (TEXTURE_NOISE == eTexture)
		strTexureComTag = TEXT("Com_Noise");
	else if (TEXTURE_SPRITE == eTexture)
		strTexureComTag = TEXT("Com_Sprite");


	m_tVoidDesc.strTextureTag[eTexture] = TEXT("");
	m_tVoidDesc.iTextureIndex[eTexture] = 0;

	Remove_Component(strTexureComTag, reinterpret_cast<CComponent**>(&m_pTextureCom[eTexture]));

	return S_OK;
}

HRESULT CEffect_Particle::Ready_Components()
{
	//_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Particle_Point"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}


	/* For.Com_VIBuffer */
	{
		CVIBuffer_Particle::PARTICLE_BUFFER_DESC tBufferDesc = {};
#ifdef _DEBUG
		if (ECast(LEVEL_TOOL) != m_pGameInstance->Get_NextLevel())
		{
#endif // _DEBUG
			tBufferDesc.iCurNumInstance = m_tVoidDesc.iCurNumInstance;
			tBufferDesc.bUseRigidBody = m_tVoidDesc.bUseRigidBody;
#ifdef _DEBUG
		}
#endif // _DEBUG

		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &tBufferDesc));
	}


	/* For.Com_Texture */
	{
		// Diffuse
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_DIFFUSE])
			FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, m_tVoidDesc.strTextureTag[TEXTURE_DIFFUSE], TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE])));

		// Normal
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_NORAML])
			FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, m_tVoidDesc.strTextureTag[TEXTURE_NORAML], TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORAML])));

		// Mask => ! LEVEL_STATIC 으로 변경 !
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_MASK])
			FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, m_tVoidDesc.strTextureTag[TEXTURE_MASK], TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK])));

		// Noise => ! LEVEL_STATIC 으로 변경 !
		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_NOISE])
			FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, m_tVoidDesc.strTextureTag[TEXTURE_NOISE], TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE])));

		// Sprite
		//if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_SPRITE])
		//	FAILED_CHECK(__super::Add_Component(iNextLevel, m_tVoidDesc.strTextureTag[TEXTURE_SPRITE], TEXT("Com_Sprite"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_SPRITE])));
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
	//if (m_tVoidDesc.bUseSpriteAnim)
	//{
	//	// 스프라이트 사용이고, 
	//	if(nullptr != m_pTextureCom[TEXTURE_SPRITE])	// 스프라이트 텍스처 있으면 바인드
	//		FAILED_CHECK(m_pTextureCom[TEXTURE_SPRITE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_SPRITE]));
	//}
	//else
	{
		// 기본은 디퓨즈만 바인드
		if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
			FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_DIFFUSE]));
	}

	if (nullptr != m_pTextureCom[TEXTURE_NORAML])	// 노말 텍스처 있으면 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_NORAML]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NORAML]));

	if (nullptr != m_pTextureCom[TEXTURE_MASK])		// 마스크 텍스처 있으면 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tVoidDesc.iTextureIndex[TEXTURE_MASK]));

	if (nullptr != m_pTextureCom[TEXTURE_NOISE])	// 노이즈 텍스처 있으면 바인드
		FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tVoidDesc.iTextureIndex[TEXTURE_NOISE]));



	/* Camera ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	_float3 vCamDirectionFloat3 = m_pGameInstance->Get_CamDirection();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat3, sizeof(_float3)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));


	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bBillBoard", &m_tVoidDesc.bBillBoard, sizeof(_bool)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bSoft", &m_tVoidDesc.bSoft, sizeof(_bool)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bUseMask", &m_tVoidDesc.bUseMask, sizeof(_bool)));

	/* Color & Discard ===============================================================================*/
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_iColorMode", &m_tVoidDesc.eMode_Color, sizeof(_int)));
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vColor_Mul", &m_tVoidDesc.vColor_Mul, sizeof(_float4)));

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tVoidDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = _float3(m_tVoidDesc.vColor_Clip.x, m_tVoidDesc.vColor_Clip.y, m_tVoidDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));


	// 이펙트 정보(파티클 버퍼에서 정해지는 정보들)
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_EffectDesc", m_pVIBufferCom->Get_ParticleShaderInfoDescs().data(), _uint(sizeof(CVIBuffer_Particle::PARTICLE_SHADER_INFO_DESC) * m_pVIBufferCom->Get_ParticleShaderInfoDescs().size())));


	// 스프라이트
	//FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bSprite", &m_tVoidDesc.bUseSpriteAnim, sizeof(_bool)));
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
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_iScrollType", &m_tDistortionDesc.eType_Scroll, sizeof(_int)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vScrollSpeeds", &m_tDistortionDesc.vScrollSpeeds, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vScales", &m_tDistortionDesc.vScales, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion1", &m_tDistortionDesc.vDistortion1, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion2", &m_tDistortionDesc.vDistortion2, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vDistortion3", &m_tDistortionDesc.vDistortion3, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDistortionScale", &m_tDistortionDesc.fDistortionScale, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDistortionBias", &m_tDistortionDesc.fDistortionBias, sizeof(_float)));



	/* Rim Bloom */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_tVoidDesc.vBloomPower, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_tVoidDesc.vRimColor, sizeof(_float4)));


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

	Delete_Object_Owner();

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pShaderCom);


}

