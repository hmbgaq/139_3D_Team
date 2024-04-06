#pragma once
#include "Mother_State_Normal.h"

BEGIN(Client)

class CMother_Spawn : public CMother_State_Normal
{
public:
	virtual void Initialize(CMother* pActor) override;
	virtual CState<CMother>* Update(CMother* pActor, _float fTimeDelta) override;
	virtual void Release(CMother* pActor) override;

public:
	_float m_fOffSetX = 1.f;
	_float m_fOffSetY = 0.5f;
	_float m_fOffSetZ = -3.f;
	_float3 m_fOffSet = {};
public:
	static const _uint g_iAnimIndex = CMother::Mother_State::GrandParasiter_Intro;

};

END

