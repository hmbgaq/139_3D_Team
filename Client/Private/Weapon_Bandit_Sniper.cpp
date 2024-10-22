#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Bandit_Sniper.h"
#include "Effect_Manager.h"

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
	m_iRenderPass = ECast(MONSTER_SHADER::COMMON_ORIGIN);

	return S_OK;
}

void CWeapon_Bandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Bandit_Sniper::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Bandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Bandit_Sniper::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	/* Camera */
	m_fCamFar = m_pGameInstance->Get_CamFar();
	m_vCamPos = m_pGameInstance->Get_CamPosition();
	m_pShaderCom->Bind_RawValue("g_fCamFar", &m_fCamFar, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPos, sizeof(_float4));

	/* RimLight */
	//m_vRimColor = { 0.0f, 0.0f, 0.f, 1.f };
	//m_vBloomPower = _float3(0.7f, 0.7f, 0.7f);
	//m_fRimPower = 5.f;
	//
	//m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));
	//m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_vBloomPower, sizeof(_float3));
	//m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));

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

		m_pShaderCom->Begin(m_iRenderPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Render_CSM(_uint i)
{
	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Render_OutLine()
{
	return S_OK;
}

void CWeapon_Bandit_Sniper::Sniping(_float4 vTargetPos, _float3 StartfPos)
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Bullet_Bandit_Sniper");

	pBullet->Set_Position(StartfPos);
	pBullet->Get_Transform()->Look_At(vTargetPos);

	wstring strGroupKey = L"";
	wstring strSoundKey = L"";
	_float fVolume = 8.5f;

	_vector		vDir = vTargetPos - StartfPos;
	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	_int iRandomSound = m_pGameInstance->Random_Int(1, 5);

	if (fDistance >= 15.f)
	{
		strGroupKey = L"SNIPER_ATTACKLONG";
		fVolume = 7.f;

		switch (iRandomSound)
		{
		case 1:
			strSoundKey = L"bandit_attack_revolver_shotA_distant001.wav";
			break;
		case 2:
			strSoundKey = L"bandit_attack_revolver_shotA_distant002.wav";
			break;
		case 3:
			strSoundKey = L"bandit_attack_revolver_shotA_distant003.wav";
			break;
		case 4:
			strSoundKey = L"bandit_attack_revolver_shotA_distant004.wav";
			break;
		case 5:
			strSoundKey = L"bandit_attack_revolver_shotA_distant005.wav";
			break;
		}
	}
	else
	{
		strGroupKey = L"SNIPER_ATTACKSHORT";

		switch (iRandomSound)
		{
		case 1:
			strSoundKey = L"bandit_attack_revolver_shotA_close001.wav";
			break;
		case 2:
			strSoundKey = L"bandit_attack_revolver_shotA_close002.wav";
			break;
		case 3:
			strSoundKey = L"bandit_attack_revolver_shotA_close003.wav";
			break;
		case 4:
			strSoundKey = L"bandit_attack_revolver_shotA_close004.wav";
			break;
		case 5:
			strSoundKey = L"bandit_attack_revolver_shotA_close005.wav";
			break;
		}
	}


	m_pGameInstance->Play_Sound(strGroupKey, strSoundKey, SOUND_ENEMY_ATTACK, fVolume);
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
