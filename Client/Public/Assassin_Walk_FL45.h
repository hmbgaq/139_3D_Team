#pragma once
#include "Assassin_State_Walk.h"


BEGIN(Client)

class CAssassin_Walk_FL45 : public CAssassin_State_Walk
{
public:
	virtual void Initialize(CAssassin* pActor) override;
	virtual CState<CAssassin>* Update(CAssassin* pActor, _float fTimeDelta) override;
	virtual void Release(CAssassin* pActor) override;

public:
	static const _uint g_iAnimIndex = CAssassin::Assassin_State::Assassin_Walk_FL45;

};

END