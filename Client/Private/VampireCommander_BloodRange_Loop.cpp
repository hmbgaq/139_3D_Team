#include "VampireCommander_BloodRange_Loop.h"
#include "VampireCommander_BloodRange_Stop.h"
#include "VampireCommander_BloodRange_Stun_Start.h"

void CVampireCommander_BloodRange_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	m_iLoopescape = 0;
}

CState<CVampireCommander>* CVampireCommander_BloodRange_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (m_iLoopescape <= 1000 && m_bSuccessShooting ==false)
	{
		return new CVampireCommander_BloodRange_Stop();
	}
	else if (m_iLoopescape <= 1000 || m_bSuccessShooting == true)
	{
		return new CVampireCommander_BloodRange_Stun_Start();
	}

	++m_iLoopescape;

	return nullptr;
}

void CVampireCommander_BloodRange_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
