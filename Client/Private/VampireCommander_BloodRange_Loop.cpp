#include "VampireCommander_BloodRange_Loop.h"
#include "VampireCommander_BloodRange_Stop.h"
#include "VampireCommander_BloodRange_Stun_Start.h"

void CVampireCommander_BloodRange_Loop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	m_iLoopescape = 0;
	m_iPreHP = pActor->Get_Hp();
}

CState<CVampireCommander>* CVampireCommander_BloodRange_Loop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (m_fHealHP>=200.f && m_bSuccessShooting ==false)
	{
		return new CVampireCommander_BloodRange_Stop();
	}
	else if (m_bSuccessShooting == true)//총으로 약점 공격 성공했을시
	{
		pActor->Set_Hp(m_iPreHP);
		return new CVampireCommander_BloodRange_Stun_Start();
	}
	//체력회복 
	m_fHealHP += 0.3f;
	
	pActor->Set_Hp(m_iPreHP + m_fHealHP);

	++m_iLoopescape;

	return nullptr;
}

void CVampireCommander_BloodRange_Loop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}
