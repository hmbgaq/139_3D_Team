#include "..\Public\Player_InteractionVault200.h"

void CPlayer_InteractionVault200::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_UseGravity(false);
}

CState<CPlayer>* CPlayer_InteractionVault200::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(54);
		if (true == m_bFlags[0])
		{
			pActor->Set_UseGravity(true);
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionVault200::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
