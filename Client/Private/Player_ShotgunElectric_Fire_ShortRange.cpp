#include "..\Public\Player_ShotgunElectric_Fire_ShortRange.h"
#include "GameInstance.h"

void CPlayer_ShotgunElectric_Fire_ShortRange::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	//pActor->Set_Splitted(true);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_SHOTGUN, true);
}

CState<CPlayer>* CPlayer_ShotgunElectric_Fire_ShortRange::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//if ()

	//pActor->Aim_Walk(fTimeDelta);

	//if (pActor->Is_UpperAnimation_End())
	//{
	//	return Normal(pActor, fTimeDelta, g_iAnimIndex);
	//}

	//return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_Animation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_ShotgunElectric_Fire_ShortRange::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_SHOTGUN, false);
}
