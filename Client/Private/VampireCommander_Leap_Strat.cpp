#include "VampireCommander_Leap_Strat.h"
#include "VampireCommander_Leap_Stop.h"

void CVampireCommander_Leap_Strat::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true);
	pActor->m_bLookAt = true;
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));

	pWeapon
		->Set_Damage(0.f)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	pWeapon->Set_Enable(true);
}

CState<CVampireCommander>* CVampireCommander_Leap_Strat::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	pActor->Move_In_Proportion_To_Enemy(fTimeDelta);

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
