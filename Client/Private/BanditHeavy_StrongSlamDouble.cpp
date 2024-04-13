#include "..\Public\BanditHeavy_StrongSlamDouble.h"
#include "Weapon_Heavy_Vampiric_Zombie.h"
void CBanditHeavy_StrongSlamDouble::Initialize(CBandit_Heavy* pActor)
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

CState<CBandit_Heavy>* CBanditHeavy_StrongSlamDouble::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(26);
		if (true == m_bFlags[0])
		{
			pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, true);
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(41);
		if (true == m_bFlags[1])
		{
			pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, false);
		}
	}
	else if (false == m_bFlags[2])
	{
		pActor->Look_At_Target_Lerp(fTimeDelta);

		m_bFlags[1] = pActor->Is_Inputable_Front(72);
		if (true == m_bFlags[2])
		{
			pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, true);
		}
	}
	else if (false == m_bFlags[3])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(81);
		if (true == m_bFlags[3])
		{
			pActor->Set_Weapon_Collisions_Enable(BANDIT_HEAVY_WEAPON, false);
		}
	}
	else 
	{
		pActor->Look_At_Target_Lerp(fTimeDelta);
	}
	//Effect 끄고 키는거임 
	if (m_bFlags[4] == false && pActor->Is_Inputable_Front(37))
	{
		CWeapon_Heavy_Vampiric_Zombie* pWeapon = dynamic_cast<CWeapon_Heavy_Vampiric_Zombie*>(pActor->Get_Weapon(BANDIT_HEAVY_WEAPON));
		pWeapon->m_bAttack = true;
		m_bFlags[4] = true;
	}

	if (m_bFlags[5] == false && pActor->Is_Inputable_Front(80))
	{
		CWeapon_Heavy_Vampiric_Zombie* pWeapon = dynamic_cast<CWeapon_Heavy_Vampiric_Zombie*>(pActor->Get_Weapon(BANDIT_HEAVY_WEAPON));
		pWeapon->m_bAttack = true;
		m_bFlags[5] = true;
	}
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CBanditHeavy_StrongSlamDouble::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}
