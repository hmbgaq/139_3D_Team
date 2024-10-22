#include "..\Public\Player_InteractionJumpDown300.h"
#include "GameInstance.h"

void CPlayer_InteractionJumpDown300::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	if(m_pGameInstance->Get_NextLevel() != _uint(LEVEL_GAMEPLAY) && m_pGameInstance->Get_NextLevel() != _uint(LEVEL_SNOWMOUNTAIN))
		pActor->Set_UseGravity(true);
}

CState<CPlayer>* CPlayer_InteractionJumpDown300::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (m_pGameInstance->Get_NextLevel() != _uint(LEVEL_GAMEPLAY) && m_pGameInstance->Get_NextLevel() != _uint(LEVEL_SNOWMOUNTAIN))
	{
		pActor->Set_UseGravity(true);
	}


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(24);
		if (true == m_bFlags[0])
		{
			pActor->Set_UseGravity(true);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionJumpDown300::Release(CPlayer* pActor)
{
	if(m_pGameInstance->Get_NextLevel() == (_uint)LEVEL_SNOWMOUNTAINBOSS)
		pActor->Set_InitPosition(pActor->Get_Position());

	__super::Release(pActor);
}
