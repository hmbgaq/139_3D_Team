#include "..\Public\Player_InteractionWhipSwing.h"

void CPlayer_InteractionWhipSwing::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 12);
	pActor->Set_UseGravity(false);
}

CState<CPlayer>* CPlayer_InteractionWhipSwing::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_WHIP);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(12);
		if (true == m_bFlags[0])
		{
			pWeapon->Set_Enable(true);
			pWeapon->Set_Animation(0, CModel::ANIM_STATE::ANIM_STATE_NORMAL, 10);
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(35);//35
		if (true == m_bFlags[1])
		{
			pWeapon->Set_Enable(true);
			pWeapon->Set_Animation(0, CModel::ANIM_STATE::ANIM_STATE_NORMAL, 26);//24
		}
	}
	else if (false == m_bFlags[2])
	{
		m_bFlags[2] = pActor->Is_Inputable_Front(47);
		if (true == m_bFlags[2])
		{
			pWeapon->Set_Enable(false);
		}
	}
	else if (false == m_bFlags[3])
	{
	
		m_bFlags[3] = pActor->Is_Inputable_Front(58);
		if (true == m_bFlags[3])
		{
			pActor->Set_UseGravity(true);
		}
	}


	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_InteractionWhipSwing::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_WHIP, false);
}
