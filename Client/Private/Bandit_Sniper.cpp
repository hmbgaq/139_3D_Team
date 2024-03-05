#include "stdafx.h"
#include "Bandit_Sniper.h"
#include "GameInstance.h"
#include "Body_Bandit_Sniper.h"
#include "BanditHeavy_Idle.h"

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
	}

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

HRESULT CBandit_Sniper::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Sniper::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Sniper"), BodyDesc));

	return S_OK;
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
