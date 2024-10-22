#include "..\Public\Player_Revolver_WeaponHolster.h"
#include "GameInstance.h"

#include "Player_IdleLoop.h"

void CPlayer_Revolver_WeaponHolster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_REVOLVER, true);
	pWeapon->Play_Weapon_Sound_Holster();
}

CState<CPlayer>* CPlayer_Revolver_WeaponHolster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;


	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	//if (pActor->Is_Animation_End())
	//{
	//	return new CPlayer_IdleLoop();
	//}

	//return nullptr;
}

void CPlayer_Revolver_WeaponHolster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
