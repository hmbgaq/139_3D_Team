#include "VampireCommander.h"

#include "GameInstance.h"
#include "Body_VampireCommander.h"

#include "VampireCommander_Idle.h"
#include "VampireCommander_Spawn1.h"
#include "VampireCommander_HitCenter.h"
#include "VampireCommander_HitLeft.h"
#include "VampireCommander_HitRight.h"
#include "VampireCommander_CutScene.h"

#include "Data_Manager.h"

CVampireCommander::CVampireCommander(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
{
}

CVampireCommander::CVampireCommander(const CVampireCommander& rhs)
	: CCharacter(rhs)
{
}

HRESULT CVampireCommander::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CVampireCommander>(this);
		m_pActor->Set_State(new CVampireCommander_Spawn1);
	}

	//HP
	m_iHp = 1000;

	return S_OK;
}

void CVampireCommander::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CVampireCommander::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}
	Search_Target(L"Layer_Player");
	if (m_bLookAt == true)
	{
		Look_At_Target();
		m_bLookAt = false;
	}
}

void CVampireCommander::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CVampireCommander::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVampireCommander::Ready_Components()
{
	return S_OK;
}

HRESULT CVampireCommander::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_VampireCommander"), BodyDesc)))
		return E_FAIL;

	CWeapon::WEAPON_DESC		WeaponDesc = {};
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_VampireCommander_Weapon_Hand"), "LeftHandIK", WeaponDesc, TEXT("Weapon_hand_L")));
	FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_VampireCommander_Weapon_Hand"), "RightHandIK", WeaponDesc, TEXT("Weapon_hand_R")));

	CWeapon* m_pWeapon_Punch_L = Get_Weapon(TEXT("Weapon_hand_L"));
	m_pWeapon_Punch_L->Set_Enable(false);

	CWeapon* m_pWeapon_Punch_R = Get_Weapon(TEXT("Weapon_hand_R"));
	m_pWeapon_Punch_R->Set_Enable(false);


	return S_OK;
}

void CVampireCommander::Hitted_Left(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
		m_pActor->Set_State(new CVampireCommander_HitRight);
		break;
	}
	
}

void CVampireCommander::Hitted_Right(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
	m_pActor->Set_State(new CVampireCommander_HitLeft);
	break;
	}
}


void CVampireCommander::Hitted_Front(Power ePower)
{
	switch (ePower)
	{
	case Engine::Heavy:
	case Engine::Absolute:
	m_pActor->Set_State(new CVampireCommander_HitCenter);
	break;
	}
}

void CVampireCommander::Hitted_Dead(Power ePower)
{
	//stun이 걸리고 그다음에 처형이 있기 때문에 그냥 때려서는 죽일수 없다.
	m_pActor->Set_State(new CVampireCommander_CutScene);
	CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
	//pPlayer->Get_Actor()->Set_State(new CPlayer_) // 여기서 플레이어를 강제로 처형 애니메이션으로 돌려 버려야 함 ! 
}

CVampireCommander* CVampireCommander::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CVampireCommander* pInstance = new CVampireCommander(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVampireCommander");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander::Clone(void* pArg)
{
	CVampireCommander* pInstance = new CVampireCommander(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVampireCommander");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CVampireCommander::Pool()
{
	return new CVampireCommander(*this);
}

void CVampireCommander::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
