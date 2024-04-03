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

	if (true == m_tEnvironmentDesc.bLightObject)
	{
		if (FAILED(m_pGameInstance->Add_Light(m_tEnvironmentDesc.tLightDesc, m_tEnvironmentDesc.iLightIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CEnvironment_Object::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_Object::Tick(_float fTimeDelta)
{
	//f (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	//
	//	m_pPickingCollider->Update(m_pTransformCom->Get_WorldMatrix());
	//
	wstring strTemp = Get_ModelTag();
	int a = 0;

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

	FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONBLEND, this));

	//if (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	//{
	//	m_pGameInstance->Add_DebugRender(m_pPickingCollider);
	//}

	return;
}

HRESULT CEnvironment_Object::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	wstring strTemp = Get_ModelTag();

	if (false == bRenderIce && true == bIcarusTexture)	// Icarus Ground Render
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_tEnvironmentDesc.bAnimModel == true)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			}

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
			m_pRADTexture->Bind_ShaderResource(m_pShaderCom, "g_RADTexture");
			m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_ORIGIN));
			m_pModelCom->Render((_uint)i);
		}
	}
	else if (true == bRenderIce && false == bIcarusTexture)	// Ice 
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_tEnvironmentDesc.bAnimModel == true)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			}
			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pIceNoise->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture");
			m_pIceDiffuse->Bind_ShaderResource(m_pShaderCom, "g_ColorDiffuse");

			if (i == m_iIceMeshNumber)
				m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_ICICLE));
			else
				m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);

			m_pModelCom->Render((_uint)i);
		}
	}
	else
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (m_tEnvironmentDesc.bAnimModel == true)
			{
				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
			}

			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
			m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);
			m_pModelCom->Render((_uint)i);
		}
	}
	return S_OK;
}

HRESULT CEnvironment_Object::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_SHADOW));
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

void CEnvironment_Object::Set_AnimationIndex(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
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

	/* 소영 얼음중 */		
	
	wstring strTemp = Get_ModelTag();

	if (TEXT("Prototype_Component_Model_TeslaIcicle1") == strTemp ||
		TEXT("Prototype_Component_Model_TeslaIcicle3") == strTemp ||
		TEXT("Prototype_Component_Model_TeslaIcicle4") == strTemp ||
		TEXT("Prototype_Component_Model_TeslaIcicle5") == strTemp ||
		TEXT("Prototype_Component_Model_TeslaIcicle6") == strTemp)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_IceNoise"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pIceNoise)));
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_IceDiffuse"), TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pIceDiffuse)));
		bRenderIce = true;
		bIcarusTexture = false;
		m_iIceMeshNumber = 1;
	}
	else if (TEXT("Prototype_Component_Model_TeslaIcicle2") == strTemp ||
			 TEXT("Prototype_Component_Model_TeslaIcicle3") == strTemp ||
			 TEXT("Prototype_Component_Model_TeslaIcicleLong1") == strTemp ||
			 TEXT("Prototype_Component_Model_TeslaIcicleLong2") == strTemp ||
			 TEXT("Prototype_Component_Model_TeslaIcicleLong3") == strTemp ||
			 TEXT("Prototype_Component_Model_TeslaIcicleLong4") == strTemp)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_IceNoise"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pIceNoise)));
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_IceDiffuse"), TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pIceDiffuse)));
		bRenderIce = true;
		bIcarusTexture = false;
		m_iIceMeshNumber = 0;
	}
	else if (TEXT("Prototype_Component_Model_ICarusGround2") == strTemp ||
		TEXT("Prototype_Component_Model_ICarusRockCliff4") == strTemp)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Shader_IcarusRAD"), TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pIceDiffuse)));
		bIcarusTexture = true;
		bRenderIce = false;
	}
	else
	{
		bRenderIce = false;
		bIcarusTexture = false;
	}

	return S_OK;
}

HRESULT CEnvironment_Object::Bind_ShaderResources()
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
	
	if(nullptr != m_pIceNoise)
		Safe_Release(m_pIceNoise);
	if(nullptr != m_pIceDiffuse)
		Safe_Release(m_pIceDiffuse);

	//m_pGameInstance->Add_Light()

	if(m_iCurrentLevel == (_uint)LEVEL_TOOL)
		Safe_Release(m_pPickingCollider);
}



