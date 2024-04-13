#include "..\Public\Player_State_Run.h"

void CPlayer_State_Run::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
}

CState<CPlayer>* CPlayer_State_Run::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	m_fWalkDelay += fTimeDelta;

	if (0.40f <= m_fWalkDelay)
	{
		pActor->Play_Run_Sound();
		m_fWalkDelay = 0.f;
	}

	return nullptr;
}

void CPlayer_State_Run::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

CState<CPlayer>* CPlayer_State_Run::Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Run_State(pActor, fTimeDelta, _iAnimIndex);
}
