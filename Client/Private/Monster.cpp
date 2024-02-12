#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{

}

CMonster::CMonster(const CMonster & rhs)
	: CLandObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(rand() % 20);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 0.f, rand() % 20, 1.f));

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{

}

void CMonster::Tick(_float fTimeDelta)
{


	SetUp_OnTerrain(m_pTransformCom);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	shared_ptr<CCollider>	pTargetCollider = dynamic_pointer_cast<CCollider>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0, TEXT("Part_Weapon")));

	m_pColliderCom->Collision(pTargetCollider);

	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());

	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
	{
		m_pModelCom->Play_Animation(fTimeDelta, true);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_ptr<CMonster>(this))))//
			return;

#ifdef _DEBUG
		m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
	}


	/*if (true == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pModelCom->Play_Animation(fTimeDelta, true);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
	}*/


}

HRESULT CMonster::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(_uint(i));
	}


	

	return S_OK;
}

HRESULT CMonster::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

shared_ptr<CMonster> CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	shared_ptr<CMonster>		pInstance = make_shared<CMonster>(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMonster::Clone(void* pArg)
{
	shared_ptr<CMonster>		pInstance = make_shared<CMonster>(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	//Safe_Release(m_pColliderCom);
	//Safe_Release(m_pModelCom);	
	//Safe_Release(m_pShaderCom);
}

