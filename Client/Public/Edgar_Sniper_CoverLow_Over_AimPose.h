#pragma once
#include "Edgar_State_Cover_Low_Over.h"


BEGIN(Client)

class CEdgar_Sniper_CoverLow_Over_AimPose : public CEdgar_State_Cover_Low_Over
{
public:
	virtual void Initialize(CEdgar* pActor) override;
	virtual CState<CEdgar>* Update(CEdgar* pActor, _float fTimeDelta) override;
	virtual void Release(CEdgar* pActor) override;

public:
	static const _uint g_iAnimIndex = CEdgar::Edgar_State::Edgar_Sniper_CoverLow_Over_AimPose;

};

END