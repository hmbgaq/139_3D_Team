#include "stdafx.h"
#include "..\Public\Environment_LightObject.h"

#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Light.h"

CEnvironment_LightObject::CEnvironment_LightObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	
}

CEnvironment_LightObject::CEnvironment_LightObject(const CEnvironment_LightObject & rhs)
	: CGameObject(rhs)
{
} 

HRESULT CEnvironment_LightObject::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CEnvironment_LightObject::Initialize(void* pArg)
{	
	m_tEnvironmentDesc = *(ENVIRONMENT_LIGHTOBJECT_DESC*)pArg;

	
	
//! C:\Users\PC\Desktop\3D_TeamPortpolio\Client\Bin\DataFiles\Data_Effect\Data_Fire\FIre_Torch


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


	if (true == m_tEnvironmentDesc.bEffect)
	{
		m_pEffect = EFFECT_MANAGER->Create_Effect("FIre_Torch.json");
		m_pEffect->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	

	if(FAILED(m_pGameInstance->Add_Light(m_tEnvironmentDesc.LightDesc, m_tEnvironmentDesc.iLightIndex)))
		return E_FAIL;

	return S_OK;
}

void CEnvironment_LightObject::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_LightObject::Tick(_float fTimeDelta)
{
	//f (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	//
	//	m_pPickingCollider->Update(m_pTransformCom->Get_WorldMatrix());
	//
}

void CEnvironment_LightObject::Late_Tick(_float fTimeDelta)
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

	//if (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL)
	//{
	//	m_pGameInstance->Add_DebugRender(m_pPickingCollider);
	//}
}

HRESULT CEnvironment_LightObject::Render()
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

HRESULT CEnvironment_LightObject::Render_Shadow()
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

_bool CEnvironment_LightObject::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_LightObject::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

void CEnvironment_LightObject::Set_Diffuse(_float4 vDiffuse)
{
	CLight* pLight = m_pGameInstance->Find_Light(m_tEnvironmentDesc.iLightIndex);

	if (nullptr == pLight)
		return;

	pLight->Set_Diffuse(vDiffuse);
	m_tEnvironmentDesc.LightDesc.vDiffuse = vDiffuse;
	//pLight->Set_LightDesc()
}

void CEnvironment_LightObject::Set_Specular(_float4 vSpecular)
{
	CLight* pLight = m_pGameInstance->Find_Light(m_tEnvironmentDesc.iLightIndex);

	if (nullptr == pLight)
		return;

	pLight->Set_Specular(vSpecular);
	m_tEnvironmentDesc.LightDesc.vSpecular = vSpecular;
}

void CEnvironment_LightObject::Set_Ambient(_float4 vAmbient)
{
	CLight* pLight = m_pGameInstance->Find_Light(m_tEnvironmentDesc.iLightIndex);

	if (nullptr == pLight)
		return;

	pLight->Set_Ambient(vAmbient);
	m_tEnvironmentDesc.LightDesc.vAmbient = vAmbient;
}

void CEnvironment_LightObject::Set_Enable(_bool bEnable)
{
	CLight* pLight = m_pGameInstance->Find_Light(m_tEnvironmentDesc.iLightIndex);

	if (nullptr == pLight)
		return;

	pLight->Set_LightEnable(bEnable);
	m_tEnvironmentDesc.LightDesc.bEnable = bEnable;
}

void CEnvironment_LightObject::Set_LightDesc(LIGHT_DESC tLightDesc)
{
	CLight* pLight = m_pGameInstance->Find_Light(tLightDesc.iLightIndex);
	
	if(nullptr == pLight)
		return;

	pLight->Set_LightDesc(tLightDesc);
	m_tEnvironmentDesc.LightDesc = tLightDesc;

//TODO public: /* For.Light_Manager */
//TODO 	HRESULT			Add_Light(const LIGHT_DESC & LightDesc, _int & outLightIndex);
//TODO 	class CLight*		Find_Light(const _int iIndex);
//TODO 	void				Change_Light_Desc(const _int iIndex, LIGHT_DESC newDesc);
}

void CEnvironment_LightObject::Change_LightType(LIGHT_DESC::TYPE eLightType)
{
	CLight* pLight = m_pGameInstance->Find_Light(m_tEnvironmentDesc.iLightIndex);

	if (nullptr == pLight)
		return;

	Safe_Release(pLight);

	HRESULT hr;

	hr = m_pGameInstance->Add_Light(m_tEnvironmentDesc.LightDesc, m_tEnvironmentDesc.iLightIndex);

	if (S_OK == hr)
	{
		m_tEnvironmentDesc.LightDesc.eType = eLightType;
	}
}

void CEnvironment_LightObject::Change_LightEffect(LIGHT_EFFECT eLightEffectType)
{
	if (false == m_tEnvironmentDesc.bEffect)
		return;

	string strEffectFilePath = "FIre_Torch.json";

	switch (eLightEffectType)
	{
	case Client::CEnvironment_LightObject::LIGHTEFFECT_TORCH:
		strEffectFilePath = "FIre_Torch.json";
		break;
	case Client::CEnvironment_LightObject::LIGHTEFFECT_TEST1:
		strEffectFilePath = "FIre_Torch.json";
		break;
	case Client::CEnvironment_LightObject::LIGHTEFFECT_TEST2:
		strEffectFilePath = "FIre_Torch.json";
		break;
	case Client::CEnvironment_LightObject::LIGHTEFFECT_TEST3:
		strEffectFilePath = "FIre_Torch.json";
		break;
	case Client::CEnvironment_LightObject::LIGHTEFFECT_TEST4:
		strEffectFilePath = "FIre_Torch.json";
		break;
	
	}


	if (m_pEffect != nullptr)
	{
		m_pEffect->Set_Dead(true);
		m_pEffect = nullptr;
	}



	m_pEffect = EFFECT_MANAGER->Create_Effect(strEffectFilePath);

	if (m_pEffect == nullptr)
		MSG_BOX("라이트 오브젝트 이펙트 변경 실패");
	else
	{
		m_pEffect->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_tEnvironmentDesc.eLightEffect = eLightEffectType;
	}
}

#ifdef _DEBUG

_bool CEnvironment_LightObject::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}

_bool CEnvironment_LightObject::Picking_VerJSY(RAY* pRay, _float3* vPickedPos)
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

HRESULT CEnvironment_LightObject::Ready_Components()
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

HRESULT CEnvironment_LightObject::Bind_ShaderResources()
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
	
	return S_OK;
}

CEnvironment_LightObject * CEnvironment_LightObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CEnvironment_LightObject*		pInstance = new CEnvironment_LightObject(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_LightObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnvironment_LightObject::Clone(void* pArg)
{
	CEnvironment_LightObject*		pInstance = new CEnvironment_LightObject(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_LightObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_LightObject::Pool()
{
	return new CEnvironment_LightObject(*this);
}

void CEnvironment_LightObject::Free()
{
	__super::Free();

	
	if(m_pEffect != nullptr)
		Safe_Release(m_pEffect);

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);


	if(m_iCurrentLevel == (_uint)LEVEL_TOOL)
		Safe_Release(m_pPickingCollider);
}



