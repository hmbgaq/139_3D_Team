#include "..\Public\Bandit_Heavy_State_Run.h"
#include "GameInstance.h"
void CBandit_Heavy_State_Run::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);	
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Run::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Look_At_Target();


	m_fWalkDelay += fTimeDelta;

	if (0.40f <= m_fWalkDelay)
	{
		pActor->Play_Sound_FootStep();
		//pActor->Play_Sound_FootStep();

		m_fWalkDelay = 0.f;
	}

	return nullptr;
}

void CBandit_Heavy_State_Run::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}

CState<CBandit_Heavy>* CBandit_Heavy_State_Run::Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Run_State(pActor, fTimeDelta, _iAnimIndex);
}
