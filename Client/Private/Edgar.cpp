#include "..\Public\Edgar.h"
#include "GameInstance.h"
#include "Data_Manager.h"
//#include "Body_Edgar.h"
//#include "Weapon_Edgar.h"
//#include "Edgar_IdleLoop.h"


CEdgar::CEdgar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter_Client(pDevice, pContext, strPrototypeTag)
{
}

CEdgar::CEdgar(const CEdgar& rhs)
	: CCharacter_Client(rhs)
{
}

HRESULT CEdgar::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CEdgar::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 7.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));


	m_pActor = new CActor<CEdgar>(this);
	//m_pActor->Set_State(new CEdgar_IdleLoop());

	m_pRigidBody->Set_UseGravity(true);

	m_fHp = 1000000000;


	return S_OK;
}

void CEdgar::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CEdgar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
}

void CEdgar::Late_Tick(_float fTimeDelta)
{
	if (GAME_STATE::GAMEPLAY != CData_Manager::GetInstance()->Get_GameState())
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CEdgar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdgar::Ready_Components()
{
	return S_OK;
}

HRESULT CEdgar::Ready_PartObjects()
{

	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Edgar"), BodyDesc));

	//CWeapon::WEAPON_DESC		WeaponDesc = {};
	//FAILED_CHECK(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Edgar"), "LeftHandIK", WeaponDesc, Edgar_WEAPON_PUNCH_L));

	return S_OK;
}

void CEdgar::Search_Target(_float fMaxDistance)
{
	__super::Search_Target(LAYER_MONSTER, fMaxDistance);
	__super::Search_Target(LAYER_BOSS, fMaxDistance);
}

void CEdgar::Check_Frustum()
{
	m_bIsInFrustum = true;
}

void CEdgar::Hitted_Left(Power ePower)
{
}

void CEdgar::Hitted_Right(Power ePower)
{
}

void CEdgar::Hitted_Front(Power ePower)
{
}

CEdgar* CEdgar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEdgar* pInstance = new CEdgar(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEdgar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEdgar::Clone(void* pArg)
{
	CEdgar* pInstance = new CEdgar(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEdgar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEdgar::Pool()
{
	return new CEdgar(*this);
}

void CEdgar::Free()
{
	__super::Free();
}
