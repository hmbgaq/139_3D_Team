#include "..\Public\Player_MeleeCombo_02.h"

#include "Player_Empowered_Idle.h"

void CPlayer_MeleeCombo_02::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 5);


	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_L);

	pWeapon
		->Set_Damage(10)
		->Set_Direction(Direction::Left)
		->Set_Power(Power::Medium)
		->Set_Force(0.3f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);
}

CState<CPlayer>* CPlayer_MeleeCombo_02::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(5);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_L, true);
			pActor->Play_Whoosh_Sound();
		}
	}
	else if (false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta);
		m_bFlags[1] = pActor->Is_Inputable_Front(14);
		if (true == m_bFlags[1])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_L, false);
		}
	}
	else 
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}


	//if (false == m_bFlags[0] && pActor->Is_Inputable_Front(5))
	//{
	//	CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_L, true);
	//	m_bFlags[0] = true;
	//}

	//if (true == m_bFlags[0] && false == m_bFlags[1])
	//{
	//	pActor->Chasing_Attack(fTimeDelta);
	//}

	//if (false == m_bFlags[1] && pActor->Is_Inputable_Front(13))
	//{
	//	CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_L, false);
	//	m_bFlags[1] = true;
	//}

	//if (pActor->Is_Inputable_Front(13 - 4))
	//{
	//	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	//}

	return nullptr;
}

void CPlayer_MeleeCombo_02::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_PUNCH_L, false);
	pActor->Set_Target(nullptr);
	//pWeapon->Set_Enable(false);
}
