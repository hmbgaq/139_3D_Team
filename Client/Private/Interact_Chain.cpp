#include "Interact_Chain.h"
#include "Model.h"
#include "Shader.h"
#include "GameInstance.h"

CInteract_Chain::CInteract_Chain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteract_Chain::CInteract_Chain(const CInteract_Chain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteract_Chain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteract_Chain::Initialize(void* pArg)
{
	if(FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 0.f, 0.f, 1.f));
	//m_pTransformCom->Set_Scaling(100.f, 100.f, 100.f);

	return S_OK;
}

void CInteract_Chain::Priority_Tick(_float fTimeDelta)
{
}

void CInteract_Chain::Tick(_float fTimeDelta)
{
}

void CInteract_Chain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;


}

HRESULT CInteract_Chain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CInteract_Chain::Ready_Components()
{
	/* For. Transform*/
	{
		if (FAILED(__super::Initialize(nullptr)))
			return E_FAIL;
	}

	/* For.Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	/* For.Com_Model */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chain"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInteract_Chain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CInteract_Chain* CInteract_Chain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInteract_Chain* pInstance = new CInteract_Chain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteract_Chain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInteract_Chain::Clone(void* pArg)
{
	CInteract_Chain* pInstance = new CInteract_Chain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteract_Chain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInteract_Chain::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
