#include "..\Public\Player_DeathLight_F_02.h"

#include "Player_Empowered_Idle.h"

void CPlayer_DeathLight_F_02::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_DeathLight_F_02::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();//Is_Inputable_Front(23)
		if (true == m_bFlags[0])
		{
			pActor->Set_DiedScreen(true);
			pActor->Set_Animation(CPlayer::Player_State::Player_IdleLoop, CModel::ANIM_STATE::ANIM_STATE_NORMAL, false, false, 0);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_DeathLight_F_02::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
