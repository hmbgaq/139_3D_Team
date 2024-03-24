#pragma once
#include "MotherMouth_State_Normal.h"

BEGIN(Client)

class CMotherMouth_Idle : public CMotherMouth_State_Normal
{
public:
	virtual void Initialize(CMotherMouth* pActor) override;
	virtual CState<CMotherMouth>* Update(CMotherMouth* pActor, _float fTimeDelta) override;
	virtual void Release(CMotherMouth* pActor) override;

public:
	static const _uint g_iAnimIndex = CMotherMouth::MotherMouth_State::Grand_parasiter_mouth_Idle;

};

END

