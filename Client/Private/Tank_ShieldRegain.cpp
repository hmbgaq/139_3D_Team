#include "..\Public\Tank_ShieldRegain.h"
#include "Weapon_Tank.h"

void CTank_ShieldRegain::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	CWeapon* pWeapon = pActor->Set_Weapon_Enable(L"Weapon_Shield", true);
	CWeapon_Tank* pWeapon_Tank = dynamic_cast<CWeapon_Tank*>(pWeapon);
	if (pWeapon_Tank)
	{
		pWeapon_Tank->Set_Follow(true);
		pWeapon_Tank->Set_Animation(0, CModel::ANIM_STATE_REVERSE, 28);
	}

	pActor->Set_MonsterAttackState(false);

	pActor->Play_Sound_Shield_PickUp();
}

CState<CTank>* CTank_ShieldRegain::Update(CTank* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CTank_ShieldRegain::Release(CTank* pActor)
{
	__super::Release(pActor);
}
