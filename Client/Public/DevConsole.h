#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include <future>

BEGIN(Client)

class CDevConsole : public CBase
{
private:
	CDevConsole();
	virtual ~CDevConsole() = default;
	
public:
	virtual HRESULT Initialize();
	virtual void Tick();

private:
	HANDLE	m_hOut;

private:
	/* Test Line */    
	std::future<void> input_thread_;
	int iDebugNumber;
	bool bQuitRequested;


	//HANDLE					m_hThread;
	//CRITICAL_SECTION		m_CriticalSection;
	//void Test_Function();
	//std::future<void> input_thread_;
	//int iDebugNumber;
	//bool bQuitRequested;


public:
	static CDevConsole* Create();
	virtual void Free() override;

};

END