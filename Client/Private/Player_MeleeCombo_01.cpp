#include "..\Public\Player_MeleeCombo_01.h"

#include "Weapon.h"

void CPlayer_MeleeCombo_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
	
	pWeapon
		->Set_Damage(0.f)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Light)
		->Set_Force(0.5f);

	pWeapon->Set_Enable(true);

}

CState<CPlayer>* CPlayer_MeleeCombo_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);


	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_MeleeCombo_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
	pWeapon->Set_Enable(false);
}
