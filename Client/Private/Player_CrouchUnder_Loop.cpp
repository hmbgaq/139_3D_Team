#include "..\Public\Player_CrouchUnder_Loop.h"

#include "Player_CrouchUnder_End.h"

void CPlayer_CrouchUnder_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_CrouchUnder_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		pActor->Remove_Ladder_Count();

		if (false == pActor->Is_Exist_Ladder_Count())
		{
			return new CPlayer_CrouchUnder_End();
		}
		else
		{
			return new CPlayer_CrouchUnder_Loop();
		}
	}

	return nullptr;
}

void CPlayer_CrouchUnder_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
