#include "Tank_State.h"
#include "GameInstance.h"
#include "Tank_Idle.h"

void CTank_State::Initialize(CTank* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CTank>* CTank_State::Update(CTank* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CTank_State::Release(CTank* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CTank>* CTank_State::Normal_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CTank>* CTank_State::Walk_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Run_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Attack_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Hit_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Knock_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Death_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}


CState<CTank>* CTank_State::Spawn_State(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CTank>* CTank_State::Normal(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CTank>* pState = { nullptr };

	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	if (pActor->Is_Animation_End())
	{
		return new CTank_Idle();
	}

	return nullptr;
}

CState<CTank>* CTank_State::Idle(CTank* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CTank>* pState = { nullptr };



	return pState;
}

