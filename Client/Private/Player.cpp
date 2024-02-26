#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Weapon_Player.h"
#include "Body_Player.h"

#include "Data_Manager.h"
#include "Clone_Manager.h"

#include "TestEvent.h"
#include "TestEventWithActor.h"
#include "TestEventWithPlayer.h"

#include "PhysXController.h"
#include "PhysXCharacterController.h"



CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{	
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	CData_Manager::GetInstance()->Set_Player(this);
	

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	//CData_Manager::GetInstance()->Reset_Player((LEVEL)m_pGameInstance->Get_NextLevel());
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	CBody_Player*		pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));
	Safe_AddRef(pBody);

	if (m_pGameInstance->Get_NextLevel() != _uint(LEVEL_TOOL))
	{
		if (GetKeyState(VK_DOWN) & 0x8000)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		}
		if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
		if (GetKeyState(VK_UP) & 0x8000)
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			pBody->SetUp_Animation(4);
		}
		else
			pBody->SetUp_Animation(3);
	}
	else if (m_pGameInstance->Get_NextLevel() == _uint(LEVEL_TOOL))
	{
		if (m_pGameInstance->Key_Pressing(DIK_DOWNARROW))
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_LEFTARROW))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHTARROW))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_UPARROW))
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}
		//TODO 여기다가 절대로 상시 애니메이션 걸지 마세요 애니메이션툴안돌아갑니다 모든 몬스터든 뭐든 상시 애니메이션 걸면안됌 준비 다된놈만 애니메이션 걸어두셈 
		//else
		//	pBody->SetUp_Animation(3);
	}


	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	Safe_Release(pBody);

	__super::Tick(fTimeDelta);

	//if (m_pGameInstance->Key_Down(DIK_E))
	//{
	//	//IEvent* pEvent = CTestEvent::Create();
	//	//IEvent* pEvent = CTestEventWithActor::Create(this);
	//	IEvent* pEvent = CTestEventWithPlayer::Create(this);
	//
	//	m_pGameInstance->Add_Event(pEvent);
	//}


}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = 0;

	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

//	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Navigation2"),
//		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
//		return E_FAIL;

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_PhysXController"),
		TEXT("Com_PhysXCharacterController"), reinterpret_cast<CComponent**>(&m_pPhysXControllerCom))))
		return E_FAIL;
	
	m_pPhysXControllerCom->Init_Controller(Preset::PhysXControllerDesc::PlayerSetting(m_pTransformCom), (_uint)PHYSX_COLLISION_LAYER::PLAYER);



	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* For.Part_Body */
	CBody_Player::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Player"), BodyDesc)))
		return E_FAIL;

	//{
	//	CWeapon_Player::WEAPON_DESC	WeaponDesc = {};
	//	if (FAILED(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Player"), "SWORD", WeaponDesc, TEXT("Weapon_L"))))
	//		return E_FAIL;
	//}
	//CWeapon* m_pWeapon_L = Get_Weapon(TEXT("Weapon_L"));


	return S_OK;
}


void CPlayer::Test_Create_Monster()
{
	CGameObject* pMonster = CClone_Manager::GetInstance()->Clone_Object<CGameObject>(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("Prototype_GameObject_Monster"));
	if (pMonster)
	{
		_float3 vPos = Get_Transform()->Get_Position();
		pMonster->Get_Transform()->Set_Position(vPos);
	}
	else
	{
		_bool test = false;
	}
}

void CPlayer::Activate()
{
	Test_Create_Monster();
}

_bool CPlayer::Activate_Condition()
{
	return true;
}

_bool CPlayer::End_Condition()
{
	return true;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void* pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Pool()
{
	return new CPlayer(*this);
}

void CPlayer::Free()
{
	__super::Free();
}



