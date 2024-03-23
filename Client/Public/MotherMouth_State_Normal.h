#pragma once
#include "MotherMouth_State.h"

BEGIN(Client)

class CMotherMouth_State_Normal : public CMotherMouth_State
{
public:
	virtual void Initialize(CMotherMouth* pActor) override;
	virtual CState<CMotherMouth>* Update(CMotherMouth* pActor, _float fTimeDelta) override;
	virtual void Release(CMotherMouth* pActor) override;

public:
	virtual CState<CMotherMouth>* Update_State(CMotherMouth* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END