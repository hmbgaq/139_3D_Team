#include "..\Public\Player_Shotgun_WeaponHolster.h"
#include "GameInstance.h"

void CPlayer_Shotgun_WeaponHolster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	//pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Shotgun_WeaponHolster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	//pActor->Aim_Walk(fTimeDelta);

	//if (pActor->Is_UpperAnimation_End())
	//{
	//	return Normal(pActor, fTimeDelta, g_iAnimIndex);
	//}

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);

	if (pActor->Is_Animation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_Shotgun_WeaponHolster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
