#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
public:
    typedef struct tagMouseState
    {
        _long   lX;
        _long   lY;
        _long   lZ;
        BYTE    rgbButtons[4];
    } DIMOUSESTATE, * LPDIMOUSESTATE;

    typedef struct tagKeyState
    {
        _bool m_byKeyData[256];
        _byte m_byKeyState[256];
    }KEYSTATE;

private:
    CInput_Device();
    virtual ~CInput_Device() = default;

public:
    HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
    void    Tick();
    void    LateTick();

public:
    _bool   Key_Pressing(_ubyte byKeyID);
    _bool   Key_Down(_ubyte byKeyID);
    _bool   Key_Up(_ubyte byKeyID);

    _bool   Mouse_Pressing(MOUSEKEYSTATE eMouse);
    _bool   Mouse_Down(MOUSEKEYSTATE eMouse);
    _bool   Mouse_Up(MOUSEKEYSTATE eMouse);

    _byte   Get_DIKeyState(_ubyte byKeyID)
    {
        return m_byKeyState[byKeyID];
    }

    _byte   Get_DIMouseState(MOUSEKEYSTATE eMouse)
    {
        return m_tMouseState.rgbButtons[eMouse];
    }

    _long   Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
    {
        return *(((_long*)&m_tMouseState) + eMouseState);
    }

public:
    HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);


private:
    IDirectInput8* m_pInput = nullptr;

private:
    IDirectInputDevice8* m_pKeyBoard = nullptr;
    IDirectInputDevice8* m_pMouse = nullptr;

private:
    //KEYSTATE             m_tKeyState;
    _bool                m_byKeyData[256];
    _byte                m_byKeyState[256];
    DIMOUSESTATE         m_tMouseState;
    _bool                m_bMouseData[DIM_END];



public:
    static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
    virtual void Free() override;

};
END
