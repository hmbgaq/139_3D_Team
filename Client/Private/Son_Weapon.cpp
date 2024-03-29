#include "stdafx.h"
#include "Son_Weapon.h"

#include "GameInstance.h"
#include "Bone.h"


CSon_Weapon::CSon_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{

}

CSon_Weapon::CSon_Weapon(const CSon_Weapon& rhs)
	: CWeapon(rhs)
{
}

HRESULT CSon_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSon_Weapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSon_Weapon::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CSon_Weapon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CSon_Weapon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSon_Weapon::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CSon_Weapon::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}


HRESULT CSon_Weapon::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSon_Weapon::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}


// CSon_Weapon * CSon_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
// {
// 	CSon_Weapon*		pInstance = new CSon_Weapon(pDevice, pContext);
// 
// 	/* 원형객체를 초기화한다.  */
// 	if (FAILED(pInstance->Initialize_Prototype()))
// 	{
// 		MSG_BOX("Failed to Created : CSon_Weapon");
// 		Safe_Release(pInstance);
// 	}
// 	return pInstance;
// }
// 
CGameObject* CSon_Weapon::Clone(void* pArg)
{
	CSon_Weapon* pInstance = new CSon_Weapon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSon_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon_Weapon::Pool()
{
	return new CSon_Weapon(*this);
}

void CSon_Weapon::Free()
{
	__super::Free();
}

