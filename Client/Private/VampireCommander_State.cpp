#include "VampireCommander_State.h"
#include "GameInstance.h"

#include "VampireCommander_Idle.h"

void CVampireCommander_State::Initialize(CVampireCommander* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CVampireCommander>* CVampireCommander_State::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CVampireCommander_State::Release(CVampireCommander* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CVampireCommander>* CVampireCommander_State::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Normal_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CVampireCommander>* CVampireCommander_State::Walk_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Attack_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::HitNormal_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Stun_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Taunt_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Spawn_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Dead_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Normal(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CVampireCommander>* pState = { nullptr };


	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}

	return nullptr;
}

CState<CVampireCommander>* CVampireCommander_State::Idle(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}
