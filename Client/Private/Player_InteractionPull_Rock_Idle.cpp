#include "..\Public\Player_InteractionPull_Rock_Idle.h"
#include "GameInstance.h"
#include "Player_InteractionPush_Rock_Pull_to_Push.h"
#include "Player_InteractionPull_Rock_Loop.h"
#include "Player_InteractionPull_Rock_End.h"


void CPlayer_InteractionPull_Rock_Idle::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Rotate_In_CameraDir(false);
}

CState<CPlayer>* CPlayer_InteractionPull_Rock_Idle::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else 
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			return new CPlayer_InteractionPush_Rock_Pull_to_Push();
		}

		if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			return new CPlayer_InteractionPull_Rock_Loop();
		}

		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			return new CPlayer_InteractionPull_Rock_End();
		}
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionPull_Rock_Idle::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
