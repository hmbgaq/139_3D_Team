//#include "..\Public\DeveloperConsole.h"
//#include <process.h>
//#include "stdafx.h"
//#include "GameInstance.h"
//
//CDeveloperConsole::CDeveloperConsole()
//	: m_pGameInstance(CGameInstance::GetInstance())
//{
//	Safe_AddRef(m_pGameInstance);
//
//}
//
//_uint APIENTRY LoadingThread(void* pArg)
//{
//	CoInitializeEx(nullptr, 0);
//
//	CDeveloperConsole* pDeveloperConsole = (CDeveloperConsole*)pArg;
//
//	pDeveloperConsole->Tick();
//
//	return 0;
//}
//
//HRESULT CDeveloperConsole::Loading()
//{
//	EnterCriticalSection(&m_CriticalSection);
//
//	AllocConsole();	// 콘솔창 생성
//
///* 콘솔 윈도우에대한 핸들 구하기 */
//	HANDLE m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//
//	GetConsoleScreenBufferInfo(m_hOut, &m_csbi);
//	m_oldattrb = m_csbi.wAttributes;
//
//
//
//	LeaveCriticalSection(&m_CriticalSection);
//
//	return S_OK;
//}
//
//HRESULT CDeveloperConsole::Initialize()
//{
//	InitializeCriticalSection(&m_CriticalSection);
//
//	/* 스레드를 생성한다. */
//	/* LoadingThread : 생성한 스레드가 가장 먼저 호출해야할 함수 */
//	/* this : 위 함수를 호출하면서 인자로 전달할 데이터. */
//	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
//	if (0 == m_hThread)
//		return E_FAIL;
//
//}
//
//void CDeveloperConsole::Tick(_float fTimeDelta)
//{
//
//}
//
//void CDeveloperConsole::Set_Attribute(WORD attrib)
//{
//	m_newattrb = attrib;
//
//	SetConsoleTextAttribute(m_hOut, attrib);
//}
//
//void CDeveloperConsole::Print(TCHAR* tszMsg, ...)
//{
//	TCHAR strBuffer[512];
//
//	va_list args;
//	va_start(args, tszMsg);
//	_vsntprintf(strBuffer, 512, tszMsg, args);
//	va_end(args);
//
//	DWORD dwWrite;
//	WriteConsole(m_hOut, strBuffer, _tcslen(strBuffer), &dwWrite, NULL);
//
//}
//
//CDeveloperConsole* CDeveloperConsole::Create()
//{
//	CDeveloperConsole* pInstance = new CDeveloperConsole();
//
//	if (FAILED(pInstance->Initialize()))
//	{
//		MSG_BOX("Failed to Created : CLoader");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//void CDeveloperConsole::Free()
//{
//	SetConsoleTextAttribute(m_hOut, m_oldattrb);
//	FreeConsole();
//
//	WaitForSingleObject(m_hThread, INFINITE);
//	DeleteCriticalSection(&m_CriticalSection);
//	DeleteObject(m_hThread);
//	CloseHandle(m_hThread);
//
//	Safe_Release(m_pGameInstance);
//}
