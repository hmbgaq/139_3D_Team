#pragma once
#include "VampireCommander_Spawn.h"

BEGIN(Client)

class CVampireCommander_Spawn1 : public CVampireCommander_Spawn
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_Spawn_01;

};

END