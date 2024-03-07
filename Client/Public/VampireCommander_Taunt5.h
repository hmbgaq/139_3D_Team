#pragma once
#include "VampireCommander_Taunt.h"

BEGIN(Client)

class CVampireCommander_Taunt5 : public CVampireCommander_Taunt
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_Taunt_05;

};

END