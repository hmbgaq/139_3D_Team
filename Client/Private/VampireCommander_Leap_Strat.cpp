#include "VampireCommander_Leap_Strat.h"
#include "VampireCommander_Leap_Stop.h"
#include "Collider.h"

#include "Effect_Manager.h"
#include "GameInstance.h"

void CVampireCommander_Leap_Strat::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_UseGravity(false);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true);
	pActor->m_bLookAt = true;
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));

	pWeapon
		->Set_Damage(50)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	_float4x4	Temp = XMMatrixIdentity();
	Temp.m[0][0] = 2.f;
	Temp.m[1][1] = 2.f;
	Temp.m[2][2] = 2.f;

	pWeapon->Get_Colliders().back()->Get_Bounding()->Set_matScale(Temp);
	m_pGameInstance->Play_Sound(L"VAMPIRE_GROUNDSLAM", L"commander_lesser_attack_ground_slam_start001.wav", SOUND_ENEMY_SKILL1, 10.f);
	m_pGameInstance->Play_Sound(L"VAMPIRE_TAUNT", L"commander_lesser_vo_taunt005.wav", SOUND_ENEMY_SKILL2, 7.f);
	
	
}

CState<CVampireCommander>* CVampireCommander_Leap_Strat::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	if (pActor->Is_Inputable_Front(34))
	{
		pActor->Move_In_Proportion_To_Enemy(fTimeDelta*0.5f,0.3f);/*�� �� �ǵ帮���� �� ���� ����*/ /*! �� ~~ �ǵ帮�� �ʹ�~~*/
		
	}
	else if (pActor->Is_Inputable_Front(58))
	{
		pActor->Move_In_Proportion_To_Enemy(0, 0);
	}

	if (m_bFlags[1] == false && pActor->Is_Inputable_Front(34))
	{
		// ��Ƣ��� ����Ʈ �߰�
		m_pGameInstance->Play_Sound(L"VAMPIRE_BODYFALL", L"commander_lesser_mvm_bodyfall001.wav", SOUND_ENEMY_SKILL3, 10.f);
		m_bFlags[1] = true;
	}
	

	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(68))
	{
		// ��Ƣ��� ����Ʈ �߰�
		EFFECT_MANAGER->Play_Effect("VampireCommander/", "landing_Rock_01.json", nullptr, pActor->Get_Position_Vector());
		m_pGameInstance->Play_Sound(L"VAMPIRE_GROUNDSLAM", L"commander_lesser_attack_ground_slam_impact001.wav", SOUND_ENEMY_SKILL4, 10.f);
		m_bFlags[0] = true;
	}

	if (pActor->Is_Inputable_Front(55))
	{
		pWeapon->Set_Enable(true);
		pActor->Set_UseGravity(true);
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
