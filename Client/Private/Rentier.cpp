#include "..\Public\Rentier.h"

#include "GameInstance.h"
#include "Body_Rentier.h"

CRentier::CRentier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
{
}

CRentier::CRentier(const CRentier& rhs)
	: CCharacter(rhs)
{
}

HRESULT CRentier::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRentier::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CRentier::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CRentier::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRentier::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRentier::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRentier::Ready_Components()
{
	return S_OK;
}

HRESULT CRentier::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Rentier"), BodyDesc)))
		return E_FAIL;


	return S_OK;
}

CRentier* CRentier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CRentier* pInstance = new CRentier(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRentier");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRentier::Clone(void* pArg)
{
	CRentier* pInstance = new CRentier(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRentier");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRentier::Pool()
{
	return new CRentier(*this);
}

void CRentier::Free()
{
	__super::Free();
}
