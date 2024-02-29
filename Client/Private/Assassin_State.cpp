#include "..\Public\Assassin_State.h"
#include "GameInstance.h"

#include "Assassin_IdleAct_01.h"
#include "Assassin_IdleAct_02.h"
#include "Assassin_IdleAct_03.h"



void CAssassin_State::Initialize(CAssassin* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

}

CState<CAssassin>* CAssassin_State::Update(CAssassin* pActor, _float fTimeDelta)
{

	return nullptr;
}

void CAssassin_State::Release(CAssassin* pActor)
{
	Safe_Release(m_pGameInstance);

}

CState<CAssassin>* CAssassin_State::Normal_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CAssassin>* CAssassin_State::Normal(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CAssassin>* pState = { nullptr };


	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CAssassin_IdleAct_01();
	}

	return nullptr;
}

CState<CAssassin>* CAssassin_State::Idle(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CAssassin>* pState = { nullptr };

	if (pActor->Is_Animation_End())
	{
		_uint iRand = rand() % 3;
		switch (iRand)
		{
		case 0:
			pState = new CAssassin_IdleAct_01();
			break;
		case 1:
			pState = new CAssassin_IdleAct_02();
			break;
		case 2:
			pState = new CAssassin_IdleAct_03();
			break;
		default:
			pState = new CAssassin_IdleAct_01();
			break;
		}

	}

	return pState;
}

