#include "stdafx.h"
#include "Heavy_Vampiric_Zombie.h"
#include "GameInstance.h"
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
	FAILED_CHECK(__super::Initialize(pArg));



	return S_OK;
}

void CHeavy_Vampiric_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHeavy_Vampiric_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fHp = 100;
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
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Heavy_Vampiric_Zombie"), "LeftInHandMiddle", WeaponDesc, TEXT("Weapon_Axe")));
	}


	return S_OK;
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
