#include "stdafx.h"
#include "..\Public\Weapon_Player.h"

#include "GameInstance.h"
#include "Bone.h"


CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{

}

CWeapon_Player::CWeapon_Player(const CWeapon_Player & rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{	
	if (FAILED(__super::Initialize_Prototype())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	//m_pTransformCom->Set_Scaling(0.1f, 0.1f, 0.1f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.7f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CWeapon_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Player::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CWeapon_Player::Render_Shadow()
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


HRESULT CWeapon_Player::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_ForkLift"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;




	return S_OK;
}

HRESULT CWeapon_Player::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;
	
	return S_OK;
}


// CWeapon_Player * CWeapon_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
// {
// 	CWeapon_Player*		pInstance = new CWeapon_Player(pDevice, pContext);
// 
// 	/* 원형객체를 초기화한다.  */
// 	if (FAILED(pInstance->Initialize_Prototype()))
// 	{
// 		MSG_BOX("Failed to Created : CWeapon_Player");
// 		Safe_Release(pInstance);
// 	}
// 	return pInstance;
// }
// 
 CGameObject * CWeapon_Player::Clone(void* pArg)
 {
 	CWeapon_Player*		pInstance = new CWeapon_Player(*this);
 
 	/* 원형객체를 초기화한다.  */
 	if (FAILED(pInstance->Initialize(pArg)))
 	{
 		MSG_BOX("Failed to Cloned : CWeapon_Player");
 		Safe_Release(pInstance);
 	}
 	return pInstance;
 }

CGameObject* CWeapon_Player::Pool()
{
	return new CWeapon_Player(*this);
}

void CWeapon_Player::Free()
{
	__super::Free();
}

