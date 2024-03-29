#pragma once
#include "Tank_State_Attack_NoShield.h"


BEGIN(Client)

class CTank_MeleeDynamicNoShield_01 : public CTank_State_Attack_NoShield
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	static const _uint g_iAnimIndex = CTank::Tank_State::Tank_MeleeDynamicNoShield_01;

};

END