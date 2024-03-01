#include "..\Public\Infected.h"

#include "GameInstance.h"
#include "Body_Infected.h"

#include "Infected_Idle.h"
#include "Infected_SpawnGround.h"


CInfected::CInfected(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
{
}

CInfected::CInfected(const CInfected& rhs)
	: CCharacter(rhs)
{
}

HRESULT CInfected::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfected::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CInfected>(this);
		m_pActor->Set_State(new CInfected_SpawnGround());
	}

	return S_OK;
}

void CInfected::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CInfected::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor)
	{
		m_pActor->Update_State(fTimeDelta);
	}

}

void CInfected::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInfected::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfected::Ready_Components()
{
	return S_OK;
}

HRESULT CInfected::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Infected"), BodyDesc)))
		return E_FAIL;


	return S_OK;
}

CInfected* CInfected::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CInfected* pInstance = new CInfected(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInfected");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInfected::Clone(void* pArg)
{
	CInfected* pInstance = new CInfected(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInfected");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInfected::Pool()
{
	return new CInfected(*this);
}

void CInfected::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
