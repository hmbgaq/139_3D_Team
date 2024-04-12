#include "VampireCommander_Ranged1.h"
#include "VampireCommander_Idle.h"
#include "GameInstance.h"
#include "Body_VampireCommander.h"

#include "VampireCommander_Weapon.h"

void CVampireCommander_Ranged1::Initialize(CVampireCommander* pActor)
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
}

CState<CVampireCommander>* CVampireCommander_Ranged1::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));

	if (m_bFlags[1] == false)
	{
		//pActor->m_bLookAt = true;
	}

	if (pActor->Is_Inputable_Front(51)) 
	{
		m_bFlags[1] = true;
		if (m_bfirstCheck)
		{
			m_pGameInstance->Add_CloneObject(LEVEL_INTRO_BOSS, L"Layer_Monster", L"Prototype_GameObject_VampireCommander_Projectile_Range1");
			m_bfirstCheck = false;
		}
	}
	else if(pActor->Is_Inputable_Front(55))
		m_bFlags[1] = false;
	if (pActor->Is_Animation_End())
	{
		

		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_Ranged1::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(false);	// 트레일 켜기

	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(false);	// 트레일 켜기
	pActor->m_bLookAt = true;

}
