#include "VampireCommander_Leap_Strat.h"
#include "VampireCommander_Leap_Stop.h"
#include "Collider.h"

void CVampireCommander_Leap_Strat::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true);
	pActor->m_bLookAt = true;
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));

	pWeapon
		->Set_Damage(50)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	_float4x4	Temp = XMMatrixIdentity();
	Temp.m[0][0] = 1.5f;
	Temp.m[1][1] = 1.5f;
	Temp.m[2][2] = 1.5f;

	pWeapon->Get_Colliders().back()->Get_Bounding()->Set_matScale(Temp);
	
}

CState<CVampireCommander>* CVampireCommander_Leap_Strat::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	if (pActor->Is_Inputable_Front(34))
	{
		pActor->Move_In_Proportion_To_Enemy(fTimeDelta*0.5f,0.3f);// ÀÌ °ª °Çµå¸®Áö¸¶ µü ¸ÂÃç ³ù´Ù 
	}
	else if (pActor->Is_Inputable_Front(58))
	{
		pActor->Move_In_Proportion_To_Enemy(fTimeDelta, 0);
	}

	if (pActor->Is_Inputable_Front(55))
	{
		pWeapon->Set_Enable(true);
	}
	else if (pActor->Is_Inputable_Front(75))
	{
		//pActor->m_bLookAt = true;
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

	_float4x4	Temp = XMMatrixIdentity();
	Temp.m[0][0] = 0.8f;
	Temp.m[1][1] = 0.8f;
	Temp.m[2][2] = 0.8f;

	pWeapon->Get_Colliders().back()->Get_Bounding()->Set_matScale(Temp);
}
