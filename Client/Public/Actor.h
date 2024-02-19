#pragma once
#include "Engine_Defines.h"
#include "State.h"

//BEGIN(Client)

template<class T>
class CActor
{
public:
	CActor(T* _pOwner);
	virtual ~CActor();

public:
	void	Update_State(_float fTimeDelta);
	void	Set_State(CState<T>* _pState);
	void	Free();

private:
	CState<T>* m_pState = nullptr;
	T* m_pOwner = { nullptr; };
};

//END;