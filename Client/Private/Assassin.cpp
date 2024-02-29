#include "..\Public\Assassin.h"

#include "GameInstance.h"
#include "Body_Assassin.h"

#include "Assassin_IdleAct_01.h"

CAssassin::CAssassin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
{
}

CAssassin::CAssassin(const CAssassin& rhs)
	: CCharacter(rhs)
{
}

HRESULT CAssassin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAssassin::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (m_pGameInstance->Get_NextLevel() != ECast(LEVEL::LEVEL_TOOL))
	{
		m_pActor = new CActor<CAssassin>(this);
		m_pActor->Set_State(new CAssassin_IdleAct_01());
	}

	return S_OK;
}

void CAssassin::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAssassin::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_pActor)
	//{
	//	m_pActor->Update_State(fTimeDelta);
	//}

}

void CAssassin::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CAssassin::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAssassin::Ready_Components()
{
	return S_OK;
}

HRESULT CAssassin::Ready_PartObjects()
{
	CBody::BODY_DESC		BodyDesc = {};
	if (FAILED(Add_Body(TEXT("Prototype_GameObject_Body_Assassin"), BodyDesc)))
		return E_FAIL;


	return S_OK;
}

CAssassin* CAssassin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CAssassin* pInstance = new CAssassin(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAssassin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAssassin::Clone(void* pArg)
{
	CAssassin* pInstance = new CAssassin(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAssassin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAssassin::Pool()
{
	return new CAssassin(*this);
}

void CAssassin::Free()
{
	__super::Free();

	if (m_pActor)
	{
		Safe_Delete(m_pActor);
	}

}
