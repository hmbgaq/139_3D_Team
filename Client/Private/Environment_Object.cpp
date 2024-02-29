#include "stdafx.h"
#include "..\Public\Environment_Object.h"

#include "GameInstance.h"

CEnvironment_Object::CEnvironment_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	
}

CEnvironment_Object::CEnvironment_Object(const CEnvironment_Object & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_Object::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CEnvironment_Object::Initialize(void* pArg)
{	
	m_tEnvironmentDesc = *(ENVIRONMENT_OBJECT_DESC*)pArg;

	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (false == m_tEnvironmentDesc.bPreview)
		m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);


	if(true == m_tEnvironmentDesc.bAnimModel)
	{
		m_pModelCom->Set_Animation(m_tEnvironmentDesc.iPlayAnimationIndex);
	}

	

	return S_OK;
}

void CEnvironment_Object::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_Object::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	{
		m_pPickingCollider->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

void CEnvironment_Object::Late_Tick(_float fTimeDelta)
{
	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		_float3 vRootAnimPos = {};

		//m_pModelCom->Play_Animation(fTimeDelta, true);
		m_pModelCom->Play_Animation(fTimeDelta, &vRootAnimPos);

		m_pTransformCom->Add_RootBone_Position(vRootAnimPos);
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return ;

	if (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	{
		m_pGameInstance->Add_DebugRender(m_pPickingCollider);
	}
}

HRESULT CEnvironment_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if(m_tEnvironmentDesc.bAnimModel == true)
		{
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		}

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_Object::Render_Shadow()
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

_bool CEnvironment_Object::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_Object::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

#ifdef _DEBUG

_bool CEnvironment_Object::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}

_bool CEnvironment_Object::Picking_VerJSY(RAY* pRay, _float3* vPickedPos)
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

HRESULT CEnvironment_Object::Ready_Components()
{

	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	else
	{
		FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), m_tEnvironmentDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC Test;

	m_pModelCom->Calculate_Sphere_Radius(&Test.vCenter, &Test.fRadius);
	Test.iLayer = (_uint)COLLISION_LAYER::PICKING_INSTANCE;

	//!CBounding_AABB::BOUNDING_AABB_DESC Desc_AABB;
	//!
	//!Desc_AABB.iLayer = (_uint)COLLISION_LAYER::PICKING_MESH;
	//!Desc_AABB.vExtents = m_pModelCom->Calculate_AABB_Extents_From_Model();
	//Desc_AABB.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pPickingCollider), &Test)))
	{
		MSG_BOX("ㅈ댐");
		return E_FAIL;
	}

	//!if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pPickingCollider), &Desc_AABB)))
	//!{
	//!	MSG_BOX("ㅈ댐");
	//!	return E_FAIL;
	//!}

//	m_pPickingCollider

	return S_OK;
}

HRESULT CEnvironment_Object::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

CEnvironment_Object * CEnvironment_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CEnvironment_Object*		pInstance = new CEnvironment_Object(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnvironment_Object::Clone(void* pArg)
{
	CEnvironment_Object*		pInstance = new CEnvironment_Object(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_Object::Pool()
{
	return new CEnvironment_Object(*this);
}

void CEnvironment_Object::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
}



