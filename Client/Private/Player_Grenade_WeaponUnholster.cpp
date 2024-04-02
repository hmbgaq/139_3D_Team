#include "..\Public\Player_Grenade_WeaponUnholster.h"
#include "GameInstance.h"
#include "Player_Grenade_Throw_v2.h"

void CPlayer_Grenade_WeaponUnholster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	//pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	//pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	//pActor->Set_Splitted(true);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL);
	pActor->Set_Splitted(true);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_DYNAMITE, true);
}

CState<CPlayer>* CPlayer_Grenade_WeaponUnholster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	if (m_pGameInstance->Mouse_Down(DIM_LB) && pActor->Is_UpperAnimation_End())
	{
		return new CPlayer_Grenade_Throw_v2();
	}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Grenade_WeaponUnholster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	//pActor->Set_Splitted(false);
}
