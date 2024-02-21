#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{

}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));

	m_iRenderPass = 0;
	m_fTimeDelta = 0;

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{

}

void CMonster::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_K))
	{
		m_bEnable = false;
	}

	//if (m_pGameInstance->Key_Pressing(DIK_T)) 
	//{
	//	CGameObject* pPlayer = m_pGameInstance->Get_Player();
	//	CTransform* pPlayerTransform = pPlayer->Get_Transform();
	//	_vector pPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);
	//	m_pTransformCom->Look_At_Lerp(pPos, fTimeDelta);
	//}

	//SetUp_OnTerrain(m_pTransformCom);

	

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	//CCollider*	pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0, TEXT("Part_Weapon")));
	//
	//m_pColliderCom->Collision(pTargetCollider);

	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());

	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
	{
		_float3 vRootAnimPos = {};

		m_pModelCom->Play_Animation(fTimeDelta, &vRootAnimPos);

		m_pTransformCom->Add_RootBone_Position(vRootAnimPos);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
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
		//m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS); /* 노말 텍스쳐 추가 */

		if (3 == m_iRenderPass)
		{
			m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fTimeDelta, sizeof(_float));
			m_pDissolveTexCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture");
		}
		m_pShaderCom->Begin(m_iRenderPass);

		m_pModelCom->Render(_uint(i));
	}


	

	return S_OK;
}

HRESULT CMonster::Ready_Components()
{
	/* For.Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	/* For.Com_Model */

	//!Prototype_Component_Model_BeastBoss_Phase1
	//! //!Prototype_Component_Model_BeastBoss_Phase2
	//! //!Prototype_Component_Model_BeastBoss_Phase3


	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BeastBoss_Phase3"),
		//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		//	return E_FAIL;
	}

	/* For. Texture */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissolve"),
			TEXT("Com_DissolveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTexCom))))
			return E_FAIL;
	}

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	{
		BoundingDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
			return E_FAIL;
	}

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

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonster::Clone(void* pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster::Pool()
{
	return new CMonster(*this);
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
}



