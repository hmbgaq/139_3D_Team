#include "..\Public\Player_InteractionClimbRope_Stop.h"
#include "Environment_Interact.h"

void CPlayer_InteractionClimbRope_Stop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Interection(false);
}

CState<CPlayer>* CPlayer_InteractionClimbRope_Stop::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(8);

		if (true == m_bFlags[0]) 
		{
			CEnvironment_Interact* pInteractObject = pActor->Get_InteractObject();
			if (pInteractObject)
			{
				pInteractObject->Set_Animation(7);
			}
		}
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionClimbRope_Stop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
