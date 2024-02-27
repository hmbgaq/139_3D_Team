#pragma once
#include "Assassin_State_Normal.h"


BEGIN(Client)

class CAssassin_IdleAct_01 : public CAssassin_State_Normal
{
public:
	virtual void Initialize(CAssassin* pActor) override;
	virtual CState<CAssassin>* Update(CAssassin* pActor, _float fTimeDelta) override;
	virtual void Release(CAssassin* pActor) override;

public:
	static const _uint g_iAnimIndex = CAssassin::Assassin_State::Assassin_IdleAct_01;

};

END