#include "stdafx.h"
#include "..\Public\Environment_SpecialObject.h"

#include "GameInstance.h"
#include "Environment_LightObject.h"
#include "Environment_Interact.h"
#include "Data_Manager.h"
//#include "UI_Weakness.h"

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

	m_iCurrnetLevel = m_pGameInstance->Get_NextLevel();

	
	m_tEnvironmentDesc.strDiffuseTextureTag = L"Prototype_Component_Texture_SpecialSignalDiffuseTexture";


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

	

	if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
	{
		SignalInit();
	}
	else if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
	{
		TrackLeverInit();
	}

	
	m_pSnowMountainWagon = CData_Manager::GetInstance()->Get_SnowMountainWagon();

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

	if(m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
	{
		SignalFunction(fTimeDelta);
	}
	else if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
	{
		TrackLeverFunction();
	}
	

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
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

		if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
		{
			if (m_bLeverOn == true)
			{
		
				m_pShaderCom->Begin(m_iSignalMeshShaderPass);
		 
			}
			else
				m_pShaderCom->Begin(8);
		}
		else
		{
			m_pShaderCom->Begin(m_iSignalMeshShaderPass);
		}
		
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Render_Shadow()
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

_bool CEnvironment_SpecialObject::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_SpecialObject::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

void CEnvironment_SpecialObject::Set_AnimationIndex(_int iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
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

void CEnvironment_SpecialObject::SignalInit()
{
	m_vSignalColor = m_vRedSignal;
	m_bSignalChange = false;
	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);
	m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(90.f));
}

void CEnvironment_SpecialObject::SignalFunction(const _float fTimeDelta)
{
	if (m_bSignalChange == true && m_bChangeLerp == false)
	{
		m_bChangeLerp = m_pTransformCom->Rotation_Lerp(XMConvertToRadians(90.f), fTimeDelta, 1.f);
	}
}

void CEnvironment_SpecialObject::Set_SignalChange(_bool bChange)
{
	#ifdef _DEBUG
		if (m_tEnvironmentDesc.eSpecialType != CEnvironment_SpecialObject::SPECIAL_SIGNAL)
		{
			MSG_BOX("CEnvironment_SpecialObject : 스페셜타입이 시그널이 아닙니다.");
			return;
		}
	#endif // _DEBUG

	m_bSignalChange = bChange; 
	
	if (m_bSignalChange == true)
		m_vSignalColor = m_vGreenSignal;
	else
		m_vSignalColor = m_vRedSignal;

	
}

HRESULT CEnvironment_SpecialObject::TrackLeverInit()
{
	m_pLeverWeaknessUI = dynamic_cast<CUI_Weakness*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));
	

	m_pLeverWeaknessUI->Set_Active(true);

	if(nullptr == m_pLeverWeaknessUI)
		return E_FAIL;

	m_pLeverWeaknessUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldFloat4x4(), _float3(0.f, 1.f, 0.f));

	if (nullptr != m_pLightObject)
		m_pLightObject->Set_Enable(false);

	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);

	m_bLeverOn = false;
	m_bSignalChange = false;

	if (m_iCurrnetLevel == (_uint)LEVEL_SNOWMOUNTAIN)
	{
		return Find_SignalBox_AndLightObject();
	}

	return S_OK;

	
}

HRESULT CEnvironment_SpecialObject::Find_SignalBox_AndLightObject()
{
	list<CGameObject*> BackGroundObjects = *m_pGameInstance->Get_GameObjects(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround");
	
	if(true == BackGroundObjects.empty())
		return E_FAIL;

	for (auto& iter : BackGroundObjects)
	{
		if (typeid(*iter) == typeid(CEnvironment_SpecialObject))
		{
			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(iter);

			if(pSpecialObject == nullptr)
				return E_FAIL;
			else
			{
				if (pSpecialObject->Get_SpecialType() == CEnvironment_SpecialObject::SPECIAL_SIGNAL && pSpecialObject->Get_SpecialGroupIndex() == m_tEnvironmentDesc.iSpecialGroupIndex)
				{
					m_pSignalObject = pSpecialObject;
				}
				else 
					continue;
			}
		}
		else if (typeid(*iter) == typeid(CEnvironment_LightObject))
		{
			CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(iter);

			if (pLightObject == nullptr)
				return E_FAIL;
			else
			{
				if (pLightObject->Get_SpecialGroupIndex() == m_tEnvironmentDesc.iSpecialGroupIndex)
				{
					m_pLightObject = pLightObject;
				}
				else
					continue;
			}
		}
	}


	return S_OK;
}

void CEnvironment_SpecialObject::TrackLeverFunction()
{
	if (m_pLeverWeaknessUI != nullptr)
	{
		if (m_pLeverWeaknessUI->Get_Enable() == false)
		{
			if(m_pSignalObject != nullptr)
				m_pSignalObject->Set_SignalChange(true);

			m_pLeverWeaknessUI->Set_Dead(true);
			m_pLeverWeaknessUI = nullptr;

			if (nullptr != m_pLightObject)
				m_pLightObject->Set_Enable(true);


			
			m_bLeverOn = true;
			m_pSnowMountainWagon->Set_SplineCheck(m_tEnvironmentDesc.iSpecialGroupIndex, true);
		}
		else
		{
			m_pLeverWeaknessUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldFloat4x4(), _float3(0.f, 1.f, 0.f));
		}
	}
	
}



HRESULT CEnvironment_SpecialObject::Ready_Components()
{

	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	else
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	if (m_tEnvironmentDesc.strDiffuseTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, m_tEnvironmentDesc.strDiffuseTextureTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTexture)));
	}

	if (m_tEnvironmentDesc.strMaskTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, m_tEnvironmentDesc.strMaskTextureTag, TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexture)));
	}

	if (m_tEnvironmentDesc.strNoiseTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, m_tEnvironmentDesc.strNoiseTextureTag, TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTexture)));
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

	if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseColor", &m_vSignalColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_fSignalBloomPower, sizeof(_float))))
			return E_FAIL;
	}
	else if(m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER && m_bLeverOn == true)
	{
		_float4 GreenColor = { 0.f, 1.f, 0.f, 1.f};
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseColor", &GreenColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_fSignalBloomPower, sizeof(_float))))
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


	if (m_iCurrnetLevel == (_uint)LEVEL_TOOL)
		Safe_Release(m_pPickingCollider);
}



