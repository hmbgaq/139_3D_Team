#include "..\Public\Player_ShotgunElectric_Fire_ShortRange.h"
#include "GameInstance.h"

void CPlayer_ShotgunElectric_Fire_ShortRange::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_SHOTGUN, true);

	pActor->Set_UseMouseMove(false);
}

CState<CPlayer>* CPlayer_ShotgunElectric_Fire_ShortRange::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(9);
		if (true == m_bFlags[0]) 
		{
			CWeapon* pWeapon = pActor->Get_Weapon(PLAYER_WEAPON_SHOTGUN);
			pWeapon->Fire(_float3(-0.3f, 0.f, 1.f));

			pActor->Set_Animation(ECast(CPlayer::Player_State::Player_IdleLoop), CModel::ANIM_STATE_LOOP, true, false);
			pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
			
			pActor->Set_Splitted(true);

			pActor->Activate_ShootingReaction(30.f);
			pActor->Apply_Shake_And_Blur(Power::Medium);
		}
	}
	else if (false == m_bFlags[1]) 
	{
		pActor->Aim_Walk(fTimeDelta);
		m_bFlags[1] = pActor->Is_UpperAnimation_End();
	}
	else 
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

	

	//if (pActor->Is_Animation_End())
	//{
	//	return Normal(pActor, fTimeDelta, g_iAnimIndex);
	//}

	//return nullptr;

}

void CPlayer_ShotgunElectric_Fire_ShortRange::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
	pActor->Set_Weapon_Enable(PLAYER_WEAPON_SHOTGUN, false);
}
