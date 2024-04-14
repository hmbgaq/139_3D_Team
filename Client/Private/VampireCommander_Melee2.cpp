#include "VampireCommander_Melee2.h"
#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"
#include "VampireCommander_Weapon.h"
#include "GameInstance.h"

void CVampireCommander_Melee2::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));

	pWeapon
		->Set_Damage(30)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	dynamic_cast<CVampireCommander_Weapon*>(pWeapon)->Play_Trail(true);	// 트레일 켜기

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);

	pActor->m_bLookAt = false;

	m_pGameInstance->Play_Sound(L"VAMPIRE_WING", L"commander_lesser_mvm_wings004.wav", SOUND_ENEMY_BODYMOVE, 8.f);
}

CState<CVampireCommander>* CVampireCommander_Melee2::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	if (m_bFlags[1] == false)
	{
		//pActor->m_bLookAt = true;
	}

	if (pActor->Is_Inputable_Front(38))
	{
		m_pGameInstance->Play_Sound(L"VAMPIRE_ATTACK", L"william_attack_melee_whoosh001.wav", SOUND_ENEMY_ATTACK, 8.f);
		m_bFlags[1] = true;
		pWeapon->Set_Enable(true);

	}
	else if (pActor->Is_Inputable_Front(45))
	{
		m_bFlags[1] = false;
		pWeapon->Set_Enable(false);
	}

	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}

	return nullptr;
}

void CVampireCommander_Melee2::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon)->Play_Trail(false);	// 트레일 끄기

	pWeapon->Set_Enable(false);

	pActor->m_bLookAt = true;

}
