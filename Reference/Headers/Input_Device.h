#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device();
	virtual ~CInput_Device() = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)		
	{ return m_byKeyState[byKeyID]; }

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 
	{ 	return m_tMouseState.rgbButtons[eMouse]; 	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse	= nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState;	

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
