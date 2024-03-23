#include "MotherMouth.h"
#include "GameInstance.h"
#include "Mother.h"

CMotherMouth::CMotherMouth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CMonster_Character(pDevice, pContext, strPrototypeTag)
{
}

CMotherMouth::CMotherMouth(const CMotherMouth& rhs)
	: CMonster_Character(rhs)
{
}

HRESULT CMotherMouth::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMotherMouth::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	//{
	//	m_pActor = new CActor<CVampireCommander>(this);
	//	m_pActor->Set_State(new CVampireCommander_Spawn1);
	//}
	//HP

	return S_OK;
}

void CMotherMouth::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CMotherMouth::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMotherMouth::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMotherMouth::Render()
{
	return E_NOTIMPL;
}

HRESULT CMotherMouth::Ready_Components()
{
	return S_OK;
}

HRESULT CMotherMouth::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_MotherMouth"), BodyDesc)))
		return E_FAIL;

	return S_OK;
}

CMotherMouth* CMotherMouth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CMotherMouth* pInstance = new CMotherMouth(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMotherMouth");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMotherMouth::Clone(void* pArg)
{
	CMotherMouth* pInstance = new CMotherMouth(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMotherMouth");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMotherMouth::Pool()
{
	return new CMotherMouth(*this);
}

void CMotherMouth::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}
}
