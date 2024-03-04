#include "..\Public\Player_Rifle_Ironsights_Fire.h"
#include "GameInstance.h"

void CPlayer_Rifle_Ironsights_Fire::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(CPlayer::Player_State::Player_Rifle_IdleWeaponHolster, CModel::ANIM_STATE_STOP);
	pActor->Set_Splitted(true);
}

CState<CPlayer>* CPlayer_Rifle_Ironsights_Fire::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Rifle_Ironsights_Fire::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}
