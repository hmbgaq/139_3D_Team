#include "stdafx.h"
#include "AnimalObject.h"
#include "GameInstance.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "UI_Weakness.h"

CAnimalObject::CAnimalObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	
}

CAnimalObject::CAnimalObject(const CAnimalObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnimalObject::Initialize_Prototype()
{	
	return S_OK;
}

HRESULT CAnimalObject::Initialize(void* pArg)
{	
	m_tAnimalDesc = *(Animal_OBJECT_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (false == m_tAnimalDesc.bPreview)
		m_pTransformCom->Set_WorldMatrix(m_tAnimalDesc.WorldMatrix);

	_int iAnimCount = m_pModelCom->Get_AnimationNum();

	if (iAnimCount < m_tAnimalDesc.iPlayAnimationIndex || iAnimCount <= 0)
	{
		m_tAnimalDesc.iPlayAnimationIndex = 0;
	}

	m_pModelCom->Set_Animation(m_tAnimalDesc.iPlayAnimationIndex);
	


	return S_OK;
}

void CAnimalObject::Priority_Tick(_float fTimeDelta)
{
}

void CAnimalObject::Tick(_float fTimeDelta)
{
	_float3 vRootAnimPos = {};

	//m_pModelCom->Play_Animation(fTimeDelta, true);
	m_pModelCom->Play_Animation(fTimeDelta, &vRootAnimPos);

	m_pTransformCom->Add_RootBone_Position(vRootAnimPos);
}

void CAnimalObject::Late_Tick(_float fTimeDelta)
{

	FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONBLEND, this));
	
	return;
}

HRESULT CAnimalObject::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	wstring strTemp = Get_ModelTag();

	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		

		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
		m_pShaderCom->Begin(m_tAnimalDesc.iShaderPassIndex);
		m_pModelCom->Render((_uint)i);
	}
	
	return S_OK;
}

HRESULT CAnimalObject::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_SHADOW));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

_bool CAnimalObject::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CAnimalObject::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

void CAnimalObject::Set_AnimationIndex(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}


#ifdef _DEBUG


_bool CAnimalObject::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}

_bool CAnimalObject::Picking_VerJSY(RAY* pRay, _float3* vPickedPos)
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

HRESULT CAnimalObject::Ready_Components()
{

	
	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), m_tAnimalDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CAnimalObject::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	m_fCamFar = m_pGameInstance->Get_CamFar();
	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_fCamFar, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4)));
	
	return S_OK;
}

CAnimalObject * CAnimalObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CAnimalObject*		pInstance = new CAnimalObject(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnimalObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAnimalObject::Clone(void* pArg)
{
	CAnimalObject*		pInstance = new CAnimalObject(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimalObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAnimalObject::Pool()
{
	return new CAnimalObject(*this);
}

void CAnimalObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
	
	if(m_iCurrnetLevel == (_uint)LEVEL_TOOL)
		Safe_Release(m_pPickingCollider);

	if(nullptr != m_pEffect)
		Safe_Release(m_pEffect);
}



