#include "stdafx.h"
#include "Tank.h"
#include "GameInstance.h"
#include "Body_Tank.h"
#include "Weapon_Tank.h"
#include "Tank_Idle.h"

CTank::CTank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CTank::CTank(const CTank& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CTank::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CTank::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CTank>(this);
		m_pActor->Set_State(new CTank_Idle());
	}

	return S_OK;
}

void CTank::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CTank::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

}

void CTank::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CTank::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CTank::Ready_Components()
{
	return S_OK;
}

HRESULT CTank::Ready_PartObjects()
{
	/* For. Body */
	{
		CBody::BODY_DESC		BodyDesc = {};
		FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Tank"), BodyDesc));
	}

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("RightHandIK");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Tank"), "RightHandIK", WeaponDesc, TEXT("Weapon_Shield")));

		//CWeapon* m_pWeapon = Get_Weapon(TEXT("Weapon_Punch"));
		//m_pWeapon->Set_Enable(false);
	}


	return S_OK;
}

CTank* CTank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CTank* pInstance = new CTank(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTank::Clone(void* pArg)
{
	CTank* pInstance = new CTank(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTank::Pool()
{
	return new CTank(*this);
}

void CTank::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
