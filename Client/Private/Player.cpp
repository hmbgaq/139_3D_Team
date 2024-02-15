#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Weapon_Player.h"
#include "Body_Player.h"



CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CCharacter(rhs)
{
}

CComponent * CPlayer::Find_Component(const wstring & strComTag, const wstring & strPartTag)
{
	auto	iter = m_PartObjects.find(strPartTag);
	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second->Find_Component(strComTag);
}

HRESULT CPlayer::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{	
	CGameObject::GAMEOBJECT_DESC pGameObjectDesc;// = (CGameObject::GAMEOBJECT_DESC*)pArg;

	pGameObjectDesc.fSpeedPerSec = 10.f;
	pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);
			
	if (FAILED(__super::Initialize(&pGameObjectDesc)))
		return E_FAIL;	

	if (FAILED(Ready_Components(LEVEL(m_pGameInstance->Get_NextLevel()))))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}


}

void CPlayer::Tick(_float fTimeDelta)
{
	CBody_Player*		pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));
	Safe_AddRef(pBody);

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

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	Safe_Release(pBody);

	//__super::SetUp_OnTerrain(m_pTransformCom);

	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(m_pNavigationCom);
	//m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
}

HRESULT CPlayer::Render()
{

#ifdef _DEBUG
	/*m_pNavigationCom->Render();
	m_pColliderCom->Render();*/
#endif	

	return S_OK;
}

CGameObject * CPlayer::Find_PartObject(const wstring & strPartTag)
{
	auto	iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;	
}

HRESULT CPlayer::Ready_Components(LEVEL eLevel)
{
	/* For.Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(eLevel, TEXT("Prototype_Component_Navigation2"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* For.Part_Body */
	CBody_Player::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Player"), BodyDesc)))
		return E_FAIL;

	

	{
		CWeapon_Player::WEAPON_DESC	WeaponDesc = {};
		if (FAILED(Add_Weapon(TEXT("Prototype_GameObject_Weapon_Player"), "SWORD", WeaponDesc, TEXT("Weapon_L"))))
			return E_FAIL;
	}
	CWeapon* m_pWeapon_L = Get_Weapon(TEXT("Weapon_L"));


	return S_OK;
}

HRESULT CPlayer::Add_PartObject(const wstring & strPrototypeTag, const wstring & strPartTag, void * pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject*		pPartObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	return S_OK;
}


CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

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

void CPlayer::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);
	m_PartObjects.clear();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}

