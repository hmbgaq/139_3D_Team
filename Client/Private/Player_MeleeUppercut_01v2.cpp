#include "Player_MeleeUppercut_01v2.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Clone_Manager.h"
#include "Effect.h"
#include "Bone.h"

#include "Player_Leap_01_Lower.h"

void CPlayer_MeleeUppercut_01v2::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Get_Weapon(WEAPON_PUNCH_R);

	pWeapon
		->Set_Damage(0.f)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Heavy)
		->Set_Force(0.0f);

	pWeapon->Set_Enable(true);
	pWeapon->Set_Enable_Collisions(false);

}

CState<CPlayer>* CPlayer_MeleeUppercut_01v2::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0] && pActor->Is_Inputable_Front(13))
	{
		CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(WEAPON_PUNCH_R, true);
		m_bFlags[0] = true;
	}

	if (true == m_bFlags[0] && false == m_bFlags[1])
	{
		pActor->Chasing_Attack(fTimeDelta, 5.f, 1);
	}

	if (false == m_bFlags[1] && pActor->Is_Inputable_Front(23))
	{
		CWeapon* pWeapon = pActor->Set_Weapon_Collisions_Enable(WEAPON_PUNCH_R, false);
		m_bFlags[1] = true;
	}

	if (pActor->Is_Inputable_Front(29))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_LB))
		{
			return new CPlayer_Leap_01_Lower();
		}

		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

}

void CPlayer_MeleeUppercut_01v2::Release(CPlayer* pActor)
{
	__super::Release(pActor);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(WEAPON_PUNCH_R, false);
	pActor->Set_Target(nullptr);
}
