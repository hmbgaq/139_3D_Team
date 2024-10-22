#include "stdafx.h"
#include "Environment_Instance.h"

#include "GameInstance.h"
#include "VIBuffer_Environment_Model_Instance.h"
#include "Model.h"

CEnvironment_Instance::CEnvironment_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{

}

CEnvironment_Instance::CEnvironment_Instance(const CEnvironment_Instance & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_Instance::Initialize_Prototype()
{	
	
	return S_OK;
}

HRESULT CEnvironment_Instance::Initialize(void* pArg)
{	
	m_tInstanceDesc = *(MAPTOOL_INSTANCE_DESC*)pArg;
	
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;	

	if (m_pGameInstance->Get_CurrentLevel() == (UINT)LEVEL_TOOL)
	{
		m_vecColliders.reserve(m_tInstanceDesc.iNumInstance);

		for (_uint i = 0; i < m_tInstanceDesc.iNumInstance; ++i)
		{
			m_vecColliders.push_back(nullptr);
		}
	}

	m_iCurrentLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CEnvironment_Instance::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_Instance::Tick(_float fTimeDelta)
{
	//m_pInstanceModelCom->Update(m_tInstanceDesc.vecInstanceInfoDesc);
	if (m_iCurrentLevel == (UINT)LEVEL_TOOL/* && m_bColliderRender == true*/)
	{
		for (_uint i = 0; i < m_tInstanceDesc.iNumInstance; ++i)
		{
			_matrix WorldMatrix = m_tInstanceDesc.vecInstanceInfoDesc[i].Get_Matrix();
			
			//_float3 vCenter = m_tInstanceDesc.vecInstanceInfoDesc[i].vCenter;
			//
			//WorldMatrix.r[3] -= XMLoadFloat3(&vCenter);
			
			m_vecColliders[i]->Update(WorldMatrix);
		}
	}
}

void CEnvironment_Instance::Late_Tick(_float fTimeDelta)
{

// 	m_pGameInstance->Transform_Frustum_ToLocalSpace(m_pTransformCom->Get_WorldMatrix());
// 
// 	if (true == m_pGameInstance->isIn_LocalPlanes(XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrixInverse()), 0.f))
// 	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
/*	}*/

		//if (m_iCurrentLevel == (UINT)LEVEL_TOOL/* && m_bColliderRender == true*/)
		//{
		//	for (_int i = 0; i < m_tInstanceDesc.iNumInstance; ++i)
		//	{
		//		m_pGameInstance->Add_DebugRender(m_vecColliders[i]);
		//	}
		//}
	

}

HRESULT CEnvironment_Instance::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	wstring strTemp = Get_ModelTag();

	//if (strTemp == TEXT("Prototype_Component_Model_OakTreeDry1") ||
	//	strTemp == TEXT("Prototype_Component_Model_OakTreeDry2") ||
	//	strTemp == TEXT("Prototype_Component_Model_OakTreeDry3") ||
	//	strTemp == TEXT("Prototype_Component_Model_OakTreeDry4") ||
	//	strTemp == TEXT("Prototype_Component_Model_OakTreeDry5") ||
	//	strTemp == TEXT("Prototype_Component_Model_OakTreeDry6"))
	//		int a = 0;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
		m_pShaderCom->Begin(m_tInstanceDesc.iShaderPassIndex);
		m_pInstanceModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_Instance::Render_Shadow()
{
	return S_OK;
}

_bool CEnvironment_Instance::Picking_Instance(RAY* pRay, _float3* vOutPoint)
{
	_vector vOrigin = XMLoadFloat4(&pRay->vPosition);
	_vector vDir = XMLoadFloat3(&pRay->vDirection);

	_float fDist;
	_float fClosestDist = FLT_MAX;
	_bool bResult = false;

	//nm_pGameInstance->iSIS
	for (_uint i = 0; i < m_tInstanceDesc.iNumInstance; ++i)
	{
		
		if (true == m_pGameInstance->isIn_WorldPlanes(m_tInstanceDesc.vecInstanceInfoDesc[i].Get_Position()))
		{
			CBounding_Sphere* pBoundingSphere = dynamic_cast<CBounding_Sphere*>(m_vecColliders[i]->Get_Bounding());

			if (pBoundingSphere == nullptr)
			{
				MSG_BOX("콜라이더 없대");
				return false;
			}

			const BoundingSphere* pBoundingSphereBox = pBoundingSphere->Get_Bounding();

			if (true == pBoundingSphereBox->Intersects(vOrigin, vDir, fDist))
			{

				_matrix matLocal = XMMatrixInverse(nullptr, m_tInstanceDesc.vecInstanceInfoDesc[i].Get_Matrix());

				*vOutPoint = vOrigin + fDist * vDir;
				//_float3 CenterToHitPoint = vOrigin + fDist * vDir;

				//_vector vCenterToHitPoint = vOrigin + fDist * vDir;
				////pBoundingSphereBox->
				//
				//_float3 vClosestPointOnSurface = XMLoadFloat3(&pBoundingSphereBox->Center) + pBoundingSphereBox->Radius * XMVector3Normalize(vCenterToHitPoint);
				//
				//*vOutPoint = vClosestPointOnSurface;

				return true;
			}
		}
		else
			return false;
	}
	
	return false;
}

_bool CEnvironment_Instance::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
	
}

void CEnvironment_Instance::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

void CEnvironment_Instance::Update(INSTANCE_INFO_DESC InstanceDesc, _int iIndex)
{
	m_pInstanceModelCom->Update(InstanceDesc, iIndex);
}

HRESULT CEnvironment_Instance::Remove_Instance(_uint iIndex)
{
	if(FAILED(m_pInstanceModelCom->Remove_Instance(iIndex)))
		return E_FAIL;
	
	m_tInstanceDesc.vecInstanceInfoDesc.erase(m_tInstanceDesc.vecInstanceInfoDesc.begin() + iIndex);
	m_tInstanceDesc.iNumInstance--;

	CCollider* pCollider = m_vecColliders[iIndex];

	m_vecColliders.erase(m_vecColliders.begin() + iIndex);

	Safe_Release(pCollider);

	return S_OK;
}

HRESULT CEnvironment_Instance::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Model_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_tInstanceDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	

	CVIBuffer_Environment_Model_Instance::ENVIRONMENT_MODEL_INSTANCE_DESC Desc;
	
	Desc.pModel = m_pModelCom;
	Desc.iNumInstance = m_tInstanceDesc.iNumInstance; // 5만개 해보니 내 컴기준 프레임 45까지 떨어짐
	Desc.vecBufferInstanceInfo = m_tInstanceDesc.vecInstanceInfoDesc;

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_VIBuffer_Environment_Model_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pInstanceModelCom), &Desc)))
		return E_FAIL;
	
	if (m_iCurrentLevel == (UINT)LEVEL_TOOL)
	{
		for (_int i = 0; i < Desc.iNumInstance; ++i)
		{
			CBounding_Sphere::BOUNDING_SPHERE_DESC Test;
			
			
			m_pModelCom->Calculate_Sphere_Radius(nullptr, &Test.fRadius);
			Test.vCenter = {0.f, 0.f, 0.f};
			Test.iLayer = (_uint)COLLISION_LAYER::PICKING_INSTANCE;

			wstring strColliderComTag = L"Com_Collider" + to_wstring(i);
			
			if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider_Sphere"), strColliderComTag, reinterpret_cast<CComponent**>(&m_vecColliders[i]), &Test)))
			{
				MSG_BOX("ㅈ댐");
				return E_FAIL;
			}
			
		}
	}
	

	return S_OK;
}

HRESULT CEnvironment_Instance::Bind_ShaderResources()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	_float gCamFar = m_pGameInstance->Get_CamFar();
	_float4 m_vCamPos = m_pGameInstance->Get_CamPosition();

	m_pShaderCom->Bind_RawValue("g_fCamFar", &gCamFar, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));

	return S_OK;
}

CEnvironment_Instance * CEnvironment_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CEnvironment_Instance*		pInstance = new CEnvironment_Instance(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnvironment_Instance::Clone(void* pArg)
{
	CEnvironment_Instance*		pInstance = new CEnvironment_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnvironment_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);


	if (m_iCurrentLevel == (_uint)LEVEL_TOOL)
	{
		_uint iColliderSize = (_uint)m_vecColliders.size();

		for (_uint i = 0; i < iColliderSize; ++i)
		{
			Safe_Release(m_vecColliders[i]);
		}
	}
	

	Safe_Release(m_pInstanceModelCom);
}

CGameObject* CEnvironment_Instance::Pool()
{
	return new CEnvironment_Instance(*this);
}

