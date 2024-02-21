#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Transform.h"


CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
	, m_isCloned(false)
	, m_strPrototypeTag(strPrototypeTag)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_pGameInstance(rhs.m_pGameInstance)
	, m_isCloned(true)
	, m_strPrototypeTag(rhs.m_strPrototypeTag)
{

	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext); 
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	m_bEnable = true;

	GAMEOBJECT_DESC		Desc = {};
	
	if(nullptr != pArg)
		Desc = *(GAMEOBJECT_DESC*)pArg;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, Desc.fSpeedPerSec, Desc.fRotationPerSec);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (nullptr != Find_Component(g_pTransformTag))
		return E_FAIL;

	m_Components.emplace(g_pTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Tick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

_bool CGameObject::Picking(_Out_ _float3* vPickedPos)
{
	return false;
}



CComponent * CGameObject::Find_Component(const wstring & strComTag, const wstring & strPartTag)
{
	auto	iter = m_Components.find(strComTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Set_Position(const _float3& vState)
{
	m_pTransformCom->Set_Position(vState);
}

void CGameObject::Set_WorldMatrix(_float4x4 matrix)
{
	m_pTransformCom->Set_WorldMatrix(matrix);
}

_bool CGameObject::Write_Json(json& Out_Json)
{
	for (auto& elem_List : m_Components)
	{
		elem_List.second->Write_Json(Out_Json["Component"]);
	}

	return false;
}

void CGameObject::Load_FromJson(const json& In_Json)
{
	for (auto& elem_List : m_Components)
	{
		elem_List.second->Load_FromJson(In_Json["Component"]);
	}

	_float4x4 WorldMatrix;

	ZeroMemory(&WorldMatrix, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["Component"]["Transform"], WorldMatrix);

	Set_WorldMatrix(WorldMatrix);
}

CTransform* CGameObject::Get_Transform()
{
	return m_pTransformCom;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComTag, _Inout_ CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(strComTag))
		return E_FAIL;

	CComponent*		pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	m_Components.emplace(strComTag, pComponent);

	Safe_AddRef(pComponent);

	pComponent->Set_Owner(this);

	return S_OK;
}

HRESULT CGameObject::Remove_Component(const wstring& strComTag, _Inout_ CComponent** ppOut) 
{
	auto	iter = m_Components.find(strComTag);

	if (iter == m_Components.end())
		return E_FAIL;

	CComponent* pComponent = iter->second;

	Safe_Release(pComponent);

	if (nullptr != ppOut && nullptr != *ppOut)
	{
		Safe_Release(pComponent);
		*ppOut = nullptr;
	}

	m_Components.erase(iter);

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
