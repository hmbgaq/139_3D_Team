#include "..\Public\Tank_MeleeDynamicShield_05.h"
#include "Tank_MeleeDynamicShield_05_FollowUp_01.h"

void CTank_MeleeDynamicShield_05::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, true);
	pWeapon
		->Set_Damage(10)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.2f)
		;

	pWeapon->Set_Enable_Collisions(false);

}

CState<CTank>* CTank_MeleeDynamicShield_05::Update(CTank* pActor, _float fTimeDelta)
{
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(25);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(TANK_WEAPON_PUNCH_L, true);
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(35);
		if (true == m_bFlags[1])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(TANK_WEAPON_PUNCH_L, false);
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CTank_MeleeDynamicShield_05_FollowUp_01();
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_MeleeDynamicShield_05::Release(CTank* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
}
