#include "..\Public\Bandit_Sniper.h"

#include "GameInstance.h"
#include "Body_Bandit_Sniper.h"

#include "Sniper_IdlePose.h"

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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBandit_Sniper::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CBandit_Sniper>(this);
		m_pActor->Set_State(new CSniper_IdlePose());
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
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBandit_Sniper::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Sniper::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Sniper"), BodyDesc)))
		return E_FAIL;


	return S_OK;
}

CBandit_Sniper* CBandit_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBandit_Sniper* pInstance = new CBandit_Sniper(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
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

	/* 원형객체를 초기화한다.  */
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
