#include "Player_MeleeKick.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_MeleeKick::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_KICK);
	pWeapon
		->Set_Damage(0)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.2f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);
}

CState<CPlayer>* CPlayer_MeleeKick::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0]) 
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(14);
		if (true == m_bFlags[0])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_KICK, true);
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(22);
		if (true == m_bFlags[1])
		{
			pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_KICK, false);
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = true;
	}

	if (pActor->Is_Inputable_Front(31))
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
		//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CPlayer_MeleeKick::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_KICK, false);
	pActor->Set_Target(nullptr);
}
