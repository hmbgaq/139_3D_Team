#include "stdafx.h"
#include "..\Public\DevConsole.h"
#include <tchar.h>

CDevConsole::CDevConsole()
{
}

HRESULT CDevConsole::Initialize()
{
	//AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::cout << "Dev Console" << std::endl;

    //freopen("CONOUT$", "w", stdout);
    //std::cout << "Dev Console" << std::endl;
    //bQuitRequested = false;

    return S_OK;
}

void CDevConsole::Tick()
{
    //EnterCriticalSection(&m_CriticalSection);

/*    if (bQuitRequested) return;

    if (input_thread_.valid() && input_thread_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        input_thread_.get();
    }

    input_thread_ = std::async(std::launch::async, [this]() {
        std::cin >> iDebugNumber;

        if (iDebugNumber != 0) {
            switch (iDebugNumber) {
            case 1:
                std::cout << "test 1  - Logic A" << std::endl;
                break;
            case 2:
                std::cout << "Test 2 - Logic B" << std::endl;
                break;
            default:
                break;
            }
            iDebugNumber = 0;
        }
        })*/;
}

CDevConsole* CDevConsole::Create()
{
	CDevConsole* pInstance = new CDevConsole();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CDevConsole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDevConsole::Free()
{
	//FreeConsole();
}
