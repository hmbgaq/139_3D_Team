#include "..\Public\Player_Revolver_WeaponUnholster.h"
#include "GameInstance.h"

#include "Player_Bandit_Special_01.h"
//#include "Player_Revolver_WeaponHolster.h"
//#include "Player_Revolver_Ironsights_ReloadFast.h"
//#include "Player_William_RevolverFanningStart_02.h"

void CPlayer_Revolver_WeaponUnholster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_REVOLVER, true);
	pWeapon->Play_Weapon_Sound_UnHolster();

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
}

CState<CPlayer>* CPlayer_Revolver_WeaponUnholster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Bandit_Special_01();
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	//if (pActor->Is_Animation_End())
	//{
	//	return new CPlayer_Bandit_Special_01();
	//}

	//return nullptr;
}

void CPlayer_Revolver_WeaponUnholster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
