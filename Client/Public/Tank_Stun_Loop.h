#pragma once
#include "Tank_State_Stun.h"


BEGIN(Client)

class CTank_Stun_Loop : public CTank_State_Stun
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_Stun_Loop;

};

END