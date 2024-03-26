#include "stdafx.h"
#include "Body_Mother.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Player.h"

CBody_Mother::CBody_Mother(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Mother::CBody_Mother(const CBody_Mother& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Mother::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Mother::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_vDiscardMesh[CBody_Mother::RENDER_STATE::ATTACK] = { 1 }; // 무기 

	return S_OK;
}

void CBody_Mother::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Mother::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	//if (m_pGameInstance->Key_Down(DIK_K))
	//{
	//	iDiscardMeshNumber += 1;
	//	if (iDiscardMeshNumber > (_int)m_pModelCom->Get_NumMeshes())
	//	{
	//		iDiscardMeshNumber = 0;
	//	}
	//	cout << iDiscardMeshNumber << endl;
	//}
}

void CBody_Mother::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

	m_pModelCom->Play_Animation(fTimeDelta, m_vMovePos, CData_Manager::GetInstance()->Get_Player()->Get_Position());

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
		return;

// 	if (true == m_pGameInstance->isIn_WorldPlanes(m_pParentTransform->Get_State(CTransform::STATE_POSITION), 2.f))
// 	{
// 		
// 	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
}

HRESULT CBody_Mother::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPass = m_iShaderPass;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		// 		if (i == 0)
		// 			continue;

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i);
		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

// 	FAILED_CHECK(Bind_ShaderResources());
// 
// 	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
// 
// 	for (size_t i = 0; i < iNumMeshes; i++)
// 	{
// 		auto iter = m_vDiscardMesh.find(m_eRender_State);
// 		if (iter != m_vDiscardMesh.end())
// 		{
// 			auto& Discard = iter->second;
// 			if (find(Discard.begin(), Discard.end(), i) != Discard.end())
// 			{
// 				if (m_eRender_State == CBody_Mother::RENDER_STATE::ATTACK)
// 				{
// 					m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
// 
// 					//m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i);
// 					m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
// 					m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
// 					m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);
// 
// 
// 					m_pShaderCom->Begin(3);
// 
// 					m_pModelCom->Render((_uint)i);
// 				}
// 
// 			}
// 			else
// 			{
// 				m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
// 
// 				//m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i);
// 				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
// 				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
// 				m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);
// 
// 				m_pShaderCom->Begin(0);
// 
// 				m_pModelCom->Render((_uint)i);
// 			}
// 		}
// 	}

	return S_OK;
}

HRESULT CBody_Mother::Render_Shadow()
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

void CBody_Mother::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_Mother::OnCollisionStay(CCollider* other)
{
	__super::OnCollisionStay(other);
}

void CBody_Mother::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}

void CBody_Mother::SetUp_Animation(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

HRESULT CBody_Mother::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Mother"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.fRadius = 1.0f;
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);


	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Mother::Bind_ShaderResources()
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

	if (m_eRender_State == CBody_Mother::RENDER_STATE::ATTACK)
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

CBody_Mother* CBody_Mother::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Mother* pInstance = new CBody_Mother(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Mother");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Mother::Clone(void* pArg)
{
	CBody_Mother* pInstance = new CBody_Mother(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Mother");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Mother::Pool()
{
	return new CBody_Mother(*this);
}

void CBody_Mother::Free()
{
	__super::Free();
}

