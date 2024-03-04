#include "..\Public\Bandit_Heavy.h"

#include "GameInstance.h"
#include "Body_Bandit_Heavy.h"

#include "BanditHeavy_Idle.h"

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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBandit_Heavy::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

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

}

void CBandit_Heavy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBandit_Heavy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBandit_Heavy::Ready_Components()
{
	return S_OK;
}

HRESULT CBandit_Heavy::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Bandit_Heavy"), BodyDesc)))
		return E_FAIL;


	return S_OK;
}

CBandit_Heavy* CBandit_Heavy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBandit_Heavy* pInstance = new CBandit_Heavy(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBandit_Heavy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBandit_Heavy::Clone(void* pArg)
{
	CBandit_Heavy* pInstance = new CBandit_Heavy(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBandit_Heavy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBandit_Heavy::Pool()
{
	return new CBandit_Heavy(*this);
}

void CBandit_Heavy::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
