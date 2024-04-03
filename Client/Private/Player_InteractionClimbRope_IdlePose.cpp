#include "..\Public\Player_InteractionClimbRope_IdlePose.h"

#include "GameInstance.h"
#include "Player_InteractionClimbRope_Loop_01.h"
#include "Player_InteractionClimbRope_Stop.h"

void CPlayer_InteractionClimbRope_IdlePose::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_InteractionClimbRope_IdlePose::Update(CPlayer* pActor, _float fTimeDelta)
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
			return new CPlayer_InteractionClimbRope_Stop();
		}
		else if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			return new CPlayer_InteractionClimbRope_Loop_01();
		}
	}

	return nullptr;
}

void CPlayer_InteractionClimbRope_IdlePose::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
