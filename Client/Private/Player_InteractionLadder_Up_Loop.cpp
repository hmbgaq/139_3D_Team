#include "..\Public\Player_InteractionLadder_Up_Loop.h"
#include "GameInstance.h"
#include "Player_InteractionLadder_Up_Stop.h"
#include "Player_InteractionLadder_Up_IdlePose.h"

void CPlayer_InteractionLadder_Up_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionLadder_Up_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		pActor->Remove_Ladder_Count();

		if (false == pActor->Is_Exist_Ladder_Count())
		{
			return new CPlayer_InteractionLadder_Up_Stop();
		}
		else
		{
			return new CPlayer_InteractionLadder_Up_IdlePose();
		}
	}

	return nullptr;
}

void CPlayer_InteractionLadder_Up_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
