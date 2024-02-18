#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class IEvent;

class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

	HRESULT Initialize();

public:
	void Tick(_float fTimeDelta);


public:
	// 이벤트 리스너 등록
	void Add_Event(IEvent* pEvent) {
		m_Events.push_back(pEvent);
	}

	// 이벤트 리스너 제거
	//void RemoveEvent(IEvent* pEvent) {
	//	m_Events.erase(std::remove(m_Events.begin(), m_Events.end(), pEvent), m_Events.end());
	//}

	//// 이벤트 발생
	//void TriggerEvent();



private:
	vector<IEvent*> m_Events;


public:
	static CEvent_Manager* Create();
	virtual void Free() override;
};

END
