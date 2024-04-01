#include "Mother_State.h"
#include "GameInstance.h"
#include "Mother_StunStart.h"
#include "Mother_SpittingStart.h"
#include "Mother_VomitStart.h"
#include "Mother_Dead.h"

void CMother_State::Initialize(CMother* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CMother>* CMother_State::Update(CMother* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CMother_State::Release(CMother* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CMother>* CMother_State::Update_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMother>* CMother_State::Normal_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CMother>* CMother_State::Attack_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMother>* CMother_State::Stun_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMother>* CMother_State::Normal(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	
	//TODO 페이즈1 //////////////////////////////////////////
	if (pActor->m_bPhase == true)
	{
		if (pActor->m_iSonDead == 1 && pActor->m_bfirstCheck)
		{
			pActor->m_bfirstCheck = false;

			return new CMother_VomitStart;
		}
		else if (pActor->m_iSonDead >= 2)//Son이 두마리다 죽었을때
		{
			return new CMother_StunStart;
		}
	}
	//TODO 페이즈2,3 //////////////////////////////////////////
	else if(pActor->m_bPhase == false)
	{
		if (pActor->m_iSonDead == 0)
			pActor->m_bfirstCheck = true;

		pActor->m_fTimeDelta += fTimeDelta;

		if (pActor->m_fTimeDelta >= 15)
		{
			pActor->m_fTimeDelta = 0.f;
			return new CMother_SpittingStart;
		}

		if (pActor->m_iSonDead == 1 && pActor->m_bfirstCheck)
		{
			pActor->m_bfirstCheck = false;
			return new CMother_VomitStart;
		}
		else if (pActor->m_iSonDead >= 2)//Son이 두마리다 죽었을때
		{
			return new CMother_StunStart;
		}
	}
	if (pActor->Get_CurHP() <= 0.f)
		return new CMother_Dead;
	

	return nullptr;
}
