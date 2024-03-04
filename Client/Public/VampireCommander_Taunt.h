#pragma once
#include "VampireCommander_State.h"

BEGIN(Client)

class CVampireCommander_Taunt : public CVampireCommander_State
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	virtual CState<CVampireCommander>* Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex) override;

};

END