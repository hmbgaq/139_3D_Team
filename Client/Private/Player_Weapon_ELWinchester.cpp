#include "stdafx.h"
#include "Player_Weapon_ELWinchester.h"
#include "GameInstance.h"
#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Data_Manager.h"

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
}

void CPlayer_Weapon_ELWinchester::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_ELWinchester::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_ELWinchester::Fire()
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Bullet_Winchester");
	_float3 vSpawnPos = Get_WorldPosition();
	_float3 vTargetPos = Calc_Front_Pos(_float3(0.f, 0.f, 1.f));
	//_float3 vTargetPos = Calc_Front_Pos(_float3(-1.f, 0.f, 0.f));

	_vector vTargetVector = XMLoadFloat3(&vTargetPos);

	pBullet->Set_Position(vSpawnPos);
	pBullet->Get_Transform()->Look_At(vTargetVector);

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_ShakeCamera(true);
	}
}

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
