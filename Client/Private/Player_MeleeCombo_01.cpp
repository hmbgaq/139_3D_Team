#include "..\Public\Player_MeleeCombo_01.h"

#include "Player_Empowered_Idle.h"

void CPlayer_MeleeCombo_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Get_Weapon(TEXT("Weapon_Punch_R"))->Set_Enable(true);
}

CState<CPlayer>* CPlayer_MeleeCombo_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//if (pActor->Is_Inputable_Front(10))
	//{
	//	pActor->Get_Weapon(TEXT("Weapon_Punch_R"))->Set_Enable(false);
	//}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_MeleeCombo_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	pActor->Get_Weapon(TEXT("Weapon_Punch_R"))->Set_Enable(false);
}
