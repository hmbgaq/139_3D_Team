#include "..\Public\Player_MeleeCombo_04.h"

#include "Player_Empowered_Idle.h"

void CPlayer_MeleeCombo_04::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 6);

	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_R);

	pWeapon
		->Set_Damage(10)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.3f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);
}

CState<CPlayer>* CPlayer_MeleeCombo_04::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(7);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, true);
			pActor->Play_Whoosh_Sound();

			if (pActor->Get_Target())
			{
				pActor->Play_Voice_Melee();
			}
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(13);
		if (true == m_bFlags[1])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, false);
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(16);
		//if (true == m_bFlags[2])
		//{

		//}
	}
	else 
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	

	//if (false == m_bFlags[0] && pActor->Is_Inputable_Front(7))
	//{
	//	CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, true);
	//	m_bFlags[0] = true;
	//}

	//if (true == m_bFlags[0] && false == m_bFlags[1])
	//{
	//	pActor->Chasing_Attack(fTimeDelta);
	//}

	//if (false == m_bFlags[1] && pActor->Is_Inputable_Front(13))
	//{
	//	CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, false);
	//	m_bFlags[1] = true;
	//}

	//if (pActor->Is_Inputable_Front(16))
	//{
	//	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	//}

	return nullptr;

}

void CPlayer_MeleeCombo_04::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_PUNCH_R, false);
	pActor->Set_Target(nullptr);
	//pWeapon->Set_Enable(false);
}
