#pragma once
#include "Edgar_State_CutScene.h"


BEGIN(Client)

class CEdgar_Cargo_02 : public CEdgar_State_CutScene
{
public:
	virtual void Initialize(CEdgar* pActor) override;
	virtual CState<CEdgar>* Update(CEdgar* pActor, _float fTimeDelta) override;
	virtual void Release(CEdgar* pActor) override;

public:
	static const _uint g_iAnimIndex = CEdgar::Edgar_State::Edgar_Cargo_02;

};

END