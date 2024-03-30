#pragma once
#include "Tank_State_Death.h"


BEGIN(Client)

class CTank_DeathNormal_F_01 : public CTank_State_Death
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_DeathNormal_F_01;

};

END