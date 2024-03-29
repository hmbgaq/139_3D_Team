#pragma once
#include "Tank_State_Walk.h"


BEGIN(Client)

class CTank_Walk_F : public CTank_State_Walk
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_Walk_F;

};

END