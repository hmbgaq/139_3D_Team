#include "Player_MeleeSlashAlt_01.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_MeleeSlashAlt_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 0);


	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_R);

	pWeapon
		->Set_Damage(15.f)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Medium)
		->Set_Force(0.0f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);

}

CState<CPlayer>* CPlayer_MeleeSlashAlt_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	
	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(16);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, true);
			pActor->Play_Whoosh_Sound();
		}
	}
	else if (false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta);
		m_bFlags[1] = pActor->Is_Inputable_Front(23);
		if (true == m_bFlags[1])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, false);
		}

	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(30);
		//if (true == m_bFlags[2])
		//{
		//	
		//}

	}
	else
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}



	//if (pActor->Is_Inputable_Front(28))
	//{
	//	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	//}

	return nullptr;
}

void CPlayer_MeleeSlashAlt_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_PUNCH_R, false);
	pActor->Set_Target(nullptr);

}
