#pragma once
#include "Assassin_State_Scared.h"


BEGIN(Client)

class CAssassin_Scared_02 : public CAssassin_State_Scared
{
public:
	virtual void Initialize(CAssassin* pActor) override;
	virtual CState<CAssassin>* Update(CAssassin* pActor, _float fTimeDelta) override;
	virtual void Release(CAssassin* pActor) override;

public:
	static const _uint g_iAnimIndex = CAssassin::Assassin_State::Assassin_Scared_02;

};

END