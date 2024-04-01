#include "..\Public\Tank_Charge_Loop.h"
#include "SMath.h"
#include "Tank_Charge_Fail_L.h"
#include "Tank_Charge_Hit_L.h"

void CTank_Charge_Loop::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);

	//CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, true);
	//pWeapon
	//	->Set_Damage(10)
	//	->Set_Direction(Direction::Front)
	//	->Set_Power(Power::Medium)
	//	->Set_Force(0.5f)
	//	;

	//pWeapon->Set_Enable_Collisions(true);

}

CState<CTank>* CTank_Charge_Loop::Update(CTank* pActor, _float fTimeDelta)
{
	m_fTime += fTimeDelta;

	pActor->Go_Straight(fTimeDelta);
	pActor->Look_At_Target_Lerp(fTimeDelta * 0.25f);
	
	if (4.f <= m_fTime)
	{
		return new CTank_Charge_Fail_L();
	}

	CCharacter* pTarget = pActor->Get_Target();
	if (pTarget)
	{
		_float fDistance = pActor->Calc_Distance();
		if (2.f > fDistance)
		{
			return new CTank_Charge_Hit_L();
		}

	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Charge_Loop::Release(CTank* pActor)
{
	__super::Release(pActor);
	//CWeapon* pWeapon = pActor->Set_Weapon_Enable(TANK_WEAPON_PUNCH_L, false);
}
