#pragma once
#include "Engine_Defines.h"

//BEGIN(Client)

template<class T>
class CState
{

public:
	virtual ~CState() = default;

public:
	virtual void Initialize(T* pActor) = 0;
	virtual CState<T>* Update(T* pActor, _float fTimeDelta) = 0;
	virtual void Release(T* pActor) = 0;

};

//END;