#pragma once
#include "Tank_State_Run.h"


BEGIN(Client)

class CTank_Run_FL : public CTank_State_Run
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_Run_FL;

};

END