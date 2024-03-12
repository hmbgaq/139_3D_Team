#include "..\Public\Player_Grenade_WeaponHolster.h"
#include "GameInstance.h"

void CPlayer_Grenade_WeaponHolster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Grenade_WeaponHolster::Update(CPlayer* pActor, _float fTimeDelta)
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
}

void CPlayer_Grenade_WeaponHolster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
