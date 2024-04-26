#pragma once
#include "Mother_State_Normal.h"

BEGIN(Client)

class CMother_VomitLoop : public CMother_State_Normal
{
public:
	virtual void Initialize(CMother* pActor) override;
	virtual CState<CMother>* Update(CMother* pActor, _float fTimeDelta) override;
	virtual void Release(CMother* pActor) override;

private:
	_float m_fTimeDelta = 0.f;
	_float m_fCloneTime = 0.f;

	_bool m_bFirst = { false };

public:
	static const _uint g_iAnimIndex = CMother::Mother_State::Parasiter_Vomit_Loop_02;

};

END