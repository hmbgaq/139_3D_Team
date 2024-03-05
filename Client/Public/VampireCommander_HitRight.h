#pragma once
#include "VampireCommander_Hit.h"

BEGIN(Client)

class CVampireCommander_HitRight : public CVampireCommander_Hit
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_HitNormal_F_02;
};

END


