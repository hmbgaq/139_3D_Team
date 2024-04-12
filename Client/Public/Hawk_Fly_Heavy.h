#pragma once
#include "Hawk_State_Normal.h"


BEGIN(Client)

class CHawk_Fly_Heavy : public CHawk_State_Normal
{
public:
	virtual void Initialize(CHawk* pActor) override;
	virtual CState<CHawk>* Update(CHawk* pActor, _float fTimeDelta) override;
	virtual void Release(CHawk* pActor) override;

public:
	static const _uint g_iAnimIndex = CHawk::Hawk_State::Hawk_Fly_Heavy;

};

END