#include "Layer.h"
#include "GameObject.h"

#include "Object_Manager.h"

CLayer::CLayer()
{

}

CComponent * CLayer::Get_Component(const wstring & strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag, strPartTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && true == pGameObject->Get_Enable())
			pGameObject->Priority_Tick(fTimeDelta);
	}	
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			if(true == pGameObject->Get_Enable())
				pGameObject->Tick(fTimeDelta);
		}
		
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	//for (auto& pGameObject : m_GameObjects)
	//{
	//	if (nullptr != pGameObject)
	//		pGameObject->Late_Tick(fTimeDelta);
	//}

	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != (*iter) && true == (*iter)->Get_Enable())
			(*iter)->Late_Tick(fTimeDelta);
		

		if ((*iter)->Is_Dead())
		{
			Safe_Release((*iter));
			iter = m_GameObjects.erase(iter);
		}
		else if (false == (*iter)->Get_Enable() && true == (*iter)->Is_PoolObject())
		{
			m_pObjectManager->Fill_PoolObject((*iter));
			iter = m_GameObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::Set_ObjectManager(CObject_Manager* pObjectManager)
{
	m_pObjectManager = pObjectManager;
}

CLayer * CLayer::Create(CObject_Manager* pObjectManager)
{
	CLayer* pInstance = new CLayer();
	pInstance->Set_ObjectManager(pObjectManager);
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (false == pGameObject->Get_Enable())
		{
			m_pObjectManager->Fill_PoolObject(pGameObject);
		}
		else 
		{
			Safe_Release(pGameObject);
		}
		
	}
		
	m_GameObjects.clear();
}
