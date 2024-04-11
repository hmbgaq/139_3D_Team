#include "..\Public\Hawk.h"
#include "GameInstance.h"
#include "Data_Manager.h"


CHawk::CHawk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter_Client(pDevice, pContext, strPrototypeTag)
{
}

CHawk::CHawk(const CHawk& rhs)
	: CCharacter_Client(rhs)
{
}

HRESULT CHawk::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CHawk::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 7.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK(__super::Initialize(&GameObjectDesc));


	m_pActor = new CActor<CHawk>(this);



	return S_OK;
}

void CHawk::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHawk::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CHawk::Late_Tick(_float fTimeDelta)
{
	if (GAME_STATE::GAMEPLAY != CData_Manager::GetInstance()->Get_GameState())
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CHawk::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHawk::Ready_Components()
{
	return S_OK;
}

HRESULT CHawk::Ready_PartObjects()
{

	CBody::BODY_DESC		BodyDesc = {};
	FAILED_CHECK(Add_Body(TEXT("Prototype_GameObject_Body_Hawk"), BodyDesc));


	return S_OK;
}


CHawk* CHawk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CHawk* pInstance = new CHawk(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHawk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHawk::Clone(void* pArg)
{
	CHawk* pInstance = new CHawk(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHawk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHawk::Pool()
{
	return new CHawk(*this);
}

void CHawk::Free()
{
	__super::Free();
}
