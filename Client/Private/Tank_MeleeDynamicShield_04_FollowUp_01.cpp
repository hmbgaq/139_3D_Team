#include "..\Public\Tank_MeleeDynamicShield_04_FollowUp_01.h"

void CTank_MeleeDynamicShield_04_FollowUp_01::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, true);
	pWeapon
		->Set_Damage(10)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.5f)
		;

	pWeapon->Set_Enable_Collisions(false);

	pActor->Look_At_Target();

}

CState<CTank>* CTank_MeleeDynamicShield_04_FollowUp_01::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Look_At_Target_Lerp(fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(5);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(TANK_WEAPON_PUNCH_L, true);
			pActor->Play_Sound_Attack_Shield();
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(15);
		if (true == m_bFlags[1])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(TANK_WEAPON_PUNCH_L, false);
		}
	}
	else
	{
		pActor->Look_At_Target_Lerp(fTimeDelta);
	}

	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_MeleeDynamicShield_04_FollowUp_01::Release(CTank* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
}
