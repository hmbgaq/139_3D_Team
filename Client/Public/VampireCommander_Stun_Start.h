#pragma once
#include "VampireCommander_Stun.h"

BEGIN(Client)

class CVampireCommander_Stun_Start : public CVampireCommander_Stun
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_Stun_Start;
};

END