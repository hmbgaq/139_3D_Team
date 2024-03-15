#include "GameObject.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Navigation.h"
#include "Character.h"

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
	m_iCurrnetLevel = m_pGameInstance->Get_NextLevel();

	GAMEOBJECT_DESC		Desc = {};
	
	if(nullptr != pArg)
		Desc = *(GAMEOBJECT_DESC*)pArg;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, Desc.fSpeedPerSec, Desc.fRotationPerSec);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

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

HRESULT CGameObject::Set_InitPosition(const _float3& vPos)
{
	/* 위치 Set */
	Set_Position(vPos);

	/* 해당위치로 네비 셋팅 */
	CNavigation* pNavi = dynamic_cast<CCharacter*>(this)->Get_Navigation();
	NULL_CHECK_RETURN(pNavi, E_FAIL);


	_int iCheckIndex = pNavi->Get_SelectRangeCellIndex(this);
	if (iCheckIndex == -1)
		this->Set_Dead(true);
	pNavi->Set_CurrentIndex(iCheckIndex);

	return S_OK;
}

void CGameObject::Set_Enable(_bool _Enable)
{
	__super::Set_Enable(_Enable);
	for (auto& Pair : m_Components)
		Pair.second->Set_Enable(_Enable);

	if (false == _Enable && true == m_bIsPoolObject)
	{
		Safe_Release(m_pTransformCom);

		for (auto& Pair : m_Components)
			Safe_Release(Pair.second);

		m_Components.clear();
	}

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
	if (nullptr != m_pTransformCom)
		return m_pTransformCom;
	else
		return nullptr;
}

_vector CGameObject::Get_Position_Vector()
{
	return m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
}

_float3 CGameObject::Get_Position()
{
	return m_pTransformCom->Get_Position();
}

_vector CGameObject::Calc_Look_Dir(_vector vTargetPos)
{
	return m_pTransformCom->Calc_Look_Dir(vTargetPos);
}

_vector CGameObject::Calc_Look_Dir_XZ(_vector vTargetPos)
{
	return m_pTransformCom->Calc_Look_Dir_XZ(vTargetPos);
}

//void CGameObject::Set_Speed(_float fSpeed)
//{
//	m_pTransformCom->Set_Speed(fSpeed);
//}

CGameObject* CGameObject::Get_Object_Owner()
{
	return m_pOwner;
}

void CGameObject::Set_Object_Owner(CGameObject* pOwner)
{
	m_pOwner = pOwner;
}

void CGameObject::Delete_Object_Owner()
{
	if (nullptr != m_pOwner)	// 오너가 nullptr이 아니면 삭제 가능
	{
		//Safe_Release(m_pOwner);
		m_pOwner = nullptr;
	}
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

	if (typeid(*pComponent) == typeid(CModel))
	{
		CModel* pModel = dynamic_cast<CModel*>(pComponent);
		
		pModel->Get_Owner()->Set_ModelWidth(pModel->Get_ModelWidth_WithModel());
		pModel->Get_Owner()->Set_ModelHeight(pModel->Get_ModelHeight_WithModel());

	}

	//if (typeid(*pComponent) == typeid(CNavigation))
	//{
	//	CNavigation* pNavigation = dynamic_cast<CNavigation*>(pComponent);
	//
	//	pNavigation->Set_CurrentIndex(pNavigation->Get_SelectRangeCellIndex(this));
	//}

	return S_OK;
}

HRESULT CGameObject::Remove_Component(const wstring& strComTag, _Inout_ CComponent** ppOut) 
{
	auto	iter = m_Components.find(strComTag);

	if (iter == m_Components.end())
		return E_FAIL;

	for (auto& Pair : m_Components)
		Pair.second->Set_Enable(false);

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
	{
		Pair.second->Set_Enable(false);
		Safe_Release(Pair.second);
	}
		

	m_Components.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
