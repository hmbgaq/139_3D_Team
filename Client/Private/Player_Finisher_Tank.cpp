#include "..\Public\Player_Finisher_Tank.h"

void CPlayer_Finisher_Tank::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_SHOTGUN, true);
}

CState<CPlayer>* CPlayer_Finisher_Tank::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Finisher_Tank::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Weapon_Enable(PLAYER_WEAPON_SHOTGUN, false);
}
