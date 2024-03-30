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
#include "Finisher_Tank.h"

#include "Player.h"
#include "Player_Finisher_Tank.h"


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
	Reset_AttackCount();

	CWeapon_Tank* pWeapon_Tank = dynamic_cast<CWeapon_Tank*>(Get_Weapon(L"Weapon_Shield"));
	if (pWeapon_Tank)
	{
		pWeapon_Tank->Set_Follow(false);
		if (CModel::ANIM_STATE_NORMAL != pWeapon_Tank->Get_AnimState()) 
		{
			pWeapon_Tank->Set_Animation(0, CModel::ANIM_STATE_NORMAL, 0);

		}
		
	}
		
	m_fShieldBrokenTime = 20.f;
}

HRESULT CTank::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CTank::Initialize(void* pArg)
{

	if (pArg == nullptr)
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

		GameObjectDesc.fSpeedPerSec = 10.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(&GameObjectDesc));
	}
	else
	{
		CMonster_Character::MONSTER_DESC MonsterDesc = *(CMonster_Character::MONSTER_DESC*)pArg;

		MonsterDesc.fSpeedPerSec = 10.f;
		MonsterDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(pArg));
	}


	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CTank>(this);
		m_pActor->Set_State(new CTank_Idle());
	}

	//m_iHp = 150;
	m_fHp = 100;


	m_bIsFixed = true;

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

	m_bIsFixed = !Is_ShieldBroken();

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
	if (true == Is_ShieldBroken())
	{
		switch (ePower)
		{
		case Engine::Medium:
			m_pActor->Set_State(new CTank_HitNormalShield_FL_01());
		case Engine::Heavy:
			m_pActor->Set_State(new CTank_HitHeavyShield_FL_01());
		}
	}

}

void CTank::Hitted_Right(Power ePower)
{
	if (true == Is_ShieldBroken())
	{
		switch (ePower)
		{
		case Engine::Medium:
			m_pActor->Set_State(new CTank_HitNormalShield_FR_01());
		case Engine::Heavy:
			m_pActor->Set_State(new CTank_HitHeavyShield_FR_01());
		}
	}

}

void CTank::Hitted_Front(Power ePower)
{
	if (true == Is_ShieldBroken())
	{
		switch (ePower)
		{
		case Engine::Medium:
			m_pActor->Set_State(new CTank_HitNormalShield_F_01());
		case Engine::Heavy:
			m_pActor->Set_State(new CTank_HitHeavyShield_F_01());
		}
	}
}

void CTank::Hitted_Stun(Power ePower)
{
	m_pActor->Set_State(new CTank_Stun_Start());
}

void CTank::Hitted_Finish()
{
	m_pActor->Set_State(new CFinisher_Tank());
	Look_At_OnLand(CData_Manager::GetInstance()->Get_Player()->Get_Position_Vector());
		
	CPlayer* pPlayer = Set_Player_Finisher_Pos(_float3(0.f, 0.f, 2.5f));
	_vector vFront = XMLoadFloat3(&Calc_Front_Pos(_float3(-1.2f, 0.f, 0.5f)));
	pPlayer->Look_At_OnLand(vFront);
	pPlayer->Get_Actor()->Set_State(new CPlayer_Finisher_Tank());
}

void CTank::Hitted_Dead(Power ePower)
{
	m_pActor->Set_State(new CTank_DeathNormal_F_01());
}

void CTank::Create_GroundWave()
{
	CGameObject* pGroundWave = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_MONSTER_BULLET, L"Prototype_GameObject_Projectile_GroundWave");
	pGroundWave->Set_Position(Get_Position());

	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	pGroundWave->Get_Transform()->Look_At(pPlayer->Get_Position_Vector());
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
	}

	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Punch_Tank"), "LeftHand", WeaponDesc, TANK_WEAPON_PUNCH_L));
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Punch_Tank"), "RightHand", WeaponDesc, TANK_WEAPON_PUNCH_R));

		Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
		Set_Weapon_Enable(TANK_WEAPON_PUNCH_R, false);

		Set_Weapon_Collisions_Enable(TEXT("Weapon_Shield"), true);

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
