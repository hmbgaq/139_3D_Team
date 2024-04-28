#include "VampireCommander_Ranged3.h"
#include "VampireCommander_Idle.h"
#include "GameInstance.h"
#include "Body_VampireCommander.h"

#include "VampireCommander_Weapon.h"

void CVampireCommander_Ranged3::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);

	pActor->m_bLookAt = false;


	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(true);	// 트레일 켜기

	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(true);	// 트레일 켜기

	m_fWingTimeArray[0] = 29.f;
	m_fWingTimeArray[1] = 65.f;
	
	m_bPlayWingSoundArray[0] = false;
	m_bPlayWingSoundArray[1] = false;

}
 
CState<CVampireCommander>* CVampireCommander_Ranged3::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	//CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));

	

	PlaySound_Wing(pActor);

	if (m_bFlags[1] == false)
	{
		//pActor->m_bLookAt = true;
	}

	if (pActor->Is_Inputable_Front(66))
	{
		m_bFlags[1] = true;
		if (m_bfirstCheck)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_MonsterBullet", L"Prototype_GameObject_VampireCommander_Projectile_Range3");
			m_bfirstCheck = false;
		}

		
	}
	else if (pActor->Is_Inputable_Front(68))
	{
		m_bFlags[1] = false;
	}

	if (pActor->Is_Animation_End())
	{

		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_Ranged3::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);


	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(false);	// 트레일 켜기

	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(false);	// 트레일 켜기

	pActor->m_bLookAt = true;


	m_fWingTimeArray[0] = 0.f;
	m_fWingTimeArray[1] = 0.f;

	m_bPlayWingSoundArray[0] = false;
	m_bPlayWingSoundArray[1] = false;
}

void CVampireCommander_Ranged3::PlaySound_Wing(CVampireCommander* pActor)
{
	if (m_bPlayWingSoundArray[0] == false)
	{
		if (pActor->Is_Inputable_Front((_uint)m_fWingTimeArray[0]))
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_WING", L"commander_lesser_mvm_wings004.wav", SOUND_ENEMY_BODYMOVE, 10.f);
			m_bPlayWingSoundArray[0] = true;
		}
	}
	
	if (m_bPlayWingSoundArray[1] == false)
	{
		if (pActor->Is_Inputable_Front((_uint)m_fWingTimeArray[1]))
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_WING", L"commander_lesser_mvm_wings004.wav", SOUND_ENEMY_BODYMOVE2, 10.f);
			m_bPlayWingSoundArray[1] = true;
		}
	}
	
}
