#pragma once
#include "Mother_State.h"

BEGIN(Client)

class CMother_State_Normal : public CMother_State
{
public:
	virtual void Initialize(CMother* pActor) override;
	virtual CState<CMother>* Update(CMother* pActor, _float fTimeDelta) override;
	virtual void Release(CMother* pActor) override;

public:
	virtual CState<CMother>* Update_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END