#pragma once
#include "VampireCommander_Attack.h"

BEGIN(Client)

class CVampireCommander_BloodRange_Loop : public CVampireCommander_Attack
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_BloodRange_02_Loop;
	//static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::jump_loop;
public:
	_float  m_fHealHP = 0.f;
	_float	m_fPreHP = 0;
	_uint	m_iLoopescape = 0;
	_bool	m_bSuccessShooting = false;
	_uint	m_iRandom[3] = {0};
	class CEffect* m_pEffect = { nullptr };
};

END
