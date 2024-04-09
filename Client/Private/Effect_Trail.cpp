#include "stdafx.h"
#include "..\Public\Effect_Trail.h"

#include "GameInstance.h"

#include "Effect.h"


CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = false;
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

	//m_tTrailDesc = *(TRAIL_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 트레일은 빌보드를 항상 끔
	m_tVoidDesc.bBillBoard = false;

	return S_OK;
}

void CEffect_Trail::Priority_Tick(_float fTimeDelta)
{

}

void CEffect_Trail::Tick(_float fTimeDelta)
{
	// 안씀. Tick_Trail()사용
};


void CEffect_Trail::Tick_Trail(_float _fTimeDelta, _float4x4 _ParentMatrix)
{
#ifdef _DEBUG
	//if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		//if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG
// 			if (m_pOwner == nullptr)
// 				return;

			//if (!m_pOwner->Is_Dead())
			{
				if (false == m_tVoidDesc.bPlay)
				{
					m_tVoidDesc.bRender = false;
					m_pVIBufferCom->Reset_Points(_ParentMatrix);
					return;
				}
				else
				{
					m_tVoidDesc.bRender = true;

					if (!m_bPause)	// 일시정지
					{
						m_pVIBufferCom->Update(_fTimeDelta, _ParentMatrix);
					}

				}
			}


#ifdef _DEBUG
		}
	}
#endif // _DEBUG
}


void CEffect_Trail::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		//if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG

			//if (nullptr != m_pOwner)	// 주인이 존재하고,
			//{
			//	// 비활성화 상태다.
			//	if (!m_pOwner->Get_Enable())
			//	{
			//		Set_Play(false);	// 재생 정지만

			//		return;
			//	}
		
			//	// 이펙트의 주인이 죽었다.
			//	if (m_pOwner->Is_Dead())
			//	{
			//		Set_Play(false);	// 재생 정지
			//	
			//		Delete_Object_Owner();	// 오너 해제
			//		Set_Dead(true);			// 트레일도 죽음

			//		return;
			//	}
			//}


			if (m_tVoidDesc.bRender)
			{
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
	//if (LEVEL_TOOL == static_cast<LEVEL>(m_pGameInstance->Get_CurrentLevel()))
	{
		//if (m_tVoidDesc.bActive_Tool)
		{
#endif // _DEBUG

			if (m_tVoidDesc.bRender)
			{
				if (nullptr == m_pShaderCom)
					return S_OK;

				if (FAILED(Bind_ShaderResources()))
					return E_FAIL;

				/* 이 쉐이더에 n번째 패스로 그릴거야. */
				m_pShaderCom->Begin(m_tVoidDesc.iShaderPassIndex);

				/* 내가 그릴려고하는 정점, 인덱스버퍼를 장치에 바인딩해. */
				m_pVIBufferCom->Bind_VIBuffers();

				/* 바인딩된 정점, 인덱스를 그려. */
				m_pVIBufferCom->Render();
			}

#ifdef _DEBUG
		}
	}
#endif // _DEBUG

	return S_OK;
}



