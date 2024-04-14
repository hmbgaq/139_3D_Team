#pragma once
#include "VampireCommander_Attack.h"

BEGIN(Client)

class CVampireCommander_BloodRange_Loop : public CVampireCommander_Attack
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

private:
	void	PlaySound_Healing(_float fTimeDelta); //! ½Â¿ë Ãß°¡

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_BloodRange_02_Loop;
	//static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::jump_loop;
public:
	_float  m_fHealHP = 0.f;
	_float	m_fPreHP = 0.f;
	_float m_bBloodLoopTime = 0.f;
	_uint	m_iLoopescape = 0;
	_bool	m_bSuccessShooting = false;
	_uint	m_iShootingCount = 0;
	_uint	m_iRandom[3] = {0};
	class CEffect* m_pEffect = { nullptr };

private: //!½Â¿ë Ãß°¡
	_float	m_fSoundTimeAcc = 0.f;
	_float  m_fHealingPlayTime = 1.5f;
};

END
