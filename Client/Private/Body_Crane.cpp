#include "stdafx.h"
#include "Body_Crane.h"
#include "GameInstance.h"

CBody_Crane::CBody_Crane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBody(pDevice, pContext, strPrototypeTag)
{
}

CBody_Crane::CBody_Crane(const CBody_Crane& rhs)
	: CBody(rhs)
{
}

HRESULT CBody_Crane::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBody_Crane::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CBody_Crane::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Crane::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Crane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBody_Crane::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CBody_Crane::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}


HRESULT CBody_Crane::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrnetLevel, L"Prototype_Component_Model_Crane",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
	BoundingDesc.vExtents = _float3(0.0f, 0.0f, 0.0f);
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);

	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc));


	return S_OK;
}

HRESULT CBody_Crane::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

void CBody_Crane::Check_Frustum()
{
	m_bIsInFrustum = true;
}

CBody_Crane* CBody_Crane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBody_Crane* pInstance = new CBody_Crane(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Crane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Crane::Clone(void* pArg)
{
	CBody_Crane* pInstance = new CBody_Crane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Crane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody_Crane::Pool()
{
	return new CBody_Crane(*this);
}

void CBody_Crane::Free()
{
	__super::Free();
}
