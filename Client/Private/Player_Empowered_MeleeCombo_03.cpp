#include "..\Public\Player_Empowered_MeleeCombo_03.h"

#include "Player_Empowered_Idle.h"

void CPlayer_Empowered_MeleeCombo_03::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));

	pWeapon
		->Set_Damage(0)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.5f);

	pWeapon->Set_Enable(true);
}

CState<CPlayer>* CPlayer_Empowered_MeleeCombo_03::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Empowered_MeleeCombo_03::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
	pWeapon->Set_Enable(false);
}



