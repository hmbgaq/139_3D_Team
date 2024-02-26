#include "..\Public\Player_State.h"
#include "GameInstance.h"

#include "Player_Empowered_MeleeCombo_01.h"

void CPlayer_State::Initialize(CPlayer* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CPlayer>* CPlayer_State::Update(CPlayer* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CPlayer_State::Release(CPlayer* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CPlayer>* CPlayer_State::MeleeCombo(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		return new CPlayer_Empowered_MeleeCombo_01();
	}

	return nullptr;
}
