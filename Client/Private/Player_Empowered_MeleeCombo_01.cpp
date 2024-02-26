#include "..\Public\Player_Empowered_MeleeCombo_01.h"

#include "Player_Empowered_Idle.h"

void CPlayer_Empowered_MeleeCombo_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_Empowered_MeleeCombo_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	if (true == pActor->Is_Animation_End())
	{
		return new CPlayer_Empowered_Idle();
	}

	return nullptr;
}

void CPlayer_Empowered_MeleeCombo_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
