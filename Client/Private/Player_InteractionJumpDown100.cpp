#include "..\Public\Player_InteractionJumpDown100.h"
#include "GameInstance.h"

void CPlayer_InteractionJumpDown100::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);

}

CState<CPlayer>* CPlayer_InteractionJumpDown100::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[10])
	{
		m_bFlags[10] = pActor->Is_Inputable_Front(22);
		if (true == m_bFlags[10])
		{
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"interaction_jumpdown_2.wav", CHANNELID::SOUND_INTERACTION, 10.f);
			m_pGameInstance->Play_Sound(L"PLAYER_INTERACTION", L"HM_PlayerFoley_JumpOff_02.wav", CHANNELID::SOUND_INTERACTION2, 10.f);
		}
	}
	else if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(27);
		if (true == m_bFlags[0])
		{
			pActor->Set_UseGravity(true);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionJumpDown100::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
