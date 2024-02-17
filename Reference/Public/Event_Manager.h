#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CEvent;

class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

	HRESULT Initialize();

public:
	// 이벤트 리스너 등록


	void AddEventListener(CEvent* pEvent) {
		m_Events.push_back(pEvent);
	}

	void AddEventListener(CGameObject* pEventActor, OnEvent OnEvent, Condition Condition);

	// 이벤트 리스너 제거
	void RemoveEventListener(CEvent* pEvent) {
		m_Events.erase(std::remove(m_Events.begin(), m_Events.end(), pEvent), m_Events.end());
	}

	// 이벤트 발생
	void TriggerEvent();

private:
	vector<CEvent*> m_Events;


public:
	static CEvent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END

//public:
//	void RegisterEvent(const string& eventName, OnEvent callback, CGameObject* pTarget);
//	void UnregisterEvent(const string& eventName, OnEvent callback, CGameObject* pTarget);
//	void TriggerEvent(const string& eventName);
//
//private:
//	map<string, vector<OnEvent>> m_events;
//
//private:
//	_uint			m_iCurrentEvnet = { 0 };
//	class CGameInstance* m_pGameInstance = { nullptr };