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


	return S_OK;
}

void CDevConsole::Tick()
{
	//int a = 0;
	//cin >> a;
	//cout << a << endl;
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
