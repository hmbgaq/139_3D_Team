#pragma once
#include "Assassin_State_Attack.h"


BEGIN(Client)

class CAssassin_Riposte_02 : public CAssassin_State_Attack
{
public:
	virtual void Initialize(CAssassin* pActor) override;
	virtual CState<CAssassin>* Update(CAssassin* pActor, _float fTimeDelta) override;
	virtual void Release(CAssassin* pActor) override;

public:
	static const _uint g_iAnimIndex = CAssassin::Assassin_State::Assassin_Riposte_02;

};

END