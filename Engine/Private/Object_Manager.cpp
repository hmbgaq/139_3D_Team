#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "GameInstance.h"

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex, strPartTag);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype || 
		nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	list<CGameObject*> ObjectList;
	
	if (true == pPrototype->Is_PoolObject()) 
	{
		for (_uint i = 0; i < 200; ++i)
		{
			ObjectList.push_back(pPrototype->Pool());
		}
	}
	
	m_Pool.emplace(strPrototypeTag, ObjectList);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	/* 원형을 찾고. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);

	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = { nullptr };

	auto	iter = m_Pool.find(strPrototypeTag);
	if (iter == m_Pool.end())
	{
		return E_FAIL;
	}
	else 
	{
		if (0 < (*iter).second.size())
		{
			pGameObject = (*iter).second.back();
			pGameObject->Initialize(pArg);
			pGameObject->Set_Enable(true);
			(*iter).second.pop_back();
		}
		else 
		{
			/* 원형을 복제하여 실제 게임내에 사용할 사본 객체를 생성해낸다.  */
			pGameObject = pPrototype->Clone(pArg);



			pGameObject->Set_Enable(true);
		}
	}

	if (nullptr == pGameObject)
		return E_FAIL;


	/* 만들어낸 사본객체를 추가해야할 레이어를 찾자. */
	CLayer*			pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 해당 이름을 가진 레이어가 없었다. */
	/* 이 이름을 가진 레이어에 최초로 추가하고 있는 상황이다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create(this);
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* 이미 이름을 가진 레이어가 있었어. */
	else
		pLayer->Add_GameObject(pGameObject);
	
	return S_OK;
}

CGameObject * CObject_Manager::Clone_Prototype(const wstring & strPrototypeTag, void * pArg)
{
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);
}

HRESULT CObject_Manager::Fill_PoolObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject || true == pGameObject->Get_Enable())
		return E_FAIL;

	auto	iter = m_Pool.find(pGameObject->Get_ProtoTypeTag());
	if (iter == m_Pool.end())
		return E_FAIL;

	pGameObject->Set_Enable(false);

	(*iter).second.push_back(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Create_PoolObjects(const wstring& strPrototypeTag, _uint iSize)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	if (false == pPrototype->Is_PoolObject())
		return S_OK;

	auto	iter = m_Pool.find(strPrototypeTag);
	if (iter == m_Pool.end())
		return E_FAIL;


	for (_uint i = 0; i < iSize; ++i)
	{
		CGameObject* pCloneObject = pPrototype->Pool();
		pCloneObject->Set_Enable(false);
		(*iter).second.push_back(pCloneObject);
	}
	
	return S_OK;
}

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (true == Pair.second->Get_Stop())
				continue;

			Pair.second->Priority_Tick(fTimeDelta);		

		}
	}
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i]) 
		{
			if (true == Pair.second->Get_Stop())
				continue;

			Pair.second->Tick(fTimeDelta);
		}
	}

}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (true == Pair.second->Get_Stop())
				continue;

			Pair.second->Late_Tick(fTimeDelta);
		}
			
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

list<class CGameObject*>* CObject_Manager::Get_GameObjects(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* layer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == layer)
		return nullptr;

	return layer->Get_GameObjects();
}

void CObject_Manager::Get_CloneGameObjects(_uint iLevelIndex, vector<CGameObject*>* clonevector)
{
	for (auto& item : m_pLayers[iLevelIndex])
	{

		list<CGameObject*> pGameObjects = *item.second->Get_GameObjects();
		for (auto& pGameObject : pGameObjects)
		{
			clonevector->push_back(pGameObject);
		}
	}
	//_bool test = false;
}

void CObject_Manager::Fill_PrototypeTags(vector<string>* _vector)
{
	for (auto& item : m_Prototypes)
	{
		string key;
		m_pGameInstance->WString_To_String(item.first, key);

		wstring Test = {};
		m_pGameInstance->String_To_WString(key, Test);

		_vector->push_back(key);
	}
}

CCharacter* CObject_Manager::Get_Player()
{
	return m_pPlayer;
}

void CObject_Manager::Set_Player(CCharacter* _pPlayer)
{
	m_pPlayer = _pPlayer;
}

void CObject_Manager::Set_Layer_Speed(_uint iLevelIndex, const wstring& strLayerTag, _float fSpeed)
{
	CLayer* layer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == layer)
		return;

	layer->Set_Speed(fSpeed);
}

void CObject_Manager::Reset_Layers_Speed(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Pair.second->Set_Speed(1.f);
}


void CObject_Manager::Set_Layer_Stop(_uint iLevelIndex, const wstring& strLayerTag, _bool bStop)
{
	CLayer* layer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == layer)
		return;

	layer->Set_Stop(bStop);
}

void CObject_Manager::Reset_Layer_Stop(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Pair.second->Set_Stop(false);
}

CGameObject * CObject_Manager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring & strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;	
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager;

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);

		m_pLayers[i].clear();		
	}

	Safe_Delete_Array(m_pLayers);


	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();

	for (auto& PoolList : m_Pool) 
	{
		for (auto& pPoolObject : PoolList.second)
			Safe_Release(pPoolObject);
	}

	m_Pool.clear();


	Safe_Release(m_pGameInstance);
}
