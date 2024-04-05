#pragma once
#include "Edgar_State_Cover_Low_Right.h"


BEGIN(Client)

class CEdgar_Sniper_CoverLow_Side_R_Stop : public CEdgar_State_Cover_Low_Right
{
public:
	virtual void Initialize(CEdgar* pActor) override;
	virtual CState<CEdgar>* Update(CEdgar* pActor, _float fTimeDelta) override;
	virtual void Release(CEdgar* pActor) override;

public:
	static const _uint g_iAnimIndex = CEdgar::Edgar_State::Edgar_Sniper_CoverLow_Side_R_Stop;

};

END