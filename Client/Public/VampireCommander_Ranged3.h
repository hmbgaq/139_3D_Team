#pragma once
#include "VampireCommander_Attack.h"

BEGIN(Client)

class CVampireCommander_Ranged3 : public CVampireCommander_Attack
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	static const _uint g_iAnimIndex = CVampireCommander::VampireCommander_State::VampireCommander_AttackRanged_03_d;

public:
	_bool	m_bfirstCheck = true;


private: //! ½Â¿ëÃß°¡
	void	PlaySound_Wing(CVampireCommander* pActor);

private:
	_float	m_fWingTimeArray[2] = { 0.f, 0.f};
	_bool	m_bPlayWingSoundArray[2] = { false, false};
};

END