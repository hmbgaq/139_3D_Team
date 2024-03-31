#pragma once
#include "Tank_State_Attack_Shield.h"


BEGIN(Client)

class CTank_Charge_Fail_R : public CTank_State_Attack_Shield
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_Charge_Fail_R;

};

END