#include "..\Public\Player_FlameBelcher_WeaponHolster.h"
#include "GameInstance.h"

void CPlayer_FlameBelcher_WeaponHolster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_FlameBelcher_WeaponHolster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (pActor->Is_UpperAnimation_End())
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_FlameBelcher_WeaponHolster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
}
