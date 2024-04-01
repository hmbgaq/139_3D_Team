#include "..\Public\Player_Interaction_Glamour.h"

void CPlayer_Interaction_Glamour::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_Hp(100.f);
}

CState<CPlayer>* CPlayer_Interaction_Glamour::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(24);
		//if (true == m_bFlags[0])
		//{
		//	
		//}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Interaction_Glamour::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
