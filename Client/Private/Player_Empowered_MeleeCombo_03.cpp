#include "..\Public\Player_Empowered_MeleeCombo_03.h"

#include "Player_Empowered_Idle.h"

void CPlayer_Empowered_MeleeCombo_03::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(WEAPON_PUNCH_R);

	pWeapon
		->Set_Damage(0)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.5f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);
}

CState<CPlayer>* CPlayer_Empowered_MeleeCombo_03::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0] && pActor->Is_Inputable_Front(20))
	{
		CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(WEAPON_PUNCH_R, true);
		m_bFlags[0] = true;
	}

	if (true == m_bFlags[0] && false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta);
	}

	if (false == m_bFlags[1] && pActor->Is_Inputable_Front(24))
	{
		CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(WEAPON_PUNCH_R, false);
		m_bFlags[1] = true;
	}

	if (pActor->Is_Inputable_Front(23))
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_Empowered_MeleeCombo_03::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(WEAPON_PUNCH_R, false);
	pActor->Set_Target(nullptr);
}



