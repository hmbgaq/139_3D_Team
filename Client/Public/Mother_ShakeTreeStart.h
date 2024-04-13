#pragma once
#include "Mother_State_Normal.h"

BEGIN(Client)

class CMother_ShakeTreeStart : public CMother_State_Normal
{
public:
	virtual void Initialize(CMother* pActor) override;
	virtual CState<CMother>* Update(CMother* pActor, _float fTimeDelta) override;
	virtual void Release(CMother* pActor) override;

public:
	static const _uint g_iAnimIndex = CMother::Mother_State::Parasiter_ShakeTree_Start;



};

END