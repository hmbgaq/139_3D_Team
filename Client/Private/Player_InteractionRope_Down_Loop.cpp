#include "..\Public\Player_InteractionRope_Down_Loop.h"
#include "Player_InteractionRope_Down_Stop.h"

void CPlayer_InteractionRope_Down_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_Animation_End(false);
}

CState<CPlayer>* CPlayer_InteractionRope_Down_Loop::Update(CPlayer* pActor, _float fTimeDelta)
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
			return new CPlayer_InteractionRope_Down_Stop();
		}
		else
		{
			return new CPlayer_InteractionRope_Down_Loop();
			//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
		}
	}

	return nullptr;
}

void CPlayer_InteractionRope_Down_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
