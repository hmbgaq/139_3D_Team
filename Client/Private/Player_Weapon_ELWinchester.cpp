#include "stdafx.h"
#include "Player_Weapon_ELWinchester.h"
#include "GameInstance.h"
#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Data_Manager.h"
#include "SMath.h"

CPlayer_Weapon_ELWinchester::CPlayer_Weapon_ELWinchester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext,strPrototypeTag)
{

}

CPlayer_Weapon_ELWinchester::CPlayer_Weapon_ELWinchester(const CPlayer_Weapon_ELWinchester& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_ELWinchester::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_ELWinchester::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	string path = "../Bin/DataFiles/Data_Weapon/Player/ELWinchester/Aim.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

void CPlayer_Weapon_ELWinchester::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_ELWinchester::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_pGameInstance->Key_Down(DIK_F))
	//{
	//	string path = "../Bin/DataFiles/Data_Weapon/Player/ELWinchester/Aim.json";
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

}

void CPlayer_Weapon_ELWinchester::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_ELWinchester::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

		if (i == 1)
		{
			_float3 bloompower = {1.f, 1.f, 1.f };
			_float4 campos = m_pGameInstance->Get_CamPosition();
			_float4 RimColor = { 1.f, 1.f, 1.f, 1.f };
			m_pShaderCom->Bind_RawValue("g_vBloomPower", &bloompower, sizeof(_float3));
			m_pShaderCom->Bind_RawValue("g_vCamPosition", &campos, sizeof(_float4));
			m_pShaderCom->Bind_RawValue("g_vRimColor", &RimColor, sizeof(_float4));
			m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_MESH_BLOOM));
		}
		else
			m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_ORIGIN));
		
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CPlayer_Weapon_ELWinchester::Fire(_float3 vTargetPos, CCharacter* pTarget)
{
	__super::Fire(L"Prototype_GameObject_Bullet_Winchester", LAYER_PLAYER_BULLET, vTargetPos, pTarget);
}

void CPlayer_Weapon_ELWinchester::Play_Weapon_Sound_UnHolster()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"HolsterUnholster_Winchester_01.wav";
		break;
	case 1:
		strFileName = L"HolsterUnholster_Winchester_03.wav";
		break;
	case 2:
		strFileName = L"HolsterUnholster_Winchester_04.wav";
		break;
	default:
		strFileName = L"HolsterUnholster_Winchester_01.wav";
		break;

	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 10.f);

}

void CPlayer_Weapon_ELWinchester::Play_Weapon_Sound_Holster()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"HM_Winchester_AimOff_01.wav";
		break;
	case 1:
		strFileName = L"HM_Winchester_AimOff_02.wav";
		break;
	case 2:
		strFileName = L"HM_Winchester_AimOff_03.wav";
		break;
	default:
		strFileName = L"HM_Winchester_AimOff_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 15.f);

}

void CPlayer_Weapon_ELWinchester::Play_Weapon_Sound_Aim()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"HM_Winchester_AimOn_01.wav";
		break;
	case 1:
		strFileName = L"HM_Winchester_AimOn_02.wav";
		break;
	case 2:
		strFileName = L"HM_Winchester_AimOn_03.wav";
		break;
	case 3:
		strFileName = L"HM_Winchester_AimOn_04.wav";
		break;
	case 4:
		strFileName = L"HM_Winchester_AimOn_05.wav";
		break;
	case 5:
		strFileName = L"HM_Winchester_AimOn_06.wav";
		break;
	default:
		strFileName = L"HM_Winchester_AimOn_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 15.f);

}

void CPlayer_Weapon_ELWinchester::Play_Weapon_Sound_Reload()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"Winchester_Full_Reload_01.wav";
		break;
	case 1:
		strFileName = L"Winchester_Full_Reload_02.wav";
		break;
	case 2:
		strFileName = L"Winchester_Full_Reload_03.wav";
		break;
	default:
		strFileName = L"Winchester_Full_Reload_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 10.f);

}

void CPlayer_Weapon_ELWinchester::Play_Weapon_Sound_Fire()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"Winchester_Fire_Player_01.wav";
		break;
	case 1:
		strFileName = L"Winchester_Fire_Player_02.wav";
		break;
	case 2:
		strFileName = L"Winchester_Fire_Player_03.wav";
		break;
	default:
		strFileName = L"Winchester_Fire_Player_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_ATTACK", strFileName, CHANNELID::SOUND_PLAYER_ATTACK, 10.f);

}

//void CPlayer_Weapon_ELWinchester::Fire(_float3 vTargetPos)
//{
//	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Bullet_Winchester");
//	_float3 vSpawnPos = Get_WorldPosition();
//	_vector vTargetVector = XMLoadFloat3(&Calc_Front_Pos(vTargetPos));
//
//	pBullet->Set_Position(vSpawnPos);
//	pBullet->Get_Transform()->Look_At(vTargetVector);
//}

HRESULT CPlayer_Weapon_ELWinchester::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_ELWinchester"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer_Weapon_ELWinchester* CPlayer_Weapon_ELWinchester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_ELWinchester* pInstance = new CPlayer_Weapon_ELWinchester(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_ELWinchester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_ELWinchester::Clone(void* pArg)
{
	CPlayer_Weapon_ELWinchester* pInstance = new CPlayer_Weapon_ELWinchester(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_ELWinchester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_ELWinchester::Pool()
{
	return new CPlayer_Weapon_ELWinchester(*this);
}

void CPlayer_Weapon_ELWinchester::Free()
{
	__super::Free();
}
