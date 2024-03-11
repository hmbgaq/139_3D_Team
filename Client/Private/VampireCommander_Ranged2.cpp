#include "VampireCommander_Ranged2.h"
#include "VampireCommander_Idle.h"

void CVampireCommander_Ranged2::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_L"));

	pWeapon
		->Set_Damage(30.f)
		->Set_Direction(Direction::Left)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	
}

CState<CVampireCommander>* CVampireCommander_Ranged2::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	if (pActor->Is_Inputable_Front(37))
	{
		pWeapon->Set_Enable(true);

	}
	else if (pActor->Is_Inputable_Front(39))
	{
		pWeapon->Set_Enable(false);
	}

	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_Ranged2::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	pWeapon->Set_Enable(false);
}
