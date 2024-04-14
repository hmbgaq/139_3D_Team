#include "..\Public\Player_InteractionClimb200.h"
#include "GameInstance.h"

void CPlayer_InteractionClimb200::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);

	m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"interaction_climbup_4.wav", CHANNELID::SOUND_INTERACTION, 10.f);

}

CState<CPlayer>* CPlayer_InteractionClimb200::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[10])
	{
		m_bFlags[10] = pActor->Is_Inputable_Front(28);
		if (true == m_bFlags[10])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_01.wav", CHANNELID::SOUND_INTERACTION, 10.f);
		}
	}
	else if (false == m_bFlags[11])
	{
		m_bFlags[11] = pActor->Is_Inputable_Front(39);
		if (true == m_bFlags[11])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_03.wav", CHANNELID::SOUND_INTERACTION, 10.f);
		}
	}
	else if (false == m_bFlags[12])
	{
		m_bFlags[12] = pActor->Is_Inputable_Front(62);
		if (true == m_bFlags[12])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_05.wav", CHANNELID::SOUND_INTERACTION, 10.f);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimb200::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