HRESULT CEffect_Trail::Ready_Components()
{
	//_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));


	/* For.Com_VIBuffer */
	CVIBuffer_Trail::TRAIL_BUFFER_DESC tBufferInfo = {};
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &tBufferInfo));


	/* For.Com_Texture */
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;

		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_MASK])
		{
			/* For.Com_Mask => ! LEVEL_STATIC 으로 변경 ! */
			if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tVoidDesc.strTextureTag[TEXTURE_MASK],
				TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
				return E_FAIL;
		}

		if (TEXT("") != m_tVoidDesc.strTextureTag[TEXTURE_NOISE])
		{
			/* For.Com_Noise => ! LEVEL_STATIC 으로 변경 ! */
			if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tVoidDesc.strTextureTag[TEXTURE_NOISE],
				TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return S_OK;

	/* Matrix ============================================================================================ */
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
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

	 /* 빌보드 */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_bBillBoard", &m_tVoidDesc.bBillBoard, sizeof(_bool)));


	/* Color & Discard ===============================================================================*/
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlpha_Discard", &m_tVoidDesc.vColor_Clip.w, sizeof(_float)));

	_float3 vBlack_Discard = _float3(m_tVoidDesc.vColor_Clip.x, m_tVoidDesc.vColor_Clip.y, m_tVoidDesc.vColor_Clip.z);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBlack_Discard", &vBlack_Discard, sizeof(_float3)));

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_iColorMode", &m_tVoidDesc.eMode_Color, sizeof(_int)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vColor_Mul", &m_tVoidDesc.vColor_Mul, sizeof(_float4)));

	/* UV ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tVoidDesc.vUV_Offset, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tVoidDesc.vUV_Scale, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDegree", &m_tVoidDesc.fUV_RotDegree, sizeof(_float)));


	/* Camera ============================================================================================ */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));
	_float3 vCamDirectionFloat3 = m_pGameInstance->Get_CamDirection();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamDirection", &vCamDirectionFloat3, sizeof(_float3)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));



	/* Rim & Bloom ====================================================================================== */
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_tVoidDesc.vBloomPower, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_tVoidDesc.vRimColor, sizeof(_float4)));



	/* ETC ============================================================================================ */
	//FAILED_CHECK(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"));

	return S_OK;
}


_bool CEffect_Trail::Write_Json(json& Out_Json)
{
	CGameObject::Write_Json(Out_Json);


	Out_Json["Trail"]["eType_Effect"] = m_tVoidDesc.eType_Effect;
	Out_Json["Trail"]["eType_Easing"] = m_tVoidDesc.eType_Easing;

	string strTag = "";
	m_pGameInstance->WString_To_String(m_tVoidDesc.strProtoTag, strTag);
	Out_Json["Trail"]["strProtoTag"] = strTag;

	m_pGameInstance->WString_To_String(m_tVoidDesc.strPartTag, strTag);
	Out_Json["Trail"]["strPartTag"] = strTag;

	/* Texture */
	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		m_pGameInstance->WString_To_String(m_tVoidDesc.strTextureTag[i], strTag);
		Out_Json["Trail"]["strTextureTag"][i] = strTag;

		Out_Json["Trail"]["iTextureIndex"][i] = m_tVoidDesc.iTextureIndex[i];
	}


	Out_Json["Trail"]["iRenderGroup"] = m_tVoidDesc.iRenderGroup;
	Out_Json["Trail"]["iShaderPassIndex"] = m_tVoidDesc.iShaderPassIndex;

	CJson_Utility::Write_Float2(Out_Json["Trail"]["vUV_Offset"], m_tVoidDesc.vUV_Offset);
	CJson_Utility::Write_Float2(Out_Json["Trail"]["vUV_Scale"], m_tVoidDesc.vUV_Scale);
	Out_Json["Trail"]["fUV_RotDegree"] = m_tVoidDesc.fUV_RotDegree;

	Out_Json["bUV_Wave"] = m_tVoidDesc.bUV_Wave;
	CJson_Utility::Write_Float2(Out_Json["vUV_WaveSpeed"], m_tVoidDesc.vUV_WaveSpeed);
	CJson_Utility::Write_Float2(Out_Json["vUV_Offset_Mask"], m_tVoidDesc.vUV_Offset_Mask);
	CJson_Utility::Write_Float2(Out_Json["vUV_Scale_Mask"], m_tVoidDesc.vUV_Scale_Mask);


	CJson_Utility::Write_Float4(Out_Json["Trail"]["vColor_Offset"], m_tVoidDesc.vColor_Offset);
	CJson_Utility::Write_Float4(Out_Json["Trail"]["vColor_Clip"], m_tVoidDesc.vColor_Clip);
	CJson_Utility::Write_Float4(Out_Json["Trail"]["vColor_Mul"], m_tVoidDesc.vColor_Mul);
	Out_Json["eMode_Color"] = m_tVoidDesc.eMode_Color;

	/* Rim & Bloom */
	CJson_Utility::Write_Float3(Out_Json["Trail"]["vBloomPower"], m_tVoidDesc.vBloomPower);
	CJson_Utility::Write_Float4(Out_Json["Trail"]["vRimColor"], m_tVoidDesc.vRimColor);
	Out_Json["Trail"]["fRimPower"] = m_tVoidDesc.fRimPower;


	/* State */
	Out_Json["Trail"]["bUseSpriteAnim"] = m_tVoidDesc.bUseSpriteAnim;


	/* 주인 */
	Out_Json["Trail"]["bParentPivot"] = m_tVoidDesc.bParentPivot;	// false여야함 


	return true;
}

