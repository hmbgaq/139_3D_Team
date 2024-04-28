#include "..\Public\Player_InteractionClimb450.h"
#include "Navigation.h"
#include "GameInstance.h"

void CPlayer_InteractionClimb450::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_UseGravity(false);
}

CState<CPlayer>* CPlayer_InteractionClimb450::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[10])
	{
		m_bFlags[10] = pActor->Is_Inputable_Front(40);
		if (true == m_bFlags[10])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"oneliner_Climb up_jesse_1_alt5.wav", CHANNELID::SOUND_INTERACTION, 10.f);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_04.wav", CHANNELID::SOUND_INTERACTION2, 10.f);

		}
	}
	else if (false == m_bFlags[11])
	{
		m_bFlags[11] = pActor->Is_Inputable_Front(55);
		if (true == m_bFlags[11])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"oneliner_Climb up_jesse_1_alt6.wav", CHANNELID::SOUND_INTERACTION, 10.f);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_05.wav", CHANNELID::SOUND_INTERACTION2, 10.f);

		}
	}
	else if (false == m_bFlags[12])
	{
		m_bFlags[12] = pActor->Is_Inputable_Front(55);
		if (true == m_bFlags[12])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"oneliner_Climb up_jesse_1_alt6.wav", CHANNELID::SOUND_INTERACTION, 10.f);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_05.wav", CHANNELID::SOUND_INTERACTION2, 10.f);

		}
	}
	else if (false == m_bFlags[13])
	{
		m_bFlags[13] = pActor->Is_Inputable_Front(84);
		if (true == m_bFlags[13])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"oneliner_Climb up_jesse_1_alt7.wav", CHANNELID::SOUND_INTERACTION, 10.f);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_06.wav", CHANNELID::SOUND_INTERACTION2, 10.f);

		}
	}
	else if (false == m_bFlags[14])
	{
		m_bFlags[14] = pActor->Is_Inputable_Front(101);
		if (true == m_bFlags[14])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"interaction_ladderclimb_3.wav", CHANNELID::SOUND_INTERACTION, 10.f);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_07.wav", CHANNELID::SOUND_INTERACTION2, 10.f);

		}
	}
	else if (false == m_bFlags[15])
	{
		m_bFlags[15] = pActor->Is_Inputable_Front(117);
		if (true == m_bFlags[15])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Steps_08.wav", CHANNELID::SOUND_INTERACTION2, 10.f);
		}
	}


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(90);
		if (true == m_bFlags[0])
		{
			pActor->Set_UseGravity(true);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimb450::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}