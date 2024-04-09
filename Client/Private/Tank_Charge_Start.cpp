#include "..\Public\Tank_Charge_Start.h"
#include "Tank_Charge_Loop.h"

void CTank_Charge_Start::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Reset_AttackCount();

	//CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, true);
	//pWeapon
	//	->Set_Damage(10)
	//	->Set_Direction(Direction::Front)
	//	->Set_Power(Power::Medium)
	//	->Set_Force(0.5f)
	//	;

	//pWeapon->Set_Enable_Collisions(false);

}

CState<CTank>* CTank_Charge_Start::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(34);
		if (true == m_bFlags[0])
		{
			//pActor->Set_Weapon_Collisions_Enable(TANK_WEAPON_PUNCH_L, true);
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CTank_Charge_Loop();
	}

	return nullptr;// __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Start::Release(CTank* pActor)
{
	__super::Release(pActor);
	//CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
}