void CEffect_Trail::Load_FromJson(const json& In_Json)
{
	CGameObject::Load_FromJson(In_Json);


	/* Effect_Void */
	m_tVoidDesc.eType_Effect = In_Json["Trail"]["eType_Effect"];
	m_tVoidDesc.eType_Easing = In_Json["Trail"]["eType_Easing"];

	string strTag = "";
	strTag = static_cast<string>(In_Json["Trail"]["strProtoTag"]);
	m_pGameInstance->String_To_WString(strTag, m_tVoidDesc.strProtoTag);

	strTag = static_cast<string>(In_Json["Trail"]["strPartTag"]);
	m_pGameInstance->String_To_WString(strTag, m_tVoidDesc.strPartTag);


	/* Texture */
	for (_int i = 0; i < (_int)TEXTURE_END; i++)
	{
		strTag = static_cast<string>(In_Json["Trail"]["strTextureTag"][i]);
		m_pGameInstance->String_To_WString(strTag, m_tVoidDesc.strTextureTag[i]);

		m_tVoidDesc.iTextureIndex[i] = (_int)In_Json["Trail"]["iTextureIndex"][i];
	}


	m_tVoidDesc.iRenderGroup = (_int)In_Json["Trail"]["iRenderGroup"];
	m_tVoidDesc.iShaderPassIndex = (_int)In_Json["Trail"]["iShaderPassIndex"];


	CJson_Utility::Load_Float2(In_Json["Trail"]["vUV_Offset"], m_tVoidDesc.vUV_Offset);
	CJson_Utility::Load_Float2(In_Json["Trail"]["vUV_Scale"], m_tVoidDesc.vUV_Scale);

	m_tVoidDesc.fUV_RotDegree = (_float)In_Json["Trail"]["fUV_RotDegree"];

	m_tVoidDesc.bUV_Wave = (_bool)In_Json["Trail"]["bUV_Wave"];
	if (In_Json.contains("vUV_WaveSpeed")) // 다시 저장 후 if문 삭제
	{
		CJson_Utility::Load_Float2(In_Json["vUV_WaveSpeed"], m_tVoidDesc.vUV_WaveSpeed);
		CJson_Utility::Load_Float2(In_Json["vUV_Offset_Mask"], m_tVoidDesc.vUV_Offset_Mask);
		CJson_Utility::Load_Float2(In_Json["vUV_Scale_Mask"], m_tVoidDesc.vUV_Scale_Mask);
	}


	CJson_Utility::Load_Float4(In_Json["Trail"]["vColor_Offset"], m_tVoidDesc.vColor_Offset);
	CJson_Utility::Load_Float4(In_Json["Trail"]["vColor_Clip"], m_tVoidDesc.vColor_Clip);
	CJson_Utility::Load_Float4(In_Json["Trail"]["vColor_Mul"], m_tVoidDesc.vColor_Mul);
	if (In_Json.contains("eMode_Color")) //! TODO 다시 저장 후 if문 삭제
		m_tVoidDesc.eMode_Color = In_Json["eMode_Color"];

	/* Rim & Bloom */ 
	CJson_Utility::Load_Float3(In_Json["Trail"]["vBloomPower"], m_tVoidDesc.vBloomPower);
	CJson_Utility::Load_Float4(In_Json["Trail"]["vRimColor"], m_tVoidDesc.vRimColor);
	m_tVoidDesc.fRimPower = (_float)In_Json["Trail"]["fRimPower"];


	/* State */
	m_tVoidDesc.bUseSpriteAnim = (_bool)In_Json["Trail"]["bUseSpriteAnim"];


	/* 주인 */
	m_tVoidDesc.bParentPivot = (_bool)In_Json["Trail"]["bParentPivot"];	// false여야함 

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

	Delete_Object_Owner();	// 오너 해제

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pShaderCom);

}

