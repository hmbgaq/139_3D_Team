#include "..\Public\Player_Rifle_IdleWeaponHolster.h"
#include "GameInstance.h"
#include "Player_IdleLoop.h"

void CPlayer_Rifle_IdleWeaponHolster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Rifle_IdleWeaponHolster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);
	
	if (pActor->Is_UpperAnimation_End())
	{
		return Normal_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_Rifle_IdleWeaponHolster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
