#include "VampireCommander.h"

#include "GameInstance.h"
#include "Body_VampireCommander.h"

#include "VampireCommander_Idle.h"

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
		m_pActor->Set_State(new CVampireCommander_Idle());
	}

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


	return S_OK;
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
