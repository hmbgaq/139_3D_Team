#include "stdafx.h"
#include "..\Public\Environment_SpecialObject.h"

#include "GameInstance.h"

CEnvironment_SpecialObject::CEnvironment_SpecialObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{

}

CEnvironment_SpecialObject::CEnvironment_SpecialObject(const CEnvironment_SpecialObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_SpecialObject::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Initialize(void* pArg)
{
	m_tEnvironmentDesc = *(ENVIRONMENT_SPECIALOBJECT_DESC*)pArg;

	m_iCurrentLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (false == m_tEnvironmentDesc.bPreview)
		m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);


	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		m_pModelCom->Set_Animation(m_tEnvironmentDesc.iPlayAnimationIndex);
	}

	

	

	return S_OK;
}

void CEnvironment_SpecialObject::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_SpecialObject::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * 0.5f;

	if(m_fTimeAcc >= 180.f)
		m_fTimeAcc = 0.f;
	//f (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	//
	//	m_pPickingCollider->Update(m_pTransformCom->Get_WorldMatrix());
	//
}

void CEnvironment_SpecialObject::Late_Tick(_float fTimeDelta)
{
	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		_float3 vRootAnimPos = {};

		//m_pModelCom->Play_Animation(fTimeDelta, true);
		m_pModelCom->Play_Animation(fTimeDelta, &vRootAnimPos);

		m_pTransformCom->Add_RootBone_Position(vRootAnimPos);
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	//if (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	//{
	//	m_pGameInstance->Add_DebugRender(m_pPickingCollider);
	//}
}

HRESULT CEnvironment_SpecialObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_tEnvironmentDesc.bAnimModel == true)
		{
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		}

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//#몬스터모델렌더
	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 100.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, m_pGameInstance->Get_CamFar()));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//TODO 클라에서 모델의 메시 개수를 받아와서 순회하면서 셰이더 바인딩해주자.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(2); //TODO 추후 ENUM 으로 변경

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

_bool CEnvironment_SpecialObject::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_SpecialObject::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

#ifdef _DEBUG

_bool CEnvironment_SpecialObject::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}

_bool CEnvironment_SpecialObject::Picking_VerJSY(RAY* pRay, _float3* vPickedPos)
{
	_vector vOrigin = XMLoadFloat4(&pRay->vPosition);
	_vector vDir = XMLoadFloat3(&pRay->vDirection);


	if (true == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		_float fDist;


		CBounding_Sphere* pBoundingSphere = dynamic_cast<CBounding_Sphere*>(m_pPickingCollider->Get_Bounding());

		if (pBoundingSphere == nullptr)
		{
			MSG_BOX("콜라이더 없대");
			return false;
		}

		const BoundingSphere* pBoundingSphereBox = pBoundingSphere->Get_Bounding();

		if (true == pBoundingSphereBox->Intersects(vOrigin, vDir, fDist))
		{
			*vPickedPos = vOrigin + fDist * vDir;
			return true;
		}
	}
	else
		return false;


	return false;
}

#endif

HRESULT CEnvironment_SpecialObject::Ready_Components()
{

	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	else
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	if (m_tEnvironmentDesc.strDiffuseTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevel, m_tEnvironmentDesc.strDiffuseTextureTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTexture)));
	}

	if (m_tEnvironmentDesc.strMaskTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevel, m_tEnvironmentDesc.strMaskTextureTag, TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexture)));
	}

	if (m_tEnvironmentDesc.strNoiseTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevel, m_tEnvironmentDesc.strNoiseTextureTag, TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTexture)));
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), m_tEnvironmentDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	//CBounding_Sphere::BOUNDING_SPHERE_DESC Test;
	//
	//m_pModelCom->Calculate_Sphere_Radius(&Test.vCenter, &Test.fRadius);
	//Test.iLayer = (_uint)COLLISION_LAYER::PICKING_INSTANCE;

		//!CBounding_AABB::BOUNDING_AABB_DESC Desc_AABB;
		//!
		//!Desc_AABB.iLayer = (_uint)COLLISION_LAYER::PICKING_MESH;
		//!Desc_AABB.vExtents = m_pModelCom->Calculate_AABB_Extents_From_Model();
		//Desc_AABB.vCenter = _float3(0.f, 0.f, 0.f);

	//if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pPickingCollider), &Test)))
	//{
	//	MSG_BOX("ㅈ댐");
	//	return E_FAIL;
	//}

	//!if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pPickingCollider), &Desc_AABB)))
	//!{
	//!	MSG_BOX("ㅈ댐");
	//!	return E_FAIL;
	//!}

//	m_pPickingCollider

	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (nullptr != m_pDiffuseTexture)
	{
		if (FAILED(m_pDiffuseTexture->Bind_ShaderResource(m_pShaderCom, "g_ColorDiffuse")))
			return E_FAIL;
	}

	if (nullptr != m_pMaskTexture)
	{
		if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	if (nullptr != m_pNoiseTexture)
	{
		if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CEnvironment_SpecialObject* CEnvironment_SpecialObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEnvironment_SpecialObject* pInstance = new CEnvironment_SpecialObject(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_SpecialObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_SpecialObject::Clone(void* pArg)
{
	CEnvironment_SpecialObject* pInstance = new CEnvironment_SpecialObject(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_SpecialObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_SpecialObject::Pool()
{
	return new CEnvironment_SpecialObject(*this);
}

void CEnvironment_SpecialObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);


	if (m_iCurrentLevel == (_uint)LEVEL_TOOL)
		Safe_Release(m_pPickingCollider);
}



