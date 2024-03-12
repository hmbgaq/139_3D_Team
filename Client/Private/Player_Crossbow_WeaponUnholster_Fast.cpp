#include "..\Public\Player_Crossbow_WeaponUnholster_Fast.h"
#include "GameInstance.h"
#include "Player_Crossbow_Ironsights_AimPose.h"

void CPlayer_Crossbow_WeaponUnholster_Fast::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Crossbow_WeaponUnholster_Fast::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Aim_Walk(fTimeDelta);

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Crossbow_Ironsights_AimPose();
	}

	return nullptr;
	
}

void CPlayer_Crossbow_WeaponUnholster_Fast::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
