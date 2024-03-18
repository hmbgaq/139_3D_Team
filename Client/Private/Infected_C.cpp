#include "SMath.h"
#include "stdafx.h"
#include "Infected_C.h"
#include "GameInstance.h"
/* Spawn */
#include "Infected_SpawnClimb_01.h"
#include "Infected_SpawnClimb_02.h"
#include "Infected_SpawnCrawl_01.h"
#include "Infected_SpawnCrawl_02.h"
#include "Infected_SpawnFromCoffin0.h"
#include "Infected_SpawnGround.h"
#include "Infected_SpawnFromEgg_02.h"

CInfected_C::CInfected_C(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CInfected(pDevice, pContext, strPrototypeTag)
{
}

CInfected_C::CInfected_C(const CInfected_C& rhs)
	: CInfected(rhs)
{
}

HRESULT CInfected_C::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CInfected_C::Initialize(void* pArg)
{
	/* Ready_Component, Ready_PartObjects는 Initialize를 타고 올라가다보면 CCharacter클래스에서 수행하고있음.*/

	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Ready_Option());

	return S_OK;
}

void CInfected_C::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CInfected_C::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_pActor)
	//{
	//	m_pActor->Update_State(fTimeDelta);
	//}
}

void CInfected_C::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInfected_C::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CInfected_C::Update_RenderState(_int _iHP)
{
	return S_OK;
}

#pragma region Base Setting

HRESULT CInfected_C::Ready_Components()
{
	FAILED_CHECK(__super::Ready_Components());

	return S_OK;
}

HRESULT CInfected_C::Ready_Option()
{
	// 0		~	fAttack : 일반공격 거리 - 공격이후에는 쿨타임이라 뒷걸음질치는거 
	// fAttack	~	fWalk	: 걸어오는 거리 
	// fWalk	~			: 뛰어오는거리 -> 일반공격에 도달할때까지 뛰어옴 -> 뛰면서 공격하는 이동공격으로 감 
	m_eInfo.eType = INFECTED_TYPE::INFECTED_VESSEL_C;
	m_eInfo.RandomNumber = SMath::Random(1, 10);
	m_eInfo.fAttack_Distance = 5.f;
	m_eInfo.fWalk_Distance = 10.f;

	m_iHp = 30;
	m_iMaxHp = 30;
	m_pTarget = m_pGameInstance->Get_Player();

	/* 액터 할당 & 리스폰 애니메이션 지정 */
	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CInfected>(this);

		switch (m_eInfo.RandomNumber >> 1)
		{
		case 1:
			//m_pActor->Set_State(new CInfected_SpawnClimb_01());
			m_pActor->Set_State(new CInfected_SpawnFromEgg_02());
			break;
		case 2:
			//m_pActor->Set_State(new CInfected_SpawnClimb_02());
			m_pActor->Set_State(new CInfected_SpawnFromEgg_02());
			break;
		case 3:
			//m_pActor->Set_State(new CInfected_SpawnCrawl_01());
			m_pActor->Set_State(new CInfected_SpawnFromEgg_02());
			break;
		case 4:
			//m_pActor->Set_State(new CInfected_SpawnCrawl_02());
			m_pActor->Set_State(new CInfected_SpawnFromEgg_02());
			break;
		case 5:
			//m_pActor->Set_State(new CInfected_SpawnFromCoffin0());
			m_pActor->Set_State(new CInfected_SpawnFromEgg_02());
			break;
		default:
			//m_pActor->Set_State(new CInfected_SpawnGround());
			m_pActor->Set_State(new CInfected_SpawnFromEgg_02());
			break;
		}
	}

	return S_OK;
}

HRESULT CInfected_C::Ready_PartObjects()
{
	/* For. Body */
	{
		CBody::BODY_DESC		BodyDesc = {};
		FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Infected_C"), BodyDesc));
	}

	/* For. Weapon */
	{
		CWeapon::WEAPON_DESC		WeaponDesc = {};
		WeaponDesc.m_pSocketBone = m_pBody->Get_BonePtr("RightHandIK");
		WeaponDesc.m_pParentTransform = m_pTransformCom;
		FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Infected_C"), "RightHandIK", WeaponDesc, TEXT("Weapon_Punch")));

		CWeapon* m_pWeapon = Get_Weapon(TEXT("Weapon_Punch"));
		m_pWeapon->Set_Enable(false);
	}
	return S_OK;
	return S_OK;
}

#pragma endregion


#pragma region Create, Clone, Free, Pool

CInfected_C* CInfected_C::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CInfected_C* pInstance = new CInfected_C(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInfected_C");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInfected_C::Clone(void* pArg)
{
	CInfected_C* pInstance = new CInfected_C(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInfected_C");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInfected_C::Pool()
{
	return new CInfected_C(*this);
}

void CInfected_C::Free()
{
	__super::Free();
}

#pragma endregion
