#include "..\Public\Tank_Charge_Hit_L.h"

void CTank_Charge_Hit_L::Initialize(CTank* pActor)
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

	pWeapon->Set_Enable_Collisions(true);

	pActor->Play_Sound_Voice_Attack();
	pActor->Play_Sound_Attack_Shield();
}

CState<CTank>* CTank_Charge_Hit_L::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Look_At_Target_Lerp(fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(12);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
		}
	}


	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Hit_L::Release(CTank* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, true);
}
