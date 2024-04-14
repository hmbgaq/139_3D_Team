#include "stdafx.h"
#include "Body_VampireCommander.h"
#include "GameInstance.h"

CBody_VampireCommander::CBody_VampireCommander(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody_Monster(pDevice, pContext, strPrototypeTag)
{
}

CBody_VampireCommander::CBody_VampireCommander(const CBody_VampireCommander& rhs)
	: CBody_Monster(rhs)
{
}

HRESULT CBody_VampireCommander::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_VampireCommander::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vDiscardMesh[CBody_VampireCommander::RENDER_STATE::ATTACK] = {1}; // 무기 

	return S_OK;
}

void CBody_VampireCommander::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_VampireCommander::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_VampireCommander::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_VampireCommander::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));
		m_pShaderCom->Begin(ECast(MONSTER_SHADER::COMMON_ORIGIN));

		m_pModelCom->Render((_uint)i);
	}

	//FAILED_CHECK(Bind_ShaderResources());

	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	auto iter = m_vDiscardMesh.find(m_eRender_State);
	//	if (iter != m_vDiscardMesh.end())
	//	{
	//		auto& Discard = iter->second;
	//		if (find(Discard.begin(), Discard.end(), i) != Discard.end())
	//		{
	//			if (m_eRender_State == CBody_VampireCommander::RENDER_STATE::ATTACK)
	//			{
	//				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
	//				m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
	//				m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
	//				m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

	//				m_pShaderCom->Begin(3);
	//				m_pModelCom->Render((_uint)i);
	//			}
	//		}
	//		else
	//		{
	//			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
	//			m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
	//			m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
	//			m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

	//			m_pShaderCom->Begin(0);
	//			m_pModelCom->Render((_uint)i);
	//		}
	//	}
	//}

	return S_OK;
}

HRESULT CBody_VampireCommander::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CBody_VampireCommander::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_VampireCommander::OnCollisionStay(CCollider* other)
{
	__super::OnCollisionStay(other);
}

void CBody_VampireCommander::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}

void CBody_VampireCommander::SetUp_Animation(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

HRESULT CBody_VampireCommander::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_VampireCommander"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.fRadius = 1.3f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_VampireCommander::Bind_ShaderResources()
{
// 	FAILED_CHECK(__super::Bind_ShaderResources());
// 
// 	_float fCamFar = m_pGameInstance->Get_CamFar();
// 
// 	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	if (m_eRender_State == CBody_VampireCommander::RENDER_STATE::ATTACK)
	{
		m_vCamPos = m_pGameInstance->Get_CamPosition();
		m_vRimColor = { 0.73f, 0.0f, 0.0f, 1.0f };
		m_vBloomPower = _float3(0.5f, 0.5f, 0.5f);
		m_fRimPower = 5.f;

				m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));
				m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
				m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_vBloomPower, sizeof(_float3));
				m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));
	}

	return S_OK;
}

void CBody_VampireCommander::Check_Frustum() 
{
	m_bIsInFrustum = true;
}

CBody_VampireCommander* CBody_VampireCommander::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_VampireCommander* pInstance = new CBody_VampireCommander(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_VampireCommander");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_VampireCommander::Clone(void* pArg)
{
	CBody_VampireCommander* pInstance = new CBody_VampireCommander(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_VampireCommander");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_VampireCommander::Pool()
{
	return new CBody_VampireCommander(*this);
}

void CBody_VampireCommander::Free()
{
	__super::Free();
}
