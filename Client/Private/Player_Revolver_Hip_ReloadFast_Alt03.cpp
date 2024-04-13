#include "..\Public\Player_Revolver_Hip_ReloadFast_Alt03.h"
#include "GameInstance.h"

#include "Player_IdleLoop.h"
#include "Player_Revolver_WeaponHolster.h"


void CPlayer_Revolver_Hip_ReloadFast_Alt03::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_REVOLVER, true);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);


}

CState<CPlayer>* CPlayer_Revolver_Hip_ReloadFast_Alt03::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);


	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Upper_Inputable_Front(22);
		if (true == m_bFlags[0])
		{
			CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_REVOLVER);
			pWeapon->Play_Weapon_Sound_Reload();

		}
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Upper_Inputable_Front(38);
		if (true == m_bFlags[1])
		{
			m_pGameInstance->Stop_Sound(CHANNELID::SOUND_PLAYER_WEAPON);
		}
	}
	//else if (false == m_bFlags[2])
	//{
	//	m_bFlags[2] = pActor->Is_UpperAnimation_End();
	//	if (true == m_bFlags[2])
	//	{

	//	}
	//}
	//else if (false == m_bFlags[3])
	//{
	//	m_bFlags[3] = pActor->Is_UpperAnimation_End();
	//	if (true == m_bFlags[3])
	//	{

	//	}
	//}
	//else if (false == m_bFlags[4])
	//{
	//	m_bFlags[4] = pActor->Is_UpperAnimation_End();
	//	if (true == m_bFlags[4])
	//	{

	//	}
	//}
	//else if (false == m_bFlags[5])
	//{
	//	m_bFlags[5] = pActor->Is_UpperAnimation_End();
	//	if (true == m_bFlags[5])
	//	{

	//	}
	//}

	else if (pActor->Is_UpperAnimation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
		//return new CPlayer_IdleLoop();
		//return new CPlayer_Revolver_WeaponHolster();
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);


	//if (pActor->Is_Animation_End())
	//{
	//	return new CPlayer_IdleLoop();
	//	//return new CPlayer_Revolver_WeaponHolster();
	//}

	//return nullptr;
}

void CPlayer_Revolver_Hip_ReloadFast_Alt03::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
	pActor->Set_Weapon_Enable(PLAYER_WEAPON_REVOLVER, false);
	pActor->Set_HUD_Cooltime(pActor->Get_Skill_HUD_Enum(CPlayer::Player_Skill::REVOLVER), 0.f);
	m_pGameInstance->Stop_Sound(CHANNELID::SOUND_PLAYER_WEAPON);
}
