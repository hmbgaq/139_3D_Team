#include "..\Public\Player_Winchester_Ironsights_Reload_01.h"
#include "GameInstance.h"

#include "Player_Winchester_Ironsights_AimPose.h"

void CPlayer_Winchester_Ironsights_Reload_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, true);
	//pWeapon->Play_Weapon_Sound_Reload();
}

CState<CPlayer>* CPlayer_Winchester_Ironsights_Reload_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Upper_Inputable_Front(22);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_WINCHESTER);
			pWeapon->Play_Weapon_Sound_Holster();
		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Upper_Inputable_Front(45);
		if (true == m_bFlags[1]) 
		{
			CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_WINCHESTER);
			pWeapon->Play_Weapon_Sound_Reload();
		}
	}
	else if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Winchester_Ironsights_AimPose();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_Ironsights_Reload_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
