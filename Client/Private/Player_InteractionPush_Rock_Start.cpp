#include "..\Public\Player_InteractionPush_Rock_Start.h"
#include "GameInstance.h"
#include "Player_InteractionPush_Rock_Push_to_Pull.h"
#include "Player_InteractionPush_Rock_Loop.h"
#include "Player_InteractionPush_Rock_End.h"

void CPlayer_InteractionPush_Rock_Start::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Rotate_In_CameraDir(false);

	pActor->Set_Interection(true);
}

CState<CPlayer>* CPlayer_InteractionPush_Rock_Start::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		if (m_pGameInstance->Key_Down(DIK_W))
		{
			return new CPlayer_InteractionPush_Rock_Loop();
		}

		if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			return new CPlayer_InteractionPush_Rock_Push_to_Pull();
		}

		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			return new CPlayer_InteractionPush_Rock_End();
		}
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionPush_Rock_Start::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
