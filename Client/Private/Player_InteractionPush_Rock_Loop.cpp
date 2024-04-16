#include "..\Public\Player_InteractionPush_Rock_Loop.h"
#include "GameInstance.h"
#include "Player_InteractionPush_Rock_Push_to_Pull.h"
#include "Player_InteractionPush_Rock_Loop.h"
#include "Player_InteractionPush_Rock_End.h"

void CPlayer_InteractionPush_Rock_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, false, 0);
	//pActor->Set_Animation_End(false);

	pActor->Set_Rotate_In_CameraDir(false);
	m_pGameInstance->Play_Sound(L"INTERACT_OBJECT", L"Cart_Mechanism_loop.wav", SOUND_INTERACTOBJECT2, 7.f);

}

CState<CPlayer>* CPlayer_InteractionPush_Rock_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);


	if (false == m_bFlags[0])
	{
		//pActor->Go_Straight(fTimeDelta * 0.25f);
		_float3 vMovePos = { 0.f, 0.f, 2.f * fTimeDelta };
		pActor->Set_AddRootMotion(vMovePos);
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
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

void CPlayer_InteractionPush_Rock_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
