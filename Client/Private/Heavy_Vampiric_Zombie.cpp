#include "stdafx.h"
#include "Heavy_Vampiric_Zombie.h"
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Player.h"
#include "UI_Manager.h"
#include "UI_EnemyHP_Shard.h"
//#include "Body_Heavy_Vampiric_Zombie.h"
//#include "BanditHeavy_Idle.h"

CHeavy_Vampiric_Zombie::CHeavy_Vampiric_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CBandit_Heavy(pDevice, pContext, strPrototypeTag)
{
}

CHeavy_Vampiric_Zombie::CHeavy_Vampiric_Zombie(const CHeavy_Vampiric_Zombie& rhs)
	: CBandit_Heavy(rhs)
{
}

HRESULT CHeavy_Vampiric_Zombie::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CHeavy_Vampiric_Zombie::Initialize(void* pArg)
{
	m_fMaxHp = 100.f;
	m_fHp = m_fMaxHp;

	FAILED_CHECK(__super::Initialize(pArg));

	/* !성희 추가 : 몬스터 HUD 생성 */ // 생성함수 호출시 CMonster_Character에게 상속받은 m_pEnemyHUD 멤버변수 사용가능.
	Ready_EnemyHUD_Shard(m_pGameInstance->Get_NextLevel(), this);

	m_pTarget = CData_Manager::GetInstance()->Get_Player();

	return S_OK;
}

void CHeavy_Vampiric_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHeavy_Vampiric_Zombie::Tick(_float fTimeDelta)
{ 
	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	__super::Tick(fTimeDelta);

}

void CHeavy_Vampiric_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CHeavy_Vampiric_Zombie::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CHeavy_Vampiric_Zombie::Ready_Components()
{
	return S_OK;
}

HRESULT CHeavy_Vampiric_Zombie::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Heavy_Vampiric_Zombie"), BodyDesc));

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Heavy_Vampiric_Zombie"), "LeftInHandMiddle", WeaponDesc, BANDIT_HEAVY_WEAPON));
	}

	Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, false);

	return S_OK;
}

void CHeavy_Vampiric_Zombie::Check_Frustum()
{
	m_bIsInFrustum = true;
}

CHeavy_Vampiric_Zombie* CHeavy_Vampiric_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CHeavy_Vampiric_Zombie* pInstance = new CHeavy_Vampiric_Zombie(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHeavy_Vampiric_Zombie");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHeavy_Vampiric_Zombie::Clone(void* pArg)
{
	CHeavy_Vampiric_Zombie* pInstance = new CHeavy_Vampiric_Zombie(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHeavy_Vampiric_Zombie");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHeavy_Vampiric_Zombie::Pool()
{
	return new CHeavy_Vampiric_Zombie(*this);
}

void CHeavy_Vampiric_Zombie::Free()
{
	__super::Free();

}
