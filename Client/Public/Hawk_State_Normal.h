#pragma once
#include "Hawk_State.h"

BEGIN(Client)

class CHawk_State_Normal : public CHawk_State
{
public:
	virtual void Initialize(CHawk* pActor) override;
	virtual CState<CHawk>* Update(CHawk* pActor, _float fTimeDelta) override;
	virtual void Release(CHawk* pActor) override;

public:
	virtual CState<CHawk>* Update_State(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END