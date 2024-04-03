#include "stdafx.h"
#include "Body_Son.h"
#include "GameInstance.h"
#include "Bone.h"

CBody_Son::CBody_Son(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Son::CBody_Son(const CBody_Son& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Son::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Son::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_vDiscardMesh[CBody_Son::RENDER_STATE::ATTACK] = { 1 }; // 무기 

	return S_OK;
}

void CBody_Son::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Son::Tick(_float fTimeDelta)
{
	//auto start = chrono::high_resolution_clock::now();

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
	//auto End = chrono::high_resolution_clock::now();
	//chrono::duration<double> duration0 = End - start;
	//cout << "Son Body Tick 실행시간 : " << duration0.count() << endl;
}

void CBody_Son::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Son::Render()
{
	//auto start = chrono::high_resolution_clock::now();
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPass = m_iShaderPass;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		// 		if (i == 0)
		// 			continue;

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	//auto End = chrono::high_resolution_clock::now();
	//chrono::duration<double> duration0 = End - start;
	//cout << "Son Body Render 실행시간 : " << duration0.count() << endl;
	return S_OK;
}

HRESULT CBody_Son::Render_Shadow()
{
	//auto start = chrono::high_resolution_clock::now();
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

	//auto End = chrono::high_resolution_clock::now();
	//chrono::duration<double> duration0 = End - start;
	//cout << "Son Body Shadow 실행시간 : " << duration0.count() << endl;
	return S_OK;
}

void CBody_Son::OnCollisionEnter(CCollider* other)
{
	__super::OnCollisionEnter(other);
}

void CBody_Son::OnCollisionStay(CCollider* other)
{
	__super::OnCollisionStay(other);
}

void CBody_Son::OnCollisionExit(CCollider* other)
{
	__super::OnCollisionExit(other);
}

void CBody_Son::SetUp_Animation(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

HRESULT CBody_Son::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Son"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	_float4x4 Temp = this->Get_BonePtr("Bone020")->Get_CombinedTransformationMatrix();
	_float3 Test = _float3(Temp._41,Temp._42,Temp._43);
	//Test = Test* m_pTransformCom->Get_Position();
	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER);
	BoundingDesc.fRadius = 1.0f;
	BoundingDesc.vCenter = Test;


	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Son::Bind_ShaderResources()
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

	if (m_eRender_State == CBody_Son::RENDER_STATE::ATTACK)
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

CBody_Son* CBody_Son::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Son* pInstance = new CBody_Son(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Son");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Son::Clone(void* pArg)
{
	CBody_Son* pInstance = new CBody_Son(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Son");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Son::Pool()
{
	return new CBody_Son(*this);
}

void CBody_Son::Free()
{
	__super::Free();
}
