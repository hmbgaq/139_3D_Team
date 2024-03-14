#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Bandit_Sniper.h"

CWeapon_Bandit_Sniper::CWeapon_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Bandit_Sniper::CWeapon_Bandit_Sniper(const CWeapon_Bandit_Sniper& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Bandit_Sniper::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Load_Json());

	FAILED_CHECK(Option_Setting());

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Bandit_Sniper_Weapon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For. Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Load_Json()
{
	string path = "../Bin/DataFiles/Data_Monster/Sniper/Weapon.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Option_Setting()
{
	iRenderPass = ECast(MONSTER_SHADER::COMMON_ORIGIN);

	return S_OK;
}

void CWeapon_Bandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Bandit_Sniper::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->Key_Down(DIK_H))
	//{
	//	string path = "../Bin/DataFiles/Data_Monster/Sniper/Weapon.json";

	//	{
	//		json Out_Json;
	//		m_pTransformCom->Write_Json(Out_Json);
	//		CJson_Utility::Save_Json(path.c_str(), Out_Json);
	//	}
	//	//{
	//	//   json In_Json;
	//	//   CJson_Utility::Load_Json(path.c_str(), In_Json);
	//	//   m_pTransformCom->Load_FromJson(In_Json);
	//	//}

	//}

	__super::Tick(fTimeDelta);
}

void CWeapon_Bandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Bandit_Sniper::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(iRenderPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CWeapon_Bandit_Sniper::Sniping(_float4 vTargetPos, _float3 StartfPos)
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Bullet_Bandit_Sniper");

	pBullet->Set_Position(StartfPos);
	pBullet->Get_Transform()->Look_At(vTargetPos);
}

#pragma region Create, Clone, Pool, Free

CWeapon_Bandit_Sniper* CWeapon_Bandit_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Bandit_Sniper* pInstance = new CWeapon_Bandit_Sniper(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Bandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Bandit_Sniper::Clone(void* pArg)
{
	CWeapon_Bandit_Sniper* pInstance = new CWeapon_Bandit_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Bandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Bandit_Sniper::Pool()
{
	return new CWeapon_Bandit_Sniper(*this);
}

void CWeapon_Bandit_Sniper::Free()
{
	__super::Free();
}


#pragma endregion
