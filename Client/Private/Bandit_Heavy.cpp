#include "stdafx.h"
#include "Bandit_Heavy.h"
#include "GameInstance.h"
#include "Body_Bandit_Heavy.h"
#include "BanditHeavy_Idle.h"
#include "Data_Manager.h"
#include "Player.h"
#include "SMath.h"

#include "BanditHeavy_HitNormal_F_01.h"
#include "BanditHeavy_HitNormal_F_02.h"
#include "BanditHeavy_HitNormal_L_01.h"
#include "BanditHeavy_HitNormal_R_01.h"
#include "BanditHeavy_HitHeavy_F_01.h"
#include "BanditHeavy_DeathHeavy_F_01.h"


CBandit_Heavy::CBandit_Heavy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CBandit_Heavy::CBandit_Heavy(const CBandit_Heavy& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CBandit_Heavy::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBandit_Heavy::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

		GameObjectDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		FAILED_CHECK(__super::Initialize(&GameObjectDesc));
	}
	else
	{
		CGameObject::GAMEOBJECT_DESC		GameObjectDesc = *(CGameObject::GAMEOBJECT_DESC*)pArg;

		if (GameObjectDesc.eDescType == CGameObject::MONSTER_DESC)
		{
			CMonster_Character::MONSTER_DESC MonsterDesc = *(CMonster_Character::MONSTER_DESC*)pArg;

			MonsterDesc.fSpeedPerSec = 7.f;
			MonsterDesc.fRotationPerSec = XMConvertToRadians(90.0f);

			FAILED_CHECK(__super::Initialize(&MonsterDesc));
		}
		else
		{
			GameObjectDesc.fSpeedPerSec = 7.f;
			GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
			FAILED_CHECK(__super::Initialize(&GameObjectDesc));
		}
		
	}

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CBandit_Heavy>(this);
		m_pActor->Set_State(new CBanditHeavy_Idle());
	}

	return S_OK;
}

void CBandit_Heavy::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBandit_Heavy::Tick(_float fTimeDelta)
{
	/* !성희 추가 : 몬스터 HUD 위치 갱신 */
	Check_EnemyHUD_World(m_pTransformCom->Get_WorldMatrix()/*, vOffsetPos*/);

	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}


	if (nullptr == m_pTarget)
	{
		Search_Target(10.f);
	}


	//if (nullptr == m_pTarget && m_pGameInstance->Key_Pressing(DIK_V))
	//{
	//	m_pTarget = CData_Manager::GetInstance()->Get_Player();
	//}

}

void CBandit_Heavy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBandit_Heavy::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CBandit_Heavy::Hitted_Left(Power ePower)
{
	switch (ePower)
	{
	case Engine::Medium:
		m_pActor->Set_State(new CBanditHeavy_HitNormal_L_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CBanditHeavy_HitHeavy_F_01());
	}
}

void CBandit_Heavy::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Medium:
		m_pActor->Set_State(new CBanditHeavy_HitNormal_R_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CBanditHeavy_HitHeavy_F_01());
	}
}

void CBandit_Heavy::Hitted_Front(Power ePower)
{
	_uint iRand = SMath::Random(0, 1);

	switch (ePower)
	{
	case Engine::Medium:
		if (1 == iRand)
			m_pActor->Set_State(new CBanditHeavy_HitNormal_F_02());
		else 
			m_pActor->Set_State(new CBanditHeavy_HitNormal_F_01());
	case Engine::Heavy:
		m_pActor->Set_State(new CBanditHeavy_HitHeavy_F_01());
	}
}

void CBandit_Heavy::Hitted_Dead(Power ePower)
{
	m_pActor->Set_State(new CBanditHeavy_DeathHeavy_F_01());
}

HRESULT CBandit_Heavy::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Heavy::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Heavy"), BodyDesc));

	return S_OK;
}

//CBandit_Heavy* CBandit_Heavy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
//{
//	CBandit_Heavy* pInstance = new CBandit_Heavy(pDevice, pContext, strPrototypeTag);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX("Failed to Created : CBandit_Heavy");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}

//CGameObject* CBandit_Heavy::Clone(void* pArg)
//{
//	CBandit_Heavy* pInstance = new CBandit_Heavy(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Cloned : CBandit_Heavy");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CGameObject* CBandit_Heavy::Pool()
//{
//	return new CBandit_Heavy(*this);
//}

void CBandit_Heavy::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
