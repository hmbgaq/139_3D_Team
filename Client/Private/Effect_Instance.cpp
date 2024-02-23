#include "stdafx.h"
#include "Effect_Instance.h"

#include "GameInstance.h"
#include "VIBuffer_Effect_Model_Instance.h"

#include "Easing_Utillity.h"

CEffect_Instance::CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CEffect_Void(pDevice, pContext, strPrototypeTag)
{

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
	m_tInstanceDesc = *(EFFECT_INSTANCE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pInstanceModelCom->Add_Mesh(m_pTransformCom->Get_WorldMatrix());



	return S_OK;
}

void CEffect_Instance::Priority_Tick(_float fTimeDelta)
{

}

void CEffect_Instance::Tick(_float fTimeDelta)
{

	if (m_tInstanceDesc.bActive_Tool)
	{
		m_fSequenceTime = m_fLifeTime + m_fRemainTime;

		if (m_tInstanceDesc.bPlay)
		{
			m_fSequenceAcc += fTimeDelta;

			// 시작지연 누적시간이 지나면 렌더 시작
			if (m_fWaitingAcc <= m_fWaitingTime)
			{
				m_fWaitingAcc += fTimeDelta;

				if (m_fWaitingAcc >= m_fWaitingTime)
				{
					m_tInstanceDesc.bRender = TRUE;
				}
				else
					return;
			}

			// 라이프타임 누적 시작
			m_fTimeAcc += fTimeDelta;
			m_fLifeTimeRatio = min(1.0f, m_fTimeAcc / m_fLifeTime);

			/* ======================= 라이프 타임 동작 시작 ======================= */



			/* ======================= 라이프 타임 동작 끝  ======================= */


			if (m_fTimeAcc >= m_fLifeTime)
			{
				// 삭제 대기시간 누적 시작
				m_fRemainAcc += fTimeDelta;

				// 디졸브 시작
				m_tInstanceDesc.bDissolve = TRUE;
				if (m_tInstanceDesc.bDissolve)
				{
					m_tInstanceDesc.fDissolveAmount = Easing::LerpToType(0.f, 1.f, m_fRemainAcc, m_fRemainTime, m_tInstanceDesc.eType_Easing);
				}

				if (m_fRemainAcc >= m_fRemainTime)
				{
					m_tInstanceDesc.fDissolveAmount = 1.f;
					m_tInstanceDesc.bRender = TRUE;
					return;
				}
			}

		}
	}


}

void CEffect_Instance::Late_Tick(_float fTimeDelta)
{
	if (m_tInstanceDesc.bActive_Tool)
	{
		if (m_tInstanceDesc.bRender)
		{
			//Compute_CamDistance();

			if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tInstanceDesc.iRenderGroup, this)))
				return;
		}
	}
}

HRESULT CEffect_Instance::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if(FIGURE == m_tInstanceDesc.eType_Mesh)
			m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		//m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		//m_pInstanceModelCom->Bind_ShaderResources(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		//Prototype_Component_Texture_Effect_Sprite

		m_pShaderCom->Begin(m_tInstanceDesc.iShaderPassIndex);
		m_pVIBufferCom->Render((_uint)i);
	}	

	return S_OK;
}

void CEffect_Instance::ReSet_Effect()
{
	__super::ReSet_Effect();

	m_tInstanceDesc.fDissolveAmount = 0.f;
	m_tInstanceDesc.bDissolve = FALSE;
	m_tInstanceDesc.bRender = FALSE;

}

void CEffect_Instance::End_Effect()
{
	__super::End_Effect();

	if (m_tInstanceDesc.bLoop)
	{
		ReSet_Effect();
	}
}

_bool CEffect_Instance::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	return true;
}

void CEffect_Instance::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}



HRESULT CEffect_Instance::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, m_tInstanceDesc.strShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, m_tInstanceDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CVIBuffer_Effect_Model_Instance::EFFECT_MODEL_INSTANCE_DESC Desc;
	Desc.pModel = m_pModelCom;
	Desc.iNumInstance = m_tInstanceDesc.iCurInstanceCnt; // 5만개 해보니 내 컴기준 프레임 45까지 떨어짐
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_VIBuffer_Effect_Model_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iNextLevel, m_tInstanceDesc.strTextureTag[TEXTURE_DIFFUSE],
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
		return E_FAIL;

	if (TEXT("") != m_tInstanceDesc.strTextureTag[TEXTURE_MASK])
	{
		/* For.Com_Mask */
		if (FAILED(__super::Add_Component(iNextLevel, m_tInstanceDesc.strTextureTag[TEXTURE_MASK],
			TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
			return E_FAIL;
	}

	if (TEXT("") != m_tInstanceDesc.strTextureTag[TEXTURE_NOISE])
	{
		/* For.Com_Noise */
		if (FAILED(__super::Add_Component(iNextLevel, m_tInstanceDesc.strTextureTag[TEXTURE_NOISE],
			TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NOISE]))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Instance::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	//_float4x4 WorldMatrix;
	//XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	//FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix));

	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));

	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	if (FLAT == m_tInstanceDesc.eType_Mesh)
	{
		FAILED_CHECK(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tInstanceDesc.iTextureIndex[TEXTURE_DIFFUSE]));

		if (nullptr != m_pTextureCom[TEXTURE_MASK])
		{
			FAILED_CHECK(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_tInstanceDesc.iTextureIndex[TEXTURE_MASK]));
		}
		if (nullptr != m_pTextureCom[TEXTURE_NOISE])
		{
			FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_tInstanceDesc.iTextureIndex[TEXTURE_NOISE]));
			FAILED_CHECK(m_pTextureCom[TEXTURE_NOISE]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", m_tInstanceDesc.iTextureIndex[TEXTURE_NOISE]));
		}
	}

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)));

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float)));


	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVOffset", &m_tInstanceDesc.vUV_Offset, sizeof(_float2)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_UVScale", &m_tInstanceDesc.vUV_Scale, sizeof(_float2)));

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_tInstanceDesc.fDissolveAmount, sizeof(_float)));


	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_DiscardValue", &m_tInstanceDesc.vColor_Clip.w, sizeof(_float)));

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

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < (_int)TEXTURE_END; i++)
		Safe_Release(m_pTextureCom[i]);

}



