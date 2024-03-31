#pragma once
#include "Tank_State_Hit.h"


BEGIN(Client)

class CTank_VortexGrenade_Stop : public CTank_State_Hit
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_VortexGrenade_Stop;

};

END