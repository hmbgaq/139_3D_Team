#include "stdafx.h"
#include "..\Public\Body_Hawk.h"
#include "GameInstance.h"
#include "Character.h"
#include "Hawk.h"


CBody_Hawk::CBody_Hawk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Hawk::CBody_Hawk(const CBody_Hawk& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Hawk::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Hawk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBody_Hawk::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Hawk::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Hawk::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Hawk::Render()
{

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPass = m_iShaderPass;

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(iPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody_Hawk::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CBody_Hawk::OnCollisionEnter(CCollider* other)
{

}

void CBody_Hawk::OnCollisionStay(CCollider* other)
{
	CGameObject* pOwner = Get_Object_Owner();

	if (nullptr == pOwner || false == pOwner->Get_Enable() || true == pOwner->Is_Dead())
		return;

	CHawk* pHawk = dynamic_cast<CHawk*>(pOwner);
	if (nullptr == pHawk)
		return;

	pHawk->FlyAway();

	Collider_Off();

	
}

void CBody_Hawk::OnCollisionExit(CCollider* other)
{
	
}

HRESULT CBody_Hawk::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Hawk"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::OBSTACLE);
	BoundingDesc.fRadius = 5.f;

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBody_Hawk::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	_float fCamFar = m_pGameInstance->Get_CamFar();
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float)));

	return S_OK;
}

CBody_Hawk* CBody_Hawk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Hawk* pInstance = new CBody_Hawk(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Hawk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Hawk::Clone(void* pArg)
{
	CBody_Hawk* pInstance = new CBody_Hawk(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Hawk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Hawk::Pool()
{
	return new CBody_Hawk(*this);
}

void CBody_Hawk::Free()
{
	__super::Free();
}
