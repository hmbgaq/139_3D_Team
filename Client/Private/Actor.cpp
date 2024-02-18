#include "stdafx.h"
#include "Actor.h"
#include "Engine_Function.h"
#include "State.h"


template<class T>
CActor<T>::CActor(T* _pOwner)
    : m_pOwner(_pOwner)
{
    Safe_AddRef(_pOwner);
}

template<class T>
CActor<T>::~CActor()
{
    Free();
}


template<class T>
void CActor<T>::Update_State(_float fTimeDelta)
{
    if (nullptr == m_pOwner || nullptr == m_pState)
        return;

    CState<T>* pNowState = m_pState->Update(m_pOwner, fTimeDelta);

    Set_State(pNowState);
}

template<class T>
void CActor<T>::Set_State(CState<T>* _pState)
{
    if (nullptr == _pState)
        return;

    if (nullptr != m_pState)
    {
        m_pState->Release(m_pOwner);
        delete m_pState;
    }

    m_pState = _pState;
    m_pState->Initialize(m_pOwner);
}

template<class T>
void CActor<T>::Free()
{
    delete m_pState;
    m_pState = nullptr;

    Safe_Release(m_pOwner);
}



