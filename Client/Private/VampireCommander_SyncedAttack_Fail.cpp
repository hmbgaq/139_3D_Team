#include "VampireCommander_SyncedAttack_Fail.h"
#include "VampireCommander_SyncedAttack.h"
#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"
#include "VampireCommander_Weapon.h"
#include "GameInstance.h"

void CVampireCommander_SyncedAttack_Fail::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Get_Weapon(L"Weapon_hand_R")->Set_Synced(true);

	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(true);	// 트레일 켜기
	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(true);	// 트레일 켜기
	pWeapon_R
		->Set_Damage(0)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.f);

	CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ATTACK);


	m_pGameInstance->Play_Sound(L"VAMPIRE_ATTACK", L"commander_lesser_vo_attackB004.wav", SOUND_ENEMY_VOICE, 8.f);

	pActor->m_bLookAt = false;

}

CState<CVampireCommander>* CVampireCommander_SyncedAttack_Fail::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	if (pActor->Is_Inputable_Front(44))
	{
		pWeapon->Set_Enable(true);
	}
	else if (pActor->Is_Inputable_Front(49))
	{
		pWeapon->Set_Enable(false);
	}

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(51);
		if (true == m_bFlags[0])
		{
			m_pGameInstance->Play_Sound(L"VAMPIRE_ATTACK", L"william_attack_melee_whoosh001.wav", SOUND_ENEMY_ATTACK, 8.f);
		}
	}
	//여기서 먼저 충돌을 검사를 하고 충돌에서 잡기 성공판정이면 분기점 Trackposition을 받아서 syncedAttack 으로 전환! syncedAttack에서는 분기점 trackposition부터 스타트
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_Idle();
	}
	//else
	//{
	//
	//}

	return nullptr;
}

void CVampireCommander_SyncedAttack_Fail::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon_R = pActor->Get_Weapon(TEXT("Weapon_hand_R"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_R)->Play_Trail(false);	// 트레일 켜기
	CWeapon* pWeapon_L = pActor->Get_Weapon(TEXT("Weapon_hand_L"));
	dynamic_cast<CVampireCommander_Weapon*>(pWeapon_L)->Play_Trail(false);	// 트레일 켜기

	pWeapon_R->Set_Enable(false);

	pActor->m_bLookAt = true;

}
