#pragma once
#include "VampireCommander_State_Normal.h"

BEGIN(Client)

class CVampireCommander_TurnR180 : public CVampireCommander_State_Normal
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_Turn_R180;

};

END


