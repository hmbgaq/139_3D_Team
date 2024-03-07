#include "stdafx.h"
#include "Model_Preview.h"

#include "GameInstance.h"


CModel_Preview::CModel_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{

}

CModel_Preview::CModel_Preview(const CModel_Preview& rhs)
	: CGameObject(rhs)
{
}

HRESULT CModel_Preview::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Preview::Initialize(void* pArg)
{
	MODEL_PREVIEW_DESC* pDesc = (MODEL_PREVIEW_DESC*)pArg;

	m_tDesc.strProtoTag = pDesc->strProtoTag;
	m_tDesc.strModelTag = pDesc->strModelTag;
	m_tDesc.iAnimIndex = pDesc->iAnimIndex;
	m_tDesc.iRenderGroup = pDesc->iRenderGroup;
	m_tDesc.iShaderPassIndex = pDesc->iShaderPassIndex;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pModelCom->Set_Animation(m_tDesc.iAnimIndex);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	return S_OK;
}

void CModel_Preview::Priority_Tick(_float fTimeDelta)
{
	if (m_tDesc.bRender)
	{
		__super::Priority_Tick(fTimeDelta);
	}

}

void CModel_Preview::Tick(_float fTimeDelta)
{

	if (m_tDesc.bRender)
	{
		__super::Tick(fTimeDelta);

	}
}

void CModel_Preview::Late_Tick(_float fTimeDelta)
{
	if (m_tDesc.bRender)
	{
		__super::Late_Tick(fTimeDelta);
	
		if (TRUE == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_Position(), 2.f))
		{
			//m_pModelCom->Play_Animation(fTimeDelta, m_vMovePos);
			m_pModelCom->Play_Animation(fTimeDelta, TRUE);

			if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP(m_tDesc.iRenderGroup), this)))
				return;
		}
	}
}

HRESULT CModel_Preview::Render()
{
	if (m_tDesc.bRender)
	{
		__super::Render();

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

			m_pShaderCom->Begin(m_tDesc.iShaderPassIndex);

			m_pModelCom->Render((_uint)i);
		}

	}

	return S_OK;
}

HRESULT CModel_Preview::Ready_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL,m_tDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Preview::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	return S_OK;
}

CModel_Preview* CModel_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CModel_Preview* pInstance = new CModel_Preview(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModel_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CModel_Preview::Clone(void* pArg)
{
	CModel_Preview* pInstance = new CModel_Preview(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel_Preview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CModel_Preview::Pool()
{
	return new CModel_Preview(*this);
}

void CModel_Preview::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

