#pragma once
#include "Son_State.h"

BEGIN(Client)

class CSon_State_Normal : public CSon_State
{
public:
	virtual void Initialize(CSon* pActor) override;
	virtual CState<CSon>* Update(CSon* pActor, _float fTimeDelta) override;
	virtual void Release(CSon* pActor) override;

public:
	virtual CState<CSon>* Update_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END