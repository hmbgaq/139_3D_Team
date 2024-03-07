#pragma once
#include "VampireCommander_State_Walk.h"


BEGIN(Client)

class CVampireCommander_Walk_BR : public CVampireCommander_State_Walk
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_Walk_BR;

};

END