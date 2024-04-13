#include "..\Public\BanditHeavy_MeleeHeavy_02.h"
#include "Weapon_Heavy_Vampiric_Zombie.h"

void CBanditHeavy_MeleeHeavy_02::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(BANDIT_HEAVY_WEAPON, true);
	pWeapon
		->Set_Damage(20)
		->Set_Direction(Direction::Front)
		->Set_Power(Power::Heavy)
		->Set_Force(0.5f)
		;

	pWeapon->Set_Enable_Collisions(false);
}

CState<CBandit_Heavy>* CBanditHeavy_MeleeHeavy_02::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(20);
		if (true == m_bFlags[0])
		{
			pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, true);
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(50);
		if (true == m_bFlags[1])
		{
			pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, false);
		}
	}
	if (m_bFlags[2] == false && pActor->Is_Inputable_Front(41))
	{
		CWeapon_Heavy_Vampiric_Zombie* pWeapon = dynamic_cast<CWeapon_Heavy_Vampiric_Zombie*>(pActor->Get_Weapon(BANDIT_HEAVY_WEAPON));
		pWeapon->m_bAttack = true;
		m_bFlags[2] = true;
	}
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_MeleeHeavy_02::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
