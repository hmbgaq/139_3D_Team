#include "stdafx.h"
#include "Tank.h"
#include "GameInstance.h"
#include "Body_Tank.h"
#include "Weapon_Tank.h"
#include "Tank_Idle.h"
#include "Data_Manager.h"
#include "Player.h"

#include "Tank_HitHeavyShield_FL_01.h"
#include "Tank_HitHeavyShield_FR_01.h"
#include "Tank_HitHeavyShield_F_01.h"
#include "Tank_HitNormalShield_FL_01.h"
#include "Tank_HitNormalShield_FR_01.h"
#include "Tank_HitNormalShield_F_01.h"

#include "Tank_Stun_Start.h"
#include "Tank_DeathNormal_F_01.h"


CTank::CTank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CTank::CTank(const CTank& rhs)
	: CMonster_Character(rhs)
{
}

void CTank::Set_ShieldBroken()
{
	CWeapon_Tank* pWeapon_Tank = dynamic_cast<CWeapon_Tank*>(Get_Weapon(L"Weapon_Shield"));
	if (pWeapon_Tank)
	{
		if (CModel::ANIM_STATE_NORMAL != pWeapon_Tank->Get_AnimState()) 
		{
			pWeapon_Tank->Set_Animation(0, CModel::ANIM_STATE_NORMAL, 0);
		}
		
	}
		
	m_fShieldBrokenTime = 10.f;
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

	m_iHp = 100000;

	//m_pTarget = CData_Manager::GetInstance()->Get_Player();
	//m_pNavigationCom = nullptr;

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

	if (Is_ShieldBroken())
	{
		Update_ShieldBrokenTime(fTimeDelta);
	}

	if (nullptr == m_pTarget && m_pGameInstance->Key_Pressing(DIK_V))
	{
		m_pTarget = CData_Manager::GetInstance()->Get_Player();
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

void CTank::Hitted_Left(Power ePower)
{
	switch (ePower)
	{
	case Engine::Medium:
		m_pActor->Set_State(new CTank_HitNormalShield_FL_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CTank_HitHeavyShield_FL_01());
	}
}

void CTank::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Medium:
		m_pActor->Set_State(new CTank_HitNormalShield_FR_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CTank_HitHeavyShield_FR_01());
	}
}

void CTank::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Medium:
		m_pActor->Set_State(new CTank_HitNormalShield_F_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CTank_HitHeavyShield_F_01());
	}
}

void CTank::Hitted_Stun(Power ePower)
{
	m_pActor->Set_State(new CTank_Stun_Start());
}

void CTank::Hitted_Dead(Power ePower)
{
	m_pActor->Set_State(new CTank_DeathNormal_F_01());
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
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("LeftHandIK");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Tank"), "LeftHandIK", WeaponDesc, TEXT("Weapon_Shield")));

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
