#include "stdafx.h"
#include "Bandit_Heavy.h"
#include "GameInstance.h"
#include "Body_Bandit_Heavy.h"
#include "BanditHeavy_Idle.h"
#include "Data_Manager.h"
#include "Player.h"

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
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));

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
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

	if (nullptr == m_pTarget && m_pGameInstance->Key_Pressing(DIK_V))
	{
		m_pTarget = CData_Manager::GetInstance()->Get_Player();
	}

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
