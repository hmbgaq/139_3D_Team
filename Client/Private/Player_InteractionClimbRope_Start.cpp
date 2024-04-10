#include "..\Public\Player_InteractionClimbRope_Start.h"

#include "Player_InteractionClimbRope_IdlePose.h"
#include "Player_InteractionClimbRope_Stop.h"

void CPlayer_InteractionClimbRope_Start::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Ladder_Count(6);

	pActor->Set_Interection(true);
}

CState<CPlayer>* CPlayer_InteractionClimbRope_Start::Update(CPlayer* pActor, _float fTimeDelta)
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
			return new CPlayer_InteractionClimbRope_Stop();
		}
		else
		{
			return new CPlayer_InteractionClimbRope_IdlePose();
		}
	}

	return nullptr;
}

void CPlayer_InteractionClimbRope_Start::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
