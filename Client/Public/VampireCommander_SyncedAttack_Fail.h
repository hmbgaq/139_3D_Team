#pragma once
#include "VampireCommander_Attack.h"

BEGIN(Client)

class CVampireCommander_SyncedAttack_Fail : public CVampireCommander_Attack
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_SyncedAttack_Failure;

};

END
