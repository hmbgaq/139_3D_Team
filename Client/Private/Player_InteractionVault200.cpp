#include "..\Public\Player_InteractionVault200.h"

void CPlayer_InteractionVault200::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);

	m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"interaction_jumpdown_3.wav", CHANNELID::SOUND_INTERACTION, 10.f);
}

CState<CPlayer>* CPlayer_InteractionVault200::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	
	if (false == m_bFlags[10]) 
	{
		m_bFlags[10] = pActor->Is_Inputable_Front(19);
		if (true == m_bFlags[10])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Ladder_Impact_02.wav", CHANNELID::SOUND_INTERACTION, 10.f);
		}
	}
	else if (false == m_bFlags[11])
	{
		m_bFlags[11] = pActor->Is_Inputable_Front(34);
		if (true == m_bFlags[11])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"Player_Climb_Ladder_Impact_04.wav", CHANNELID::SOUND_INTERACTION, 10.f);
		}
	}
	else if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(58);
		if (true == m_bFlags[0])
		{
			pActor->Set_UseGravity(true);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"HM_PlayerFoley_JumpOff_Short_01.wav", CHANNELID::SOUND_INTERACTION, 10.f);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionVault200::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
