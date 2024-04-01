#include "..\Public\Player_InteractionLadder_Up_IdlePose.h"
#include "GameInstance.h"
#include "Player_InteractionLadder_Up_Loop.h"
#include "Player_InteractionLadder_Up_Stop.h"


void CPlayer_InteractionLadder_Up_IdlePose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionLadder_Up_IdlePose::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		if (false == pActor->Is_Exist_Ladder_Count())
		{
			return new CPlayer_InteractionLadder_Up_Stop();
		}
		else if (m_pGameInstance->Key_Down(DIK_W))
		{
			return new CPlayer_InteractionLadder_Up_Loop();
		}
	}

	return nullptr;
}

void CPlayer_InteractionLadder_Up_IdlePose::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
