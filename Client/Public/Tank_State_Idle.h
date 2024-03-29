#pragma once
#include "Tank_State.h"

BEGIN(Client)

class CTank_State_Idle : public CTank_State
{
public:
	virtual void Initialize(CTank* pActor) override;
	virtual CState<CTank>* Update(CTank* pActor, _float fTimeDelta) override;
	virtual void Release(CTank* pActor) override;

public:
	virtual CState<CTank>* Update_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END