#include "MotherMouth_State.h"
#include "GameInstance.h"
#include "MotherMouth_SpittingMineStart.h"

void CMotherMouth_State::Initialize(CMotherMouth* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CMotherMouth>* CMotherMouth_State::Update(CMotherMouth* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CMotherMouth_State::Release(CMotherMouth* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CMotherMouth>* CMotherMouth_State::Update_State(CMotherMouth* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMotherMouth>* CMotherMouth_State::Normal_State(CMotherMouth* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMotherMouth>* CMotherMouth_State::Attack_State(CMotherMouth* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMotherMouth>* CMotherMouth_State::Normal(CMotherMouth* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Get_Enable() == true)
	{
		return new CMotherMouth_SpittingMineStart;
	}

	return nullptr;
}
