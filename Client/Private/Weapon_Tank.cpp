#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Tank.h"

CWeapon_Tank::CWeapon_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Tank::CWeapon_Tank(const CWeapon_Tank& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Tank::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Tank::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Load_Json());

	FAILED_CHECK(Option_Setting());

	return S_OK;
}

HRESULT CWeapon_Tank::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Tank_Weapon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For. Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	return S_OK;
}

HRESULT CWeapon_Tank::Load_Json()
{
	string path = "../Bin/DataFiles/Data_Monster/Tank/Weapon.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

HRESULT CWeapon_Tank::Option_Setting()
{
	m_iRenderPass = ECast(MONSTER_SHADER::COMMON_ORIGIN);

	return S_OK;
}

void CWeapon_Tank::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Tank::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Tank::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Tank::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	//if (m_iRenderPass == ECast(MONSTER_SHADER::SNIPER_WEAPON))
	//{
	//	/* Camera */
	//	m_fCamFar = m_pGameInstance->Get_CamFar();
	//	m_vCamPos = m_pGameInstance->Get_CamPosition();
	//	m_pShaderCom->Bind_RawValue("g_fCamFar", &m_fCamFar, sizeof(_float));
	//	m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));

	//	/* RimLight */
	//	m_vRimColor = { 0.0f, 0.0f, 0.f, 1.f };
	//	m_vBloomPower = _float3(0.7f, 0.7f, 0.7f);
	//	m_fRimPower = 5.f;

	//	m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
	//	m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_vBloomPower, sizeof(_float3));
	//	m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));
	//}

	return S_OK;
}

HRESULT CWeapon_Tank::Render()
{
	//auto start = chrono::high_resolution_clock::now();

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(m_iRenderPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CWeapon_Tank::Sniping(_float4 vTargetPos, _float3 StartfPos)
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Bullet_Tank");

	pBullet->Set_Position(StartfPos);
	pBullet->Get_Transform()->Look_At(vTargetPos);
}

#pragma region Create, Clone, Pool, Free

CWeapon_Tank* CWeapon_Tank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Tank* pInstance = new CWeapon_Tank(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Tank::Clone(void* pArg)
{
	CWeapon_Tank* pInstance = new CWeapon_Tank(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Tank::Pool()
{
	return new CWeapon_Tank(*this);
}

void CWeapon_Tank::Free()
{
	__super::Free();
}


#pragma endregion
