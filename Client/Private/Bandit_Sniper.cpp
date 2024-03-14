#include "stdafx.h"
#include "GameInstance.h"
#include "Bandit_Sniper.h"
#include "Data_Manager.h"
#include "Body_Bandit_Sniper.h"
#include "Weapon_Bandit_Sniper.h"

/* State */
#include "Sniper_CoverLow_Idle.h"
#include "Sniper_DeathLight_B_01.h"
#include "Sniper_Weakspot_Death_01.h"
#include "Sniper_DeathNormal_B_01.h"
#include "Sniper_HitHeavy_FL_01.h"
#include "Sniper_KnockFrontLight_F_01.h"
#include "Sniper_HitHeavy_FR_01.h"
#include "Sniper_KnockFrontLight_F_02.h"
#include "Sniper_HitHeavy_F_01.h"

CBandit_Sniper::CBandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CBandit_Sniper::CBandit_Sniper(const CBandit_Sniper& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CBandit_Sniper::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBandit_Sniper::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	FAILED_CHECK(__super::Initialize(&GameObjectDesc));


	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CBandit_Sniper>(this);
		m_pActor->Set_State(new CSniper_CoverLow_Idle());
	}

	FAILED_CHECK(Ready_Option());
	
	m_pTransformCom->Set_Look(0.f, 0.f, -1.f);

	return S_OK;
}

void CBandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBandit_Sniper::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

}

void CBandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBandit_Sniper::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CBandit_Sniper::Set_ColliderSize(_float fSizeX, _float fSizeY, _float fSizeZ)
{
	CBody_Bandit_Sniper* pBody = dynamic_cast<CBody_Bandit_Sniper*>(m_pBody);
	NULL_CHECK_RETURN(pBody, );
	
	pBody->Get_Collider()->Set_ColliderSize(fSizeX, fSizeY, fSizeZ);
}

void CBandit_Sniper::Sniping_Target(_float4 TargetPos)
{
	CWeapon_Bandit_Sniper* pWeapon = dynamic_cast<CWeapon_Bandit_Sniper*>(m_pWeapon);
	NULL_CHECK_RETURN(pWeapon, );

	m_iBulletCnt += 1;
	pWeapon->Sniping(TargetPos, m_pTransformCom->Get_Pos()); // (_float4 vTargetPos, _float3 StartfPos)
}

HRESULT CBandit_Sniper::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Sniper::Ready_PartObjects()
{
	/* For. Body */
	{
		CBody::BODY_DESC		BodyDesc = {};
		BodyDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Sniper"), BodyDesc));
	}

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("RightHandProp");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Bandit_Sniper"), "RightHandProp", WeaponDesc, TEXT("Weapon_Sniper")));
	
		m_pWeapon = Get_Weapon(TEXT("Weapon_Sniper"));
		m_pWeapon->Set_Enable(true);
	}

	return S_OK;
}

HRESULT CBandit_Sniper::Ready_Option()
{
	m_bProtectExist = true; /* 현재 방어막 있는상태 */
	m_pTarget = m_pGameInstance->Get_Player(); /* 타겟은 플레이어 고정 */ 
	
	return S_OK;
}

void CBandit_Sniper::Hitted_Left(Power ePower)
{
	/* 무기 강도 */
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CSniper_HitHeavy_FL_01());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CSniper_HitHeavy_FL_01());
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CSniper_KnockFrontLight_F_01());
		break;
	default:
		m_pActor->Set_State(new CSniper_HitHeavy_FL_01());
		break;
	}
}

void CBandit_Sniper::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		cout << "Light " << endl;
		m_pActor->Set_State(new CSniper_HitHeavy_FR_01());
		break;
	case Engine::Medium:
		cout << "Medium " << endl;
		m_pActor->Set_State(new CSniper_HitHeavy_FR_01());
		break;
	case Engine::Heavy:
		cout << "Heavy " << endl;
		m_pActor->Set_State(new CSniper_KnockFrontLight_F_02());
		break;
	default:
		m_pActor->Set_State(new CSniper_HitHeavy_FR_01());
		break;
	}
}

void CBandit_Sniper::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		cout << "Light " << endl;
		m_pActor->Set_State(new CSniper_HitHeavy_F_01());
		break;
	case Engine::Medium:
		cout << "Medium " << endl;
		m_pActor->Set_State(new CSniper_HitHeavy_F_01());
		break;
	case Engine::Heavy:
		cout << "Heavy " << endl;
		m_pActor->Set_State(new CSniper_HitHeavy_F_01());
		break;
	default:
		m_pActor->Set_State(new CSniper_HitHeavy_F_01());
		break;
	}
}

void CBandit_Sniper::Hitted_Knock(_bool bIsCannonball)
{
	m_pActor->Set_State(new CSniper_HitHeavy_F_01());

}

void CBandit_Sniper::Hitted_Dead(Power ePower)
{
	switch (ePower)
	{
	case Engine::Light:
		m_pActor->Set_State(new CSniper_DeathLight_B_01());
		break;
	case Engine::Medium:
		m_pActor->Set_State(new CSniper_Weakspot_Death_01());
		break;
	case Engine::Heavy:
		m_pActor->Set_State(new CSniper_DeathNormal_B_01());
		break;

	default:
		m_pActor->Set_State(new CSniper_DeathNormal_B_01());
		break;
	}
}

CBandit_Sniper* CBandit_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBandit_Sniper* pInstance = new CBandit_Sniper(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBandit_Sniper::Clone(void* pArg)
{
	CBandit_Sniper* pInstance = new CBandit_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBandit_Sniper::Pool()
{
	return new CBandit_Sniper(*this);
}

void CBandit_Sniper::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
