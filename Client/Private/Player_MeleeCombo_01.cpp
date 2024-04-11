#include "Player_MeleeCombo_01.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"

void CPlayer_MeleeCombo_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	string Test = "Data_Animation/";
	pActor->Set_EventNotify(Test, "Test2_AnimationData.json");

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_R);
	pWeapon
		->Set_Damage(10)
		->Set_Direction(Direction::Right)
		->Set_Power(Power::Medium)
		->Set_Force(0.0f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);

}

CState<CPlayer>* CPlayer_MeleeCombo_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(16);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, true);
		}
	}
	else if (false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta);
		m_bFlags[1] = pActor->Is_Inputable_Front(24);
		if (true == m_bFlags[1])
		{
			CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, false);
		}

	}
	else
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}


	//if (true == m_bFlags[0] && false == m_bFlags[1]) 
	//{
	//	pActor->Chasing_Attack(fTimeDelta);
	//}

	//if (false == m_bFlags[1] && pActor->Is_Inputable_Front(23)) 
	//{
	//	CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(PLAYER_WEAPON_PUNCH_R, false);
	//	m_bFlags[1] = true;
	//}

	//if (pActor->Is_Inputable_Front(23))
	//{
	//	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	//}

	return nullptr;
}

void CPlayer_MeleeCombo_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_PUNCH_R, false);
	pActor->Set_Target(nullptr);
}



//if (false == m_bFlags[0] && pActor->Check_EffectOnTrackPosition())
//{
//	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_PUNCH_R);
//	_float3 vPos = pWeapon->Get_WorldPosition();

//	pActor->Create_Effect(vPos);

//	m_bFlags[0] = true;
//}