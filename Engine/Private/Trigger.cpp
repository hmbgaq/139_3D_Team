//#include "..\Public\Trigger.h"
//
//#include "EventActor.h"
//
//CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//	: CGameObject(pDevice, pContext)
//{
//}
//
//CTrigger::CTrigger(const CTrigger& rhs)
//	: CGameObject(rhs)
//{
//}
//
//HRESULT CTrigger::Initialize(vector<CEventActor*> _Actors)
//{
//	for (CEventActor* pEventActor : _Actors) 
//	{
//		m_Actors.push_back(pEventActor);
//	}
//
//	return S_OK;
//}
//
//void CTrigger::Activate()
//{
//	for (auto iter = m_Actors.begin(); iter != m_Actors.end();)
//	{
//		if (nullptr != (*iter))
//		{
//			(*iter)->Activate();
//		}
//		iter = m_Actors.erase(iter);
//	}
//}
//
//void CTrigger::Free()
//{
//	__super::Free();
//}
