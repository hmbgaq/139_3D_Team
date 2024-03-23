#pragma once
#include "MotherMouth_State_Normal.h"

BEGIN(Client)

class CMotherMouth_SpittingMineLoop : public CMotherMouth_State_Normal
{
public:
	virtual void Initialize(CMotherMouth* pActor) override;
	virtual CState<CMotherMouth>* Update(CMotherMouth* pActor, _float fTimeDelta) override;
	virtual void Release(CMotherMouth* pActor) override;

public:
	static const _uint g_iAnimIndex = CMotherMouth::MotherMouth_State::Parasiter_SpittingMines_Loop_1800;

};

END