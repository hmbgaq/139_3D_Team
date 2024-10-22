#include "..\Public\Player_Shotgun_Fire_ShortRange.h"
#include "GameInstance.h"

#include "Player_Shotgun_WeaponHolster.h"


void CPlayer_Shotgun_Fire_ShortRange::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	//pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Shotgun_Fire_ShortRange::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Aim_Walk(fTimeDelta);

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_Animation_End())
	{
		return new CPlayer_Shotgun_WeaponHolster();
		//return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_Shotgun_Fire_ShortRange::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
