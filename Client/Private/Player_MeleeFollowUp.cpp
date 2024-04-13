#include "Player_MeleeFollowUp.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_MeleeFollowUp::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 5);


	{
		CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_L);
		pWeapon
			->Set_Damage(7.f)
			->Set_Direction(Direction::Front)
			->Set_Power(Power::Medium)
			->Set_Force(0.0f);

		pWeapon->Set_Enable(true);
		pWeapon->Set_Enable_Collisions(false);
	}

	{
		CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_R);
		pWeapon
			->Set_Damage(7.f)
			->Set_Direction(Direction::Front)
			->Set_Power(Power::Medium)
			->Set_Force(0.0f);

		pWeapon->Set_Enable(true);
		pWeapon->Set_Enable_Collisions(false);
	}


}

CState<CPlayer>* CPlayer_MeleeFollowUp::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(5);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_L, true);
			m_pGameInstance->Play_Sound(L"PLAYER_WHOOSH", L"player_punch_basic_whoosh002.wav", CHANNELID::SOUND_PLAYER_WHOOSH2, 10.f);
		}
	}
	else if (false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta);

		if (pActor->Get_Target())
		{
			pActor->Play_Voice_Melee();
		}

		m_bFlags[1] = pActor->Is_Inputable_Front(9);
		if (true == m_bFlags[1])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_L, false);
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, true);
			pActor->Play_Whoosh_Sound();
		}
	}
	else if (false == m_bFlags[2])
	{
		pActor->Chasing_Attack(fTimeDelta);

		if (pActor->Get_Target())
		{
			pActor->Play_Voice_Melee();
		}

		m_bFlags[2] = pActor->Is_Inputable_Front(16);
		if (true == m_bFlags[2])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, false);
		}
	}
	else if (false == m_bFlags[3])
	{
		m_bFlags[3] = pActor->Is_Inputable_Front(23);
		//if (true == m_bFlags[3]) 
		//{
		//	
		//}
	}
	else 
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}



	//if (pActor->Is_Inputable_Front(23))
	//{
	//	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	//}
	return nullptr;
}

void CPlayer_MeleeFollowUp::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_PUNCH_L, false);
	pActor->Set_Weapon_Enable(PLAYER_WEAPON_PUNCH_R, false);
	pActor->Set_Target(nullptr);

	//CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
	//pWeapon->Set_Enable(false);
}
