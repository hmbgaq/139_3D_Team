#include "VampireCommander_Leap_Strat.h"
#include "VampireCommander_Leap_Stop.h"

void CVampireCommander_Leap_Strat::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true);
	pActor->m_bLookAt = true;
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));

	pWeapon
		->Set_Damage(50.f)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);
}

CState<CVampireCommander>* CVampireCommander_Leap_Strat::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	if (pActor->Is_Inputable_Front(34))
	{
		pActor->Move_In_Proportion_To_Enemy(fTimeDelta);
	}
	else if (pActor->Is_Inputable_Front(60))
	{
		pActor->Move_In_Proportion_To_Enemy(fTimeDelta, 0);
		
	}

	if (pActor->Is_Inputable_Front(55))
	{
		pWeapon->Set_Enable(true);
	}
	else if (pActor->Is_Inputable_Front(75))
	{
		pWeapon->Set_Enable(false);
	}

	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Leap_Stop();
	}

	return nullptr;
}

void CVampireCommander_Leap_Strat::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	pWeapon->Set_Enable(false);
}
