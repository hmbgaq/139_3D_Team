#include "..\Public\Player_InteractionJumpDown300.h"

void CPlayer_InteractionJumpDown300::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);
}

CState<CPlayer>* CPlayer_InteractionJumpDown300::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

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
	pActor->Set_InitPosition(pActor->Get_Position());
	__super::Release(pActor);
}
