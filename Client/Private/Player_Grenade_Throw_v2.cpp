#include "..\Public\Player_Grenade_Throw_v2.h"
#include "GameInstance.h"
//#include "Player_IdleLoop.h"

void CPlayer_Grenade_Throw_v2::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_Splitted(false);

	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	//pActor->Set_Splitted(true);

	//pActor->Set_Weapon_Enable(PLAYER_WEAPON_DYNAMITE, false);
}

CState<CPlayer>* CPlayer_Grenade_Throw_v2::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Aim_Walk(fTimeDelta);

	//if (pActor->Is_UpperAnimation_End())
	//{
	//	return Normal(pActor, fTimeDelta, g_iAnimIndex);
	//}

	//return nullptr;


	if (pActor->Is_Animation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Grenade_Throw_v2::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
