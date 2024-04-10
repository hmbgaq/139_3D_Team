#include "..\Public\DestructableProp.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Character.h"

CDestructableProp::CDestructableProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CDestructableProp::CDestructableProp(const CDestructableProp& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDestructableProp::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CDestructableProp::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	if (nullptr != pArg)
	{
		m_tDestructablePropDesc = *(DESTRUCTABLE_PROP_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(m_tDestructablePropDesc.WorldMatrix);
	}


	Ready_Components();

	return S_OK;
}

void CDestructableProp::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CDestructableProp::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(m_pColliderCom != nullptr)
	m_pColliderCom->Update(m_pTransformCom->Get_WorldFloat4x4());
}

void CDestructableProp::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return;

#ifdef _DEBUG
		if(m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
	}

}

HRESULT CDestructableProp::Render()
{
	FAILED_CHECK(__super::Render());

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CDestructableProp::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

HRESULT CDestructableProp::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	return S_OK;
}


void CDestructableProp::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
