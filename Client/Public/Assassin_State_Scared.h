#pragma once
#include "Assassin_State.h"

BEGIN(Client)

class CAssassin_State_Scared : public CAssassin_State
{
public:
	virtual void Initialize(CAssassin* pActor) override;
	virtual CState<CAssassin>* Update(CAssassin* pActor, _float fTimeDelta) override;
	virtual void Release(CAssassin* pActor) override;

public:
	virtual CState<CAssassin>* Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex) override;


};

END